#pragma once

#include <iostream>
#include <fstream>
#include <memory>

#include "../DataBuffer/DataBuffer.hpp"
#include "../NoiseChannel/NoiseChannel.hpp"
#include "../Hamming/Hamming.hpp"

class Manager {
public:
    /**
     * @brief Compara dois buffers bit a bit, conta o total de divergências e calcula a BER
     * @details
     * Útil para calcular a taxa de erro de bit (BER) após a transmissão ou decodificação.
     * @param original Buffer com os dados de referência (originais).
     * @param processed Buffer com os dados que passaram pelo canal e pelo decoder.
     * @return double BER
     */
    static double calculate_ber(const DataBuffer& original, const DataBuffer& processed) {
        double error_count = 0;
        double total_bits = original.get_total_bits();

        for (size_t i = 0; i < total_bits; ++i) {
            if (original.get_bit(i) != processed.get_bit(i)) {
                error_count++;
            }
        }

        return error_count / total_bits;
    }

    /**
     * @brief Executa uma simulação completa de transmissão com codificação Hamming e ruído AWGN.
     * @details
     * Esta função demonstra o fluxo completo de comunicação digital:
     * 1. Exibe os bits originais de entrada
     * 2. Codifica os bits usando código de Hamming
     * 3. Simula transmissão através de canal com ruído AWGN
     * 4. Decodifica os bits recebidos
     * 5. Calcula e exibe a taxa de erro de bit (BER)
     * 6. Salva os bits decodificados em arquivo
     */
    static void execute(const double& SNR_DB, const DataBuffer& input){

        const int MAX_QUANT_BITS_SHOW = 20;

        std::cout << "Enviei: ";
        input.print_bit_range(0, MAX_QUANT_BITS_SHOW);
        std::cout << std::endl;

        Hamming Master("key.txt");

        std::unique_ptr<DataBuffer> sended_bits = Master.encode(input);
        std::cout << "Após codificar: ";
        sended_bits->print_bit_range(0, MAX_QUANT_BITS_SHOW);
        std::cout << std::endl;

        ///
        NoiseChannel::transmit(*sended_bits, SNR_DB);
        ///
        std::cout << "Após Efeito de Canal: ";
        sended_bits->print_bit_range(0, MAX_QUANT_BITS_SHOW);
        std::cout << std::endl;

        std::unique_ptr<DataBuffer> received_bits = Master.decode(*sended_bits);
        std::cout << "Recebi: ";
        received_bits->print_bit_range(0, MAX_QUANT_BITS_SHOW); // Mostraremos no máximo os 50 primeiros bits, a experiência de mostrar todos não é agradável
        std::cout << std::endl;

        received_bits->save("result.out");

        std::cout << "BER -> " << Manager::calculate_ber(input, *received_bits) << std::endl;
    }
};
