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
//        Acredito que não será necessário, dado nosso contexto
//        if (original.get_total_bits() != processed.get_total_bits()) {
//            throw std::invalid_argument("Erro: Buffers de tamanhos diferentes não podem ser comparados.");
//        }

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
     * @brief Executa uma simulação completa de transmissão de dados via código de Hamming.
     * @details
     * Esta função realiza o fluxo fim-a-fim: exibição dos dados originais,
     * codificação Hamming, simulação de ruído branco (AWGN) no canal e decodificação
     * para tentativa de recuperação dos bits.
     * Se nenhum buffer for fornecido, a função executa um "caso padrão" de 4 bits (0010).
     * @param SNR_db Relação Sinal-Ruído (SNR) em decibéis para a simulação do canal.
     * @param custom_db Ponteiro opcional para um DataBuffer. Se for nullptr, usa o buffer padrão.
     */
    static void demo(const double& SNR_db, const DataBuffer* input_db = nullptr){

        DataBuffer default_db(4);
        if(input_db == nullptr){
            default_db.set_bit(0, 0);
            default_db.set_bit(1, 0);
            default_db.set_bit(2, 1);
            default_db.set_bit(3, 0);
        }

        const DataBuffer& db = (input_db == nullptr) ? default_db : *input_db;

        std::cout << "Enviei: ";
                  db.print_bit_range(0, 50); // Mostraremos no máximo os 50 primeiros bits, a experiência de mostrar todos não é agradável
        std::cout << std::endl;

        Hamming h("key.txt");

        auto sended_bits = h.encode(db);
        std::cout << "Codifiquei: ";
                  sended_bits->print_bit_range(0, 50); // Mostraremos no máximo os 50 primeiros bits, a experiência de mostrar todos não é agradável
        std::cout << std::endl;

        NoiseChannel::transmit(*sended_bits, SNR_db);
        std::cout << "Após Efeito de Canal: ";
                  sended_bits->print_bit_range(0, 50); // Mostraremos no máximo os 50 primeiros bits, a experiência de mostrar todos não é agradável
        std::cout << std::endl;

        auto received_bits = h.decode(*sended_bits);
        std::cout << "Recebi: ";
                  received_bits->print_bit_range(0, 50); // Mostraremos no máximo os 50 primeiros bits, a experiência de mostrar todos não é agradável
        std::cout << std::endl;

        std::cout << "BER -> " << Manager::calculate_ber(db, *received_bits) << std::endl;
    }

    static void extract_ber(std::ofstream& file, const DataBuffer& input){

        const int SNR_DB_MAX = 10;
        const int ITERATIONS = 100;


        // Independe dos processos sucessivos
        Hamming h("key.txt");
        std::unique_ptr<DataBuffer> sended_bits = h.encode(input);

        DataBuffer noisy_work_buffer(sended_bits->get_total_bits());

        file << "SNR_DB,BER\n";
        for(double SNR_DB = 0; SNR_DB < SNR_DB_MAX; SNR_DB += 0.25){
            double mean_ber = 0;

            for(int i = 0; i < ITERATIONS; ++i){

                noisy_work_buffer.copy_from(*sended_bits);

                // Aplicamos o ruído sempre à sequência original
                NoiseChannel::transmit(noisy_work_buffer, SNR_DB);

                std::unique_ptr<DataBuffer> received_bits = h.decode(noisy_work_buffer);

                mean_ber += Manager::calculate_ber(input, *received_bits);
            }

            mean_ber /= ITERATIONS;

            file << SNR_DB << "," << mean_ber << '\n';
        }
    }
};
