#include "DataBuffer/DataBuffer.hpp"
#include "Hamming/Hamming.hpp"
#include "NoiseChannel/NoiseChannel.hpp"

/**
 * @brief Compara dois buffers bit a bit e conta o total de divergências.
 * @details
 * Útil para calcular a taxa de erro de bit (BER) após a transmissão ou decodificação.
 * @param original Buffer com os dados de referência (originais).
 * @param processed Buffer com os dados que passaram pelo canal e pelo decoder.
 * @return size_t Quantidade total de bits invertidos (erros).
 * @throws std::invalid_argument Se os buffers possuírem tamanhos diferentes.
 */
size_t count_bits_errors(const DataBuffer& original, const DataBuffer& processed) {
    if (original.get_total_bits() != processed.get_total_bits()) {
        throw std::invalid_argument("Erro: Buffers de tamanhos diferentes não podem ser comparados.");
    }

    size_t error_count = 0;
    size_t total_bits = original.get_total_bits();

    for (size_t i = 0; i < total_bits; ++i) {
        if (original.get_bit(i) != processed.get_bit(i)) {
            error_count++;
        }
    }

    return error_count;
}

// Alterar nome da variável para buffer_size
int main(){

    DataBuffer db("src/DataBuffer/exemplo_1.txt");
//    db.set_bit(0, 0);
//    db.set_bit(1, 0);
//    db.set_bit(2, 1);
//    db.set_bit(3, 0);

    std::cout << "Enviei: ";
              db.print_bit_range(0, 1000); // Apenas para garantirmos que todos os bits serão exibidos
    std::cout << std::endl;

    Hamming h("src/Hamming/key.txt");

    auto sended_bits = h.encode(db);
    std::cout << "Codifiquei: ";
              sended_bits->print_bit_range(0, 1000); // Apenas para garantirmos que todos os bits serão exibidos
    std::cout << std::endl;

    auto received_bits = h.decode(*sended_bits);
    std::cout << "Codifiquei: ";
              received_bits->print_bit_range(0, 1000); // Apenas para garantirmos que todos os bits serão exibidos
    std::cout << std::endl;






    return 0;
}