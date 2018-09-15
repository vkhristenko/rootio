#include <iostream>

namespace root {

extern "C" {
#include "iolayer.h"
}

}

using namespace root;

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
    llio_t llio = root::open_to_read(filename.c_str());
    root::print_file_header(&llio.header);
    root::close_from_read(&llio);
}
