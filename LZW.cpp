#include <iostream>
#include "LZW_Decompressor.h"

int main(int argc, char** argv) {
    LZW_Decompressor* decompressor = new LZW_Decompressor();
    for( int i=1; i < argc; i++ ) {
        std::cout << "=============================================================\nBegin decompression of file at " << argv[i] << std::endl;
        if(  decompressor->decode_file( argv[i] ) ){
            decompressor->write_output( (std::string) argv[i] + ".txt" );
            std::cout << "===========================Result============================\n\n" << decompressor->get_output() << "\n" << std::endl;
        }else {
            std::cout << "Could not decompress the file located at " << argv[i] << std::endl; 
        }
    }
}