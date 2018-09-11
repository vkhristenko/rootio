#include <iostream>

#include "iolayer.h"

void recurse(llio_t const& llio, directory_t const& dir) 
{

}

int main(int argc, char **argv) 
{
    std::cout << "hello world\n";
    if (argc < 2) {
        std::cout << "no input filename provided" << std::endl;
        exit(1);
    }

    std::string filename {argv[1]};
    llio_t llio = open_to_read(filename.c_str());
    print_file_header(&llio);
    close_from_read(&llio);
}
