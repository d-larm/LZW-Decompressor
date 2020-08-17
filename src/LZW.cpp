#include <iostream>
#include "LZW_Decompressor.h"

int main(int argc, char** argv) {
    LZW_Decompressor* decompressor = new LZW_Decompressor();
    for( int i=1; i < argc; i++ ) {
        std::cout << "=============================================================\n\nBegin decompression of file at " << argv[i] << std::endl;
        if(  decompressor->decode_file( argv[i] ) ){
            std::string file_path =  (std::string) argv[i] + ".txt";
            std::cout << "\n-------------------------------------------------------------\n\n" << "Writing output to "<< file_path << std::endl;
            decompressor->write_output( file_path );
            std::cout << "\n---------------------------Result----------------------------\n\n" << decompressor->get_output() << "\n" << std::endl;
            std::cout << "=============================================================" << std::endl;
            
        }else {
            
            std::cout << "Could not decompress the file located at " << argv[i] << std::endl; 
        }
    }
}