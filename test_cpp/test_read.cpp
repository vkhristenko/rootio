#include <iostream>

namespace root {

extern "C" {
#include "iolayer.h"
}

}

using namespace root;

int main(int argc, char **argv) 
{
    std::cout << "hello world\n";
    if (argc < 2) {
        std::cout << "no input filename provided" << std::endl;
        exit(1);
    }

    std::string filename {argv[1]};
    llio_t llio = root::open_to_read(filename.c_str());

    root::close_from_read(&llio);
}
