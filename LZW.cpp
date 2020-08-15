#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm> 
using namespace std;

class LZW_Decompressor
{
    

    public:
        LZW_Decompressor() {
            // Initialise 256 dictionary entries
            initialise_dictionary();
            
        }
        string get_output() { return output; }
        map<int, string> get_dictionary() { return dictionary; }
        string decompress( size_t input_size );
        char* get_input_buffer() { return input_buffer; }
        int get_buffer_size() { return BUFFER_SIZE; }
        void initialise_dictionary() {
            dictionary.clear();
            for( int i=0; i<256; i++ ) {
                dictionary.insert( pair<int,string>( i, string( 1, (uint8_t) i ) ) );
            }
        }


    protected:

    private:
        map<int, string> dictionary;
        string output;
        int BUFFER_SIZE = 900; // Buffer has to be multiple of 3
        char* input_buffer = new char[BUFFER_SIZE];
        string prev_sequence;
};

string LZW_Decompressor::decompress( size_t input_size ) {
    
    // Excludes the null character, include with +1 
    // size_t size = input.length();
    char* input = input_buffer;
    string code_1, code_2, sequence_1, sequence_2, new_entry;
    int max_code = 0;
    cout << "size: " << input_size << endl;
    for( size_t i=0; i<input_size; i+=3 ) {
        if( dictionary.size() >= 4096 ) {
            initialise_dictionary();
        }
        // Split input into 24 bit sections
        uint32_t code_section;
        if( input_size < BUFFER_SIZE && i > input_size - 3 ) {
            cout << "Final padded segment, input_size:" << input_size  << " index:" << i << endl;
            if( i == input_size - 2 ){
                code_section = (uint32_t)( ( (uint8_t)( input[i] ) << 8 )
                | ( (uint8_t) input[ i+1 ] ) );
            }else{
                 code_section = (uint32_t)( (uint8_t)( input[ i ] ) );
            }
            
            /* 
                    16-bit section represented in 32 bit unsigned integer

                    00000000 00000000 [0000(0110 01111100)]
                            ^                   ^               
                        Padding                Code
            */

           // No bit shifting required
            cout << "Final code: " << code_section << endl;
            string sequence = dictionary.at( code_section );

            new_entry = prev_sequence + sequence;
            cout << "New entry into dictionary: " << new_entry << " at " << dictionary.size() << endl;

            // dictionary.insert( pair<int,string>( dictionary.size(), new_entry ) );
            output += sequence;
            break;

        }
        
        code_section = ( uint32_t )( ( (uint8_t)( input[i] ) << 16 )
        | ( (uint8_t)( input[ i+1 ] ) << 8  )
        | ( (uint8_t)( input[ i+2 ] ) ) );

        /* 
                24-bit sections represented in 32 bit unsigned integer

                00000000 [01001101 0001][1100 00110100]
                    ^          ^               ^
                  Padding    Code 1          Code 2
        */

        // Get first code in section by keeping 12 MSBs before padding
        uint32_t code_1 = ( code_section >> 12 );

        // Get second code in section by keeping 12 LSBs
        uint32_t code_2 = ( code_section & 0b111111111111 );
      

        // Decode the 12 bit codes using the dictionary
        sequence_1 = dictionary.at( code_1 );
       

        if( prev_sequence.length() > 0 ) {
            // Add dictionary entry for the last code with the new code
            new_entry = prev_sequence + sequence_1[0];
            // cout << "Current: " << prev_sequence << ", Next: " << sequence_1 << ":" << code_1 << endl;
            // cout << "New entry into dictionary: " << new_entry << " at " << dictionary.size() << endl;

            dictionary.insert( pair<int,string>( dictionary.size(), new_entry ) );
        }

        try {
            sequence_2 = dictionary.at( code_2 );
        }catch(out_of_range e){
            // The next code is not in the dictionary
            sequence_2 = sequence_1 + sequence_1[0];
        }

        // cout << "Current: " << sequence_1 << ":" << code_1 << ", Next: " << sequence_2 << ":" << code_2 << endl;
        
        new_entry = sequence_1 + sequence_2[0];
        // cout << "New entry into dictionary: " << new_entry << " at " << dictionary.size() << endl;

        dictionary.insert( pair<int,string>( dictionary.size(), new_entry ) );
        prev_sequence = sequence_2;
        int maxx = std::max( code_1, code_2 );
        max_code = std::max( maxx, max_code );
        
        
        output += sequence_1;
        output += sequence_2;
        // cout << output << endl;
    }
    cout << "Max Code" << max_code << endl;
    return output;
}


int main(int argc, char** argv) {
    LZW_Decompressor decompressor;
    string encoded_message = "";
    for( int i=1; i < argc; i++ ) {
        string line;
        ifstream compressed_file ( argv[i], ifstream::binary );
        stringstream buff;
        if ( compressed_file.is_open() ) {
            while( compressed_file ) {

                // compressed_file.seekg (0, compressed_file.end);
                // cout << compressed_file.tellg() << endl;
                // compressed_file.seekg (0, compressed_file.beg);

                compressed_file.read( decompressor.get_input_buffer(), decompressor.get_buffer_size() );
                streamsize read_count = compressed_file.gcount();
                
                if( read_count <= 0 ){
                    break;
                }
                decompressor.decompress( read_count );
            }
            cout << decompressor.get_output() << endl;

            // buff << compressed_file.rdbuf();
            // decompressor.decompress( buff.str() );
            // cout << decompressor.get_output() << endl;
            compressed_file.close();
            
        } else {
            cout << "Unable to open file " << argv[i] << endl;
        }
    }
}