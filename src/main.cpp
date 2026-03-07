#include "DataBuffer/DataBuffer.hpp"
#include "Hamming/Hamming.hpp"

int main(){

    DataBuffer db(4);
    db.set_bit(0, 0);
    db.set_bit(1, 0);
    db.set_bit(2, 1);
    db.set_bit(3, 0);
    Hamming h("src/Hamming/key.txt");

    auto out = h.encode(db);
    out->print_bit_range(0, 7);
    return 0;
}