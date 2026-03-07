#pragma once

#include <random>
#include <cmath>
#include <memory>

#include "../DataBuffer/DataBuffer.hpp"

/**
 * @brief Simula um canal de comunicação com ruído branco gaussiano aditivo (AWGN).
 */
class NoiseChannel {
public:

    /**
     * @brief Aplica ruído AWGN diretamente no buffer recebido.
     * @param[out] buffer Ponteiro único para o buffer de dados que será modificado.
     * @param snr_db Relação sinal-ruído em dB.
     */
    static void transmit(std::unique_ptr<DataBuffer>& input, double snr_db) {
        static thread_local std::mt19937 __generator(std::random_device{}());

        size_t total_bits = input->get_total_bits();

        double snr_linear = std::pow(10, snr_db / 10.0);
        double sigma = std::sqrt(1.0 / snr_linear);

        std::normal_distribution<double> noise_dist(0.0, sigma);

        for(size_t i = 0; i < total_bits; ++i) {
            double signal = (input->get_bit(i) == 1) ? 1.0 : -1.0;
            double noisy_signal = signal + noise_dist(__generator);
            uint8_t received_bit = (noisy_signal > 0.0) ? 1 : 0;

            input->set_bit(i, received_bit);
        }
    }
};