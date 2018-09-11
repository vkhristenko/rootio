#include <iostream>

extern "C" {
#include "iolayer.h"
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

    close_from_read(&llio);
}
