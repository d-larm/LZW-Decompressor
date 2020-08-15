#include <fstream>
#include <string>
#include <memory>
#include <iostream>

class File_Loader 
{
    public:
        File_Loader() { file = std::make_unique<std::ifstream>( std::ifstream() ); }

        void load_file( std::string file_path );

        void close_file() { file->close(); }

        bool is_file_open() { return file->is_open(); }

        void write_to_file( std::string file_path, std::string contents );
        
        std::streamsize read_file( char* buffer, size_t buffer_size  );

    protected:

    private:
        std::unique_ptr<std::ifstream> file;
};