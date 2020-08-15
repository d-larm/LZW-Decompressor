#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

class LZW_Decompressor
{
    

    public:
        LZW_Decompressor() {
            // Initialise 256 dictionary entries
            for( int i=0; i<256; i++ ) {
                dictionary.insert( pair<int,string>( i, string( 1, (uint8_t) i ) ) );
            }
        }
        string get_output() { return output; }
        map<int, string> get_dictionary() { return dictionary; }
        string decompress( string input );

    protected:

    private:
        map<int, string> dictionary;
        string output;



};

string LZW_Decompressor::decompress( string input ) {
    
    // Excludes the null character, include with +1 
    size_t size = input.length() + 1;

    string prev_decode;
    string code_1, code_2, sequence_1, sequence_2, new_entry;
    cout << "size: " << size << endl;
    for( size_t i=0; i<size; i+=3 ) {
        // Split input into 24 bit sections
        uint32_t code_section;
        if( size % 3 != 0 && i > size - 3 ) {
            cout << "Final padded segment, size:" << size  << " index:" << i << endl;
            if( i == size - 2 ){
                code_section = (uint32_t)( ( (uint8_t)( input[i] ) << 8 )
                | ( input[ i+1 ] ) );
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
            string decode = dictionary.at( code_section );

            new_entry = prev_decode + decode;
            cout << "New entry into dictionary: " << new_entry << " at " << dictionary.size() << endl;

            dictionary.insert( pair<int,string>( dictionary.size(), new_entry ) );
            output += decode;
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

        if( prev_decode.length() > 0 ) {
            // Add dictionary entry for the last code with the new code
            new_entry = prev_decode + sequence_1[0];
            cout << "Current: " << prev_decode << ", Next: " << sequence_1 << ":" << code_1 << endl;
            cout << "New entry into dictionary: " << new_entry << " at " << dictionary.size() << endl;

            dictionary.insert( pair<int,string>( dictionary.size(), new_entry ) );
        }

        try {
            sequence_2 = dictionary.at( code_2 );
        }catch(out_of_range e){
            sequence_2 = sequence_1 + sequence_1[0];
        }

        cout << "Current: " << sequence_1 << ":" << code_1 << ", Next: " << sequence_2 << ":" << code_2 << endl;
        
        new_entry = sequence_1 + sequence_2[0];
        cout << "New entry into dictionary: " << new_entry << " at " << dictionary.size() << endl;

        dictionary.insert( pair<int,string>( dictionary.size(), new_entry ) );
        prev_decode = sequence_2;
       
        output += sequence_1;
        output += sequence_2;

        cout << output << endl;
    }
    return output;
}


int main(int argc, char** argv) {
    LZW_Decompressor decompressor;
    string encoded_message = "";
    for( int i=1; i < argc; i++ ) {
        string line;
        ifstream compressed_file ( argv[i] );
        if ( compressed_file.is_open() ) {
            while( getline( compressed_file, line ) ) {
                // const char* input = line.c_str();
                encoded_message += line;
            }
            decompressor.decompress( encoded_message );
            cout << decompressor.get_output() << endl;
            compressed_file.close();
            
        } else {
            cout << "Unable to open file " << argv[i] << endl;
        }
    }
}