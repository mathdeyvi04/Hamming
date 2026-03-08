#include "Manager/Manager.hpp"
#include "DataBuffer/DataBuffer.hpp"

// Para tratarmos as entradas
#include <string>
#include <fstream>
#include <cctype>

int main(int argc, char* argv[]){
    /**
     * @details
     * - Caso 1: Para Produção
     * $0 -> ./main
     * $1 -> SNR_DB
     * $2 -> Nome do Arquivo ou Sequência de Bits de Entrada
     */

    // Para verificarmos se há um arquivo com o nome dado
    auto is_file   = [](const std::string& p) { return std::ifstream(p).good(); };
    auto is_number = [](const std::string& s) -> bool {
        try {
            size_t pos;
            std::stod(s, &pos);
            return pos == s.length();
        }  catch (...) {
            return false;
        }
    };

    if(argc < 3){
        std::cout << "Not enough parameters." << std::endl;
        return  -1;
    }

    if(!is_number(argv[1])){
        std::cout << "SNR_DB not valid." << std::endl;
        return -1;
    }
    double SNR_DB = std::stod(argv[1]);

    if(!is_file(argv[2])){
        // Então é sequência de bits
        std::string bit_sequence = argv[2];
        size_t i = 0;
        size_t total_bits = bit_sequence.size();
        DataBuffer input_db(total_bits);
        while(i < total_bits){
            char bit_to_be_inserted = bit_sequence[i];

            if(
                !std::isdigit(bit_to_be_inserted) || (bit_to_be_inserted != '0' && bit_to_be_inserted != '1')
            ){
                std::cout << "Error in the bit sequence." << std::endl;
                return -1;
            }

            input_db.set_bit(i, static_cast<uint8_t>(bit_to_be_inserted - 48));
            i++;
        }

        Manager::execute(
            SNR_DB,
            input_db
        );
    }
    else{
        // Então é um arquivo válido
        DataBuffer input_db(argv[2]);
        Manager::execute(
            SNR_DB,
            input_db
        );
    }


    return 0;
}