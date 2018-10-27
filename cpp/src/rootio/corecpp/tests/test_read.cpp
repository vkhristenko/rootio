#include <iostream>

namespace rootio { namespace c {

extern "C" {
#include "rootio/core/localfs.h"
}

}}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "no input filename provided" << std::endl;
        return 0;
    }

    std::string filename {argv[1]};
    auto ctx = rootio::c::localfs_open_to_read(filename.c_str());
    rootio::c::localfs_close_from_read(&ctx);
}
