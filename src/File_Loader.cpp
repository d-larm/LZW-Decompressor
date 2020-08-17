#include <string>
#include "File_Loader.h"


void File_Loader::load_file( std::string file_path ) {
    if( file->is_open() ) {
        file->close(); 
    }
    file->open( file_path, std::ifstream::binary );
}

std::streamsize File_Loader::read_file( char* buffer, size_t buffer_size  ) {
    file->read( buffer, buffer_size);
    return file->gcount();
}

void File_Loader::write_to_file( std::string file_path, std::string contents ) {
    std::ofstream output_file( file_path );
    output_file << contents;
    output_file.close();
}
