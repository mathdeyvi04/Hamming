#pragma once

#include <iostream>
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

#include "../DataBuffer/DataBuffer.hpp"

/**
 * @brief Responsável por implementar o Código de Hamming, seja na saída ou na recepção.
 */
class Hamming {
private:
    Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> __generator_matrix; /**< Matriz G (k x n). */
    Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> __parity_matrix;

public:
    /**
     * @brief Construtor que carrega a matriz geradora a partir de um arquivo .txt.
     * @details
     * O arquivo deve conter a matriz em formato de texto, ex:
     * 1 0 0 0 1 1 0
     * 0 1 0 0 1 0 1
     * ...
     * @param matrix_path Caminho para o arquivo .txt da matriz.
     */
    Hamming(const std::string& matrix_path) {
        std::ifstream file(matrix_path);
        if (!file.is_open()) {
            throw std::runtime_error("Erro ao abrir matriz geradora.");
        }

        /* Determinação de Dimensões */
        size_t rows = 0;
        size_t cols = 0;
        std::string line;

        if(std::getline(file, line)){
            rows++;
            std::stringstream ss(line);
            int temp_val;
            while(ss >> temp_val){
                cols++;
            }
        }
        while(std::getline(file, line)){
            if (!line.empty()) {
                rows++;
            }
        }

        /* Atribuição da Matriz Geradora */
        this->__generator_matrix.resize(rows, cols);
        file.clear();
        file.seekg(0, std::ios::beg);
        for(size_t i = 0; i < rows; ++i){
            for(size_t j = 0; j < cols; ++j){
                // Lemos como inteiro, mas salvaremos como uint8_t
                int val;
                if(!(file >> val)){
                    throw std::runtime_error("Erro: Matriz com formato inconsistente.");
                }
                this->__generator_matrix(i, j) = static_cast<uint8_t>(val);
            }
        }

        /* Atribuição da Matriz de Paridade */
        size_t m = cols - rows;
        this->__parity_matrix.resize(m, cols);

        for(size_t i = 0; i < m; ++i){
            for(size_t j = 0; j < rows; ++j){
                this->__parity_matrix(i, j) = this->__generator_matrix(j, rows + i);
            }
            for(size_t j = rows; j < cols; j++){
                this->__parity_matrix(i, j) = ((j - rows) == i) ? 1 : 0;
            }
        }

        file.close();
    }

    /**
     * @brief Apresentará as matrizes
     * @param id_matrix Positivo mostra Matriz Geradora. Negativo mostra Matriz de Paridade.
     */
    void show(int id_matrix){
        std::cout <<
                  ((id_matrix > 0) ? this->__generator_matrix : this->__parity_matrix)
                  << std::endl;
    }

    /**
     * @brief Codifica um DataBuffer de entrada gerando um novo com palavras-código.
     * @param input Referência para o data_buffer contendo os dados originais.
     * @return std::unique_ptr<data_buffer> Ponteiro para o novo buffer codificado.
     */
     std::unique_ptr<DataBuffer> encode(const DataBuffer& input) const {
        const size_t k_bits = static_cast<size_t>(this->__generator_matrix.rows());
        const size_t n_bits = static_cast<size_t>(this->__generator_matrix.cols());

        size_t total_input_bits = input.get_total_bits();
        size_t numblocks = (total_input_bits + k_bits - 1) / k_bits;
        size_t total_output_bits = numblocks * n_bits;

        std::unique_ptr<DataBuffer> output = std::make_unique<DataBuffer>(total_output_bits);

        for(size_t b = 0; b < numblocks; ++b) {
            // Para cada coluna 'j' da matriz geradora (cada bit da palavra-código de saída)
            for(size_t j = 0; j < n_bits; ++j) {
                int accumulator = 0;

                // Multiplicação da linha de dados pela coluna 'j' da matriz G
                for(size_t i = 0; i < k_bits; ++i) {
                    size_t bit_idx = b * k_bits + i;

                    // Pegamos o bit "on-the-fly" do buffer original
                    uint8_t data_bit = (bit_idx < total_input_bits) ? input.get_bit(bit_idx) : 0;

                    // Operação de produto: data_bit * G(i, j)
                    // Como data_bit é 0 ou 1, isso equivale a um AND lógico ou uma soma condicional
                    accumulator += data_bit * this->__generator_matrix(i, j);
                }

                // Aplica o módulo 2 (GF2) e seta o bit diretamente no destino
                output->set_bit(b * n_bits + j, static_cast<uint8_t>(accumulator % 2));
            }
        }

        return output;
     }

    /**
     * @brief Decodifica um buffer ruidoso, corrigindo erros de 1 bit por bloco.
     * @param input Buffer contendo as palavras-código (n bits cada).
     * @return std::unique_ptr<DataBuffer> Dados originais recuperados (k bits cada).
     */
     std::unique_ptr<DataBuffer> decode(const DataBuffer& input) const {
        const size_t n = static_cast<size_t>(this->__parity_matrix.cols());
        const size_t m = static_cast<size_t>(this->__parity_matrix.rows());
        const size_t k = static_cast<size_t>(this->__generator_matrix.rows());

        const size_t total_input_bits = input.get_total_bits();
        const size_t numblocks = total_input_bits / n;

        std::unique_ptr<DataBuffer> output = std::make_unique<DataBuffer>(numblocks * k);

        /* Para posterior uso, note como já fizemos a alocação */
        std::vector<uint8_t> syndrome(m, 0);
        for(size_t b = 0; b < numblocks; b++){

            /* Cálculo da Síndrome */
            for(size_t i = 0; i < m; ++i){
                int acc = 0;
                for(size_t j = 0; j < n; ++j){
                    // Multiplicação bit a bit
                    acc += input.get_bit(b * n + j) * this->__parity_matrix(i , j);
                }
                syndrome[i] = static_cast<uint8_t>(acc % 2);
            }

            /* Verificação se há erro */
            int error_bit_index = -1; // -1 Significará que não há erro
            for(const uint8_t& s_bit : syndrome){
                if(s_bit != 0){ error_bit_index = 0; break; } // Então levamos à 0 para identificar que houve erro
            }

            if(!error_bit_index){
                // (Propriedade)
                /* Se a síndrome não é zero, ela deve ser idêntica a uma das colunas de H*/
                for(size_t col = 0; col < n; ++col){
                    bool match = true;
                    for(size_t row = 0; row < m; ++row){
                        if(this->__parity_matrix(row, col) != syndrome[row]){
                            match = false;
                            break;
                        }
                    }

                    if(match){
                        error_bit_index = static_cast<int>(col);
                        break;
                    }
                }
            }

            /* Correção e Extração de Dados */
            for(size_t i = 0; i < k; ++i){
                uint8_t bit = input.get_bit(b * n + i);

                if(error_bit_index == static_cast<int>(i)){
                    bit = (bit == 1) ? 0 : 1;
                }

                output->set_bit(b * k + i, bit);
            }

            /**
             * Se o erro estiver entre k e n, significa que o erro ocorreu em um bit
             * de paridade. Como só nos interessamos pelos
             * bits de dados (0 a k-1), esses erros são corrigidos "silenciosamente"
             * apenas por não serem incluídos na extração.
             */
        }

        return output;
     }
};