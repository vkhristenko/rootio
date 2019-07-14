#include <iostream>
#include <vector>
#include <array>

#include "XXX/core/phys/Decls.hpp"

namespace XXX { namespace core { namespace phys {

SimpleFileHeader PhysReader::ReadFileHeader() {
    SimpleFileHeader fh;
    std::vector<uint8_t> buffer(100);

    // FIXME: not sure if this complication is necessary
    auto pos = source_->Pos();
    if (pos)
        source_->ReadAt(0, 100, (void*)buffer.data());
    else 
        source_->Read(100, (void*)(buffer.data()));

    // FIXME treat various statuses
    auto* ptr = buffer.data();
    if (std::strncmp((char const*)ptr, "root", 4)) {
        std::cout << "*** not a root file ***" << std::endl;
        assert(0);
    }
    ptr+=4;
    fh.DeserFrom(&ptr);

    return fh;
}

PhysReader::GenericRecord PhysReader::Read() {
    // first read the total bytes to read
    uint32_t totalBytes;
    std::array<uint8_t, 4> tmp;
    source_->Read(4, (void*)tmp.data());
    auto* tmpptr = tmp.data();
    totalBytes = get_u32(&tmpptr);

    // read the whole record
    std::vector<uint8_t> buffer(totalBytes);
    source_->Read(totalBytes-4, (void*)(buffer.data() + 4));
    buffer[0] = tmp[0];
    buffer[1] = tmp[1];
    buffer[2] = tmp[2];
    buffer[3] = tmp[3];

    // deserialize the physical key
    Key key; 
    auto *ptr = buffer.data();
    key.DeserFrom(&ptr);

    return {key, std::move(buffer)};
}

PhysReader::GenericRecord PhysReader::ReadN(uint64_t const nbytes) {
    std::vector<uint8_t> buffer(nbytes);
    source_->Read(nbytes, (void*)buffer.data());
    auto *ptr = buffer.data();

    Key key;
    key.DeserFrom(&ptr);
    return {key, std::move(buffer)};
}

PhysReader::GenericRecord PhysReader::ReadAt(uint64_t const pos) {
    // set the cursor to read from pos
    source_->SeekTo(pos);

    // reuse
    return std::move(this->Read());
}

PhysReader::GenericRecord PhysReader::ReadNAt(uint64_t const pos, uint64_t const nbytes) {
    // set the cursor to read from
    source_->SeekTo(pos);

    return std::move(this->ReadN(nbytes));
}

PhysReader::GenericRecord PhysReader::ReadByKey(Key const& key) {
    return std::move(this->ReadNAt(key.seek_key_, key.key_bytes_));
}

PhysReader::FreeSegmentsRecord PhysReader::ReadFreeSegmentsRecord(
        SimpleFileHeader const& fh) {
    // get the record itself
    auto record = this->ReadNAt(fh.seek_free_, fh.nbytes_free_);

    // interpret the blob
    auto *ptr = record.second.data() + record.first.key_bytes_;
    std::vector<FreeSegment> freeSegments(fh.nfree_);
    for (auto& freeseg : freeSegments)
        freeseg.DeserFrom(&ptr);

    return {record.first, std::move(freeSegments)};
}

}}}
