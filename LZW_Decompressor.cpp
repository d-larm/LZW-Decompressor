#include <string>
#include "LZW_Decompressor.h"


LZW_Decompressor::LZW_Decompressor() {
    // Initialise dictionary entries
    initialise_dictionary();
    file_loader =  std::make_unique<File_Loader>( File_Loader() );
}

void LZW_Decompressor::add_dictionary_entry( std::string sequence ) {
    if( dictionary.size() >= Max_DICTIONARY_SIZE ) {
        initialise_dictionary();
    }
    dictionary.insert( std::pair<int, std::string>( dictionary.size(), sequence ) );
}

void LZW_Decompressor::initialise_dictionary() {
    dictionary.clear();
    for( int i=0; i<256; i++ ) {
        dictionary.insert( std::pair<int,std::string>( i, std::string( 1, (uint8_t) i ) ) );
    }
}

void LZW_Decompressor::reinitialise() {
    initialise_dictionary();
    output = "";
    prev_sequence = "";
    input_buffer[0] = 0;
}

void LZW_Decompressor::decompress( size_t input_size ) {

    std::string current_code, next_code, current_sequence, next_sequence, new_entry, decoded_input;

    for( size_t i=0; i<input_size; i+=3 ) {
        // Split input into 24 bit sections ( pairs of codes ) equivalent to 3 characters
        uint32_t code_pair;

        // If we get to the last code in the input buffer and odd number of codes
        if( input_size < BUFFER_SIZE && i > input_size - 3 ) {
            
            if( i == input_size - 2 ){ 
                // 16 bits left in the input buffer ( two chars )
                code_pair = (uint32_t)( ( (uint8_t)( input_buffer[i] ) << 8 )
                | ( (uint8_t) input_buffer[ i+1 ] ) );
            }else{ 
                // 8 bits left in the input buffer ( single char )
                 code_pair = (uint32_t)( (uint8_t)( input_buffer[ i ] ) );
            }

            /* 
                16-bit section represented in 32 bit unsigned integer

                00000000 00000000 [0000(0110 01111100)]
                        ^                   ^               
                    Padding                Code
            */
            try {
                current_sequence = dictionary.at( code_pair );
            }catch( std::out_of_range err ) {
                current_sequence = prev_sequence + prev_sequence[0];
            }
             

            new_entry = prev_sequence + current_sequence;
            // cout << "New entry into dictionary: " << new_entry << " at " << dictionary.size() << endl;

            add_dictionary_entry( new_entry );
            decoded_input += current_sequence;
            break;

        }
        
        code_pair = ( uint32_t )( ( (uint8_t)( input_buffer[i] ) << 16 )
        | ( (uint8_t)( input_buffer[ i+1 ] ) << 8  )
        | ( (uint8_t)( input_buffer[ i+2 ] ) ) );

        /* 
            24-bit sections represented in 32 bit unsigned integer

            00000000 [01001101 0001][1100 00110100]
                ^           ^               ^
                Padding   Curent Code     Next Code
        */

        // Get first code in code pair by keeping 12 MSBs after padding
        uint32_t current_code = ( code_pair >> 12 );

        // Get second code in code pair by keeping 12 LSBs
        uint32_t next_code = ( code_pair & 0b111111111111 );
      
        try {
            // Decode the 12 bit codes using the dictionary
            current_sequence = dictionary.at( current_code );
        }catch( std::out_of_range e ) {
            // If the current code is unrecognised use first letter from previous code appended to the previous sequence 
            current_sequence = prev_sequence + prev_sequence[0];
        }
       

        if( prev_sequence.length() > 0 ) {
            // Add dictionary entry for the sequence in the previous code pair concatenated with the first sequence in the current pair
            new_entry = prev_sequence + current_sequence[0];
            add_dictionary_entry( new_entry );
            // Add the last sequence from the previous code pair to the output
            decoded_input += prev_sequence;
        }

        try {
            // Get second sequence from pair using the dictionary
            next_sequence = dictionary.at( next_code );
        }catch(std::out_of_range e){
            // The next code is not in the dictionary, use the first sequence from code pair concatenated with it's first letter
            next_sequence = current_sequence + current_sequence[0];
        }

        // Add first sequence in code pair and first letter from the second sequence in code pair to dictionary
        new_entry = current_sequence + next_sequence[0];
        add_dictionary_entry( new_entry );

        // Set the second sequence as the previous sequence, move on to the next code pair
        prev_sequence = next_sequence;        
        
        // Add the first decoded sequence to the output
        decoded_input += current_sequence;
    }
    // Add the output from the input buffer to the final output
    output += decoded_input;
}

void LZW_Decompressor::write_output( std::string file_name ) {
    file_loader->write_to_file( file_name, output );
}

bool LZW_Decompressor::decode_file( std::string file_path ) {
    reinitialise();

    file_loader->load_file( file_path );
    if ( file_loader->is_file_open() ) {
        while( file_loader ) {
            // Read section of file into the decompressor's input buffer and get number of read characters
            std::streamsize read_count = file_loader->read_file( input_buffer, BUFFER_SIZE );
            
            if( read_count <= 0 ){
                // End read if there is no more data to read
                break;
            }
            decompress( read_count );
        }
        file_loader->close_file();
        return true;
        
    }
    return false;
}