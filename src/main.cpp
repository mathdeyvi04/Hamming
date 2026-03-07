#include "DataBuffer/DataBuffer.hpp"
#include "Hamming/Hamming.hpp"
#include "NoiseChannel/NoiseChannel.hpp"

int main(){

    DataBuffer db(4);
    db.set_bit(0, 0);
    db.set_bit(1, 0);
    db.set_bit(2, 1);
    db.set_bit(3, 0);
    std::cout << "Enviei: ";
              db.print_bit_range(0, 4);
    std::cout << std::endl;

    Hamming h("src/Hamming/key.txt");

    auto sended_bits = h.encode(db);
    std::cout << "Após codificar: ";
              sended_bits->print_bit_range(0, 7);
    std::cout << std::endl;

    NoiseChannel::transmit(sended_bits, 1);

    std::cout << "Após NoiseChannel: ";
              sended_bits->print_bit_range(0, 7);
    std::cout << std::endl;

    auto received_bits = h.decode(*sended_bits);
    std::cout << "Após a detecção e correção: ";
              received_bits->print_bit_range(0, 4);
    std::cout << std::endl;

    return 0;
}