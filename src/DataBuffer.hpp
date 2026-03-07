#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <cstdint>
#include <stdexcept>

/**
 * @brief Gerenciador de memória de alta performance para leitura de arquivos binários.
 * @details
 * Esta classe carrega qualquer tipo de arquivo (.txt, .mp4, .png, etc.) para um buffer
 * contíguo de bytes e permite o acesso individual a cada bit de forma otimizada.
 */
class DataBuffer {
private:
    std::unique_ptr<uint8_t[]> __data; /**< Smart Pointer para o buffer de bytes brutos. */
    size_t __file_size;                 /**< Tamanho total do arquivo em bytes. */
    size_t __total_bits;                /**< Quantidade total de bits disponíveis para processamento. */

public:
    /**
     * @brief Construtor que carrega um arquivo para a memória.
     * @details
     * Abre o arquivo em modo binário, calcula seu tamanho e aloca memória
     * suficiente para armazenar o conteúdo completo.
     * @param file_path Caminho relativo ou absoluto do arquivo a ser lido.
     * @throws std::runtime_error Caso o arquivo não possa ser aberto ou lido.
     */
    DataBuffer(const std::string& file_path) {
        std::ifstream file(
            file_path,
            // Abrimos de forma binária e colocamos o ponteiro no fim do arquivo
            std::ios::binary | std::ios::ate
        );

        if (!file.is_open()) {
            throw std::runtime_error("Erro: Falha ao abrir o arquivo especificado.");
        }

        this->__file_size = file.tellg();
        this->__total_bits = this->__file_size * 8;

        // Alocação de memória bruta para evitar overhead de containers complexos
        this->__data = std::make_unique<uint8_t[]>(this->__file_size);

        // Movemos o ponteiro de volta ao início
        file.seekg(0, std::ios::beg);
        // Carregamos o arquivo inteiro na RAM <-- CUIDADO
        file.read(reinterpret_cast<char*>(this->__data.get()), this->__file_size);
        file.close();
    }

    /**
     * @brief Recupera o valor de um bit específico na sequência global.
     * @details
     * Utiliza operações de bitwise (deslocamento e máscara) para extrair o bit
     * sem a necessidade de converter o byte inteiro, garantindo performance O(1).
     * @param bitIndex O índice do bit desejado (0 até __total_bits - 1).
     * @return uint8_t Retorna 0 ou 1.
     * @throws std::out_of_range Se o índice solicitado for maior que o total de bits.
     */
    inline uint8_t get_bit(size_t bit_index) const {
        if (bit_index >= this->__total_bits) {
            throw std::out_of_range("Erro: Índice de bit fora dos limites do buffer.");
        }

        // byte_idx identifica em qual byte o bit está
        size_t byte_idx = bit_index / 8;

        // bit_pos identifica a posição dentro do byte (da esquerda para a direita / MSB)
        uint8_t bit_pos = 7 - (bit_index % 8);

        return (this->__data[byte_idx] >> bit_pos) & 1;
    }

    /**
     * @brief Obtém o número total de bits carregados.
     * @return size_t Total de bits.
     */
    size_t get_total_bits() const { return this->__total_bits; }

    /**
     * @brief Obtém o tamanho do arquivo original em bytes.
     * @return size_t Tamanho em bytes.
     */
    size_t get_file_size() const { return this->__file_size; }

    /**
     * @brief Imprime no console uma sequência de bits para fins de depuração.
     * @details
     * É garantido que não haverão erros no caso de count ser maior que o total de bits
     * @param start Índice inicial do bit.
     * @param count Quantidade de bits a serem exibidos.
     */
    void print_bit_range(size_t start, size_t count) const {
        for (size_t i = start; i < start + count && i < this->__total_bits; ++i) {
            std::cout << static_cast<int>(this->get_bit(i));
        }
        std::cout << std::endl;
    }
};




