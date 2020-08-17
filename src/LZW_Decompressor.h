#include <map>
#include <string>
#include <memory>
#include "File_Loader.h"

class LZW_Decompressor
{

    public:
        LZW_Decompressor();

        ~LZW_Decompressor() { delete input_buffer; }

        char* get_input_buffer() { return input_buffer; }

        size_t get_buffer_size() { return BUFFER_SIZE; }

        size_t get_max_dictionary_size() { return Max_DICTIONARY_SIZE; }

        std::string get_output() { return output; }

        std::map<int, std::string> get_dictionary() { return dictionary; }

        void decompress( size_t input_size );

        void add_dictionary_entry( std::string sequence );

        void initialise_dictionary();

        void reinitialise();

        void write_output( std::string file_name );

        bool decode_file( std::string file_path );

    protected:

    private:
        std::map<int, std::string> dictionary;
        std::string output, prev_sequence;
        size_t Max_DICTIONARY_SIZE = 4096; // Maximum of 2^12 codes 
        size_t BUFFER_SIZE = 900; // Buffer has to be multiple of 3
        char* input_buffer = new char[BUFFER_SIZE];
        std::unique_ptr<File_Loader> file_loader;
};