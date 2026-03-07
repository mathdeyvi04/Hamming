#pragma once

#include <iostream>
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

#include "../DataBuffer/DataBuffer.hpp"

/**
 * @brief Responsável por implementar o Código de Hamming, seja na saída ou na recepção.
 */
class Hamming {
private:
    Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> __generator_matrix; /**< Matriz G (k x n). */

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

        /* Atribuição */
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

        file.close();
    }

    /**
     * @brief Codifica um DataBuffer de entrada gerando um novo com palavras-código.
     * @details
     * O processo divide a sequência de bits original em blocos de tamanho 'k'
     * (linhas da matriz), realiza a multiplicação matricial pela matriz geradora G
     * (k x n) e aplica a operação módulo 2 para obter a palavra-código de 'n' bits.
     * @param input Referência para o data_buffer contendo os dados originais.
     * @return std::unique_ptr<data_buffer> Ponteiro para o novo buffer codificado.
     */
     std::unique_ptr<DataBuffer> encode(const DataBuffer& input) const {
        const size_t k_bits = static_cast<size_t>(this->__generator_matrix.rows());
        const size_t n_bits = static_cast<size_t>(this->__generator_matrix.cols());

        size_t total_input_bits = input.get_total_bits();
        size_t numblocks = (total_input_bits + k_bits - 1) / k_bits;
        size_t total_output_bits = numblocks * n_bits;

        std::unique_ptr<DataBuffer> output = std::make_unique<DataBuffer>(total_output_bits / 8);

        for(size_t b = 0; b < numblocks; ++b){

            /* Criamos e Populamos um Vetor de Modo Compatível com Eigen */
            // Usamos int para evitar overflow
            Eigen::VectorXi data_vector(k_bits);

            for(size_t i = 0; i < k_bits; ++i){
                size_t bit_idx = b * k_bits + i;
                data_vector(i) = (bit_idx < total_input_bits) ? input.get_bit(bit_idx) : 0;
            }

            /* Operação Hamming */
            // Usamos int para precisão
            Eigen::VectorXi codeword = (data_vector.transpose() * this->__generator_matrix.cast<int>());

            /* Populamos o DataBuffer de Saída */
            for (size_t i = 0; i < n_bits; ++i) {
                uint8_t bit_final = static_cast<uint8_t>(codeword(i) % 2);
                output->set_bit(b * n_bits + i, bit_final);
            }
        }

        return output;
     }
};