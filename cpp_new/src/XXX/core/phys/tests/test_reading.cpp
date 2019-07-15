#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

#include <iostream>
#include <vector>
#include <array>

#include "XXX/core/phys/Decls.hpp"
#include "extensions.hpp"

using namespace ::XXX::core::phys;
using namespace ::tests::extensions::phys;

void print_key(Key const& key) {
    printf("<<< key >>>\n");
    printf("<<< total_bytes = %u\n", key.total_bytes_);
    printf("<<< version = %u\n", key.version_);
    printf("<<< obj_bytes = %u\n", key.obj_bytes_);
    printf("<<< key_bytes = %u\n", key.key_bytes_);
    printf("<<< seek_key = %llu\n", key.seek_key_);
}

void print(Key const& key) {
    printf("Record At:%15llu Bytes:%10d\n", key.seek_key_, key.total_bytes_);
}

void print_empty_record(uint64_t location, uint64_t nbytes) {
    printf("Free Segment At:%15llu Bytes:%10llu\n", location, nbytes);
}

void print(FreeSegment const& seg) {
    printf("Free Segment At:%15llu Bytes:%10llu\n", seg.begin_,
        seg.end_ - seg.begin_ + 1u);
}

void print_file_header(SimpleFileHeader const& fh) {
    printf("<<< file header >>>\n");
    printf("<<< version = %u\n", fh.version_);
    printf("<<< begin = %u\n", fh.begin_);
    printf("<<< end = %llu\n", fh.end_);
    /*
    printf("<<< seek_free = %llu\n", fh.seek_free_);
    printf("<<< nbytes_free = %u\n", fh.nbytes_free_);
    printf("<<< nfree = %u\n", fh.nfree_);
    */
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "run with ./exec <path to file>" << std::endl;
        exit(0);
    }

    std::string infile{argv[1]};

    auto in = std::make_shared<InputFile>(infile);
    RecordReader reader{in};
    auto fhr = readFileHeader(in);
    auto const& fh = fhr.first;
    print_file_header(fh);
    //auto freeSegments = reader.ReadFreeSegmentsRecord(fh);

    /*
    for (auto const& segment : freeSegments.second)
        printf("begin = %llu end = %llu\n", segment.begin_, segment.end_);
    */

    int64_t location = fh.begin_;
    while (location < fh.end_) {
        /*
        auto itCheck = std::find_if(
            freeSegments.second.begin(), freeSegments.second.end(),
            [location](auto const& seg) { return seg.begin_ == location; });
        if (itCheck != freeSegments.second.end()) {
            print(*itCheck);
            location += (itCheck->end_ - itCheck->begin_ + 1);
            continue;
        }
        */
        
        // read the record and advance location
        auto record = reader.TryReadAt(location);
        if (record.first.total_bytes_ < 0) {
            print_empty_record(location, std::abs(record.first.total_bytes_));
            location += std::abs(record.first.total_bytes_);
            continue;
        }

        // normal non-empty record
        print(record.first);
        location+=record.first.total_bytes_;
    }

    return 0;
}
