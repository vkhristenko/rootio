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

using namespace ::XXX::core::phys;

SimpleFileHeader read_file_header(int fd) {
    std::vector<uint8_t> buffer(100); // buffer of 100 bytes
    auto nbytes = read(fd, (void*)buffer.data(), 100);
    auto* ptr = buffer.data();

    if (strncmp((char const*)ptr, "root", 4)) {
        printf("*** Not a ROOT file ***\n");
        assert(0);
    }
    SimpleFileHeader fh;
    ptr+=4;
    fh.DeserFrom(&ptr);
    return fh;
}

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

std::pair<Key, std::vector<FreeSegment>> read_free_segments(
        int fd, SimpleFileHeader const& fh) {
    std::vector<uint8_t> buffer(fh.nbytes_free_);
    lseek(fd, fh.seek_free_, SEEK_SET);
    read(fd, (void*)buffer.data(), fh.nbytes_free_);

    auto* ptr = buffer.data();

    // key
    Key key; key.DeserFrom(&ptr);
    print(key);

    // leftover from the key... 
    if (key.version_ > 1000u)
        get_u64(&ptr);
    else
        get_u32(&ptr);
    auto s1 = get_string(&ptr);
    auto s2 = get_string(&ptr);
    auto s3 = get_string(&ptr);
    printf("s1 = %s s2 = %s s3 = %s\n",
        s1.c_str(), s2.c_str(), s3.c_str());

    std::vector<FreeSegment> freeSegments(fh.nfree_);
    for (auto& freeseg : freeSegments) {
        freeseg.DeserFrom(&ptr);
        printf("begin = %llu end = %llu\n",
            freeseg.begin_, freeseg.end_);
    }

    return {key, std::move(freeSegments)};
}

Key read_record(int fd) {
    uint32_t total_bytes;
    std::array<uint8_t, 4> tmp;
    read(fd, (void*)tmp.data(), 4);
    auto* ptrtmp = tmp.data();
    total_bytes = get_u32(&ptrtmp);

    std::vector<uint8_t> buffer(total_bytes);
    read(fd, (void*)(buffer.data() + 4), total_bytes-4);
    buffer[0] = tmp[0];
    buffer[1] = tmp[1];
    buffer[2] = tmp[2];
    buffer[3] = tmp[3];
    Key rk; 
    auto* ptr = buffer.data();
    rk.DeserFrom(&ptr);
    return rk;
}

void print_file_header(SimpleFileHeader const& fh) {
    printf("<<< file header >>>\n");
    printf("<<< version = %u\n", fh.version_);
    printf("<<< begin = %u\n", fh.begin_);
    printf("<<< end = %llu\n", fh.end_);
    printf("<<< seek_free = %llu\n", fh.seek_free_);
    printf("<<< nbytes_free = %u\n", fh.nbytes_free_);
    printf("<<< nfree = %u\n", fh.nfree_);
}

void dump_records(
        int fd, SimpleFileHeader const& fh,
        std::pair<Key, std::vector<FreeSegment>> const& fsr) {
    uint64_t location = fh.begin_;
    while (location < fh.end_) {
        auto itCheck = std::find_if(fsr.second.begin(), fsr.second.end(),
            [location](auto const& fsegment) { return fsegment.begin_ == location; });

        // skip if this record is empty
        if (itCheck != fsr.second.end()) {
            location += (itCheck->end_ - itCheck->begin_ + 1);
            continue;
        }

        lseek(fd, location, SEEK_SET);
        auto key = read_record(fd);
        print(key);
//        print_key(key);
        location += key.total_bytes_;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "run with ./exec <path to file>" << std::endl;
        exit(0);
    }

    std::string infile{argv[1]};

    auto fd = open(infile.c_str(), O_RDONLY, 0);
    auto fh = read_file_header(fd);
    print_file_header(fh);
    printf("\n---\n");
    auto freeSegmentsRecord = read_free_segments(fd, fh);
    dump_records(fd, fh, freeSegmentsRecord);

    /*
    // read header
    auto fh = read_file_header(fd);
    print_file_header(fh);

    printf("\n-----\n\n");

    uint64_t location = fh.begin_;
    lseek(fd, location, SEEK_SET);
    {
        auto key = read_record(fd);
        print_key(key);
        location += key.total_bytes_;
    }
    printf("\n-----\n\n");
    lseek(fd, location, SEEK_SET);
    {
        auto key = read_record(fd);
        print_key(key);
        location += key.total_bytes_;
    }*/

    // read
    /*
    uint64_t location = XXX;
    uint32_t irecord = 0;
    while (location < theEND) {
        // read in a record
        read_generic_record();

        // print info about the record

        location += size of the record
        irecord++;
    }*/

    close(fd);
    return 0;
}
