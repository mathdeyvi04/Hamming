#include "Manager/Manager.hpp"
#include "DataBuffer/DataBuffer.hpp"

// Para tratarmos as entradas
#include <string>
#include <fstream>
#include <cctype>

// Alterar nome da variável para buffer_size
int main(int argc, char* argv[]){

    // Para verificarmos se há um arquivo com o nome dado
    auto is_file = [](const std::string& p) { return std::ifstream(p).good(); };
    auto is_number = [](const std::string& s) -> bool {
        try {
            size_t pos;
            std::stod(s, &pos);
            return pos == s.length();
        }  catch (...) {
            return false;
        }
    };
    switch(argc){
        case 1: {
            std::cout << "No parameters provided, assuming SNR_db = 1000.\n\n";
        }
        case 2: {
            Manager::demo(
                (argc == 2) ? std::stod(argv[1]) : 1000
            );
            return 0;
        }

        case 3: {
            /*
            Haverá Mais de Uma Possibilidade de Entrada Aqui
            $0 ./main

            Caso 1:
                $1 SNR_db
                $2 Nome do arquivo de entrada ou Sequência de Bits

            Caso 2:
                $1 Nome de um arquivo para expelirmos os resultados
                $2 Nome do arquivo de entrada
            */

            /* Verificação do Primeiro Argumento */
            if(!is_number(argv[1])){
                // Caso 2

                std::ofstream file(argv[1]);
                if(!file.is_open()){ std::cout << "Error in the exit file" << std::endl; return -1;}
                DataBuffer db(argv[2]);
                Manager::extract_ber(file, db);
                file.close();
                return 0;
            }

            // Caso 1

            // Entraremos com SNR_db e ou sequência de bits ou nome do arquivo a ser testado
            if(is_file(argv[2])){
                // Caso seja um arquivo válido
                DataBuffer db(argv[2]);
                Manager::demo(
                   std::stod(argv[1]),
                   &db
                );
                return 0;
            }

            // Caso não seja um arquivo válido, deve ser uma sequência de bits
            std::string bit_sequence = argv[2];
            size_t i = 0;
            size_t total_bits = bit_sequence.size();
            DataBuffer db(total_bits);
            while(i < total_bits){
                char bit_to_be_inserted = bit_sequence[i];

                if(
                    !std::isdigit(bit_to_be_inserted) || (bit_to_be_inserted != '0' && bit_to_be_inserted != '1')
                ){
                    std::cout << "Error in the bit sequence." << std::endl;
                    return -1;
                }
                db.set_bit(i, static_cast<uint8_t>(bit_to_be_inserted - 48));
                i++;
            }

            Manager::demo(
                std::stod(argv[1]),
                &db
            );
            return 0;
        }

    }

    return 0;
}