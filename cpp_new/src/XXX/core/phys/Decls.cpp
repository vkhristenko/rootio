#include <iostream>
#include <vector>
#include <array>

#include "XXX/core/phys/Decls.hpp"
#include "XXX/core/common/aux.hpp"

namespace XXX { namespace core { namespace phys {

RecordReader::GenericRecord RecordReader::TryRead() {
    // first read the total bytes to read
    int32_t totalBytes;
    std::array<uint8_t, 4> tmp;
    source_->Read(4, (void*)tmp.data());
    auto const* tmpptr = tmp.data();
    totalBytes = get_i32(&tmpptr);

    // empty record if 
    if (totalBytes < 0) {
        Key key;
        key.total_bytes_ = totalBytes;
        return {key, {}};
    }

    // read the whole record
    std::vector<uint8_t> buffer(totalBytes);
    source_->Read(totalBytes-4, (void*)(buffer.data() + 4));
    buffer[0] = tmp[0];
    buffer[1] = tmp[1];
    buffer[2] = tmp[2];
    buffer[3] = tmp[3];

    // deserialize the physical key
    Key key; 
    auto const*ptr = buffer.data();
    key.DeserFrom(&ptr);

    return {key, std::move(buffer)};
}

RecordReader::GenericRecord RecordReader::ReadN(int64_t const nbytes) {
    std::vector<uint8_t> buffer(nbytes);
    source_->Read(nbytes, (void*)buffer.data());
    auto const *ptr = buffer.data();

    Key key;
    key.DeserFrom(&ptr);
    return {key, std::move(buffer)};
}

RecordReader::GenericRecord RecordReader::TryReadAt(int64_t const pos) {
    // set the cursor to read from pos
    source_->SeekTo(pos);

    // reuse
    return std::move(this->TryRead());
}

RecordReader::GenericRecord RecordReader::ReadNAt(int64_t const pos, int64_t const nbytes) {
    // set the cursor to read from
    source_->SeekTo(pos);

    return std::move(this->ReadN(nbytes));
}

RecordReader::GenericRecord RecordReader::ReadByKey(Key const& key) {
    return std::move(this->ReadNAt(key.seek_key_, key.key_bytes_));
}

void RecordWriter::Write(GenericRecord const& record) {
    assert(record.first.seek_key_ == sink_->Pos());
    assert(record.first.total_bytes_ == record.second.size());

    sink_->Write(record.first.total_bytes_, (void*)record.second.data());
}

void RecordWriter::TryWrite(GenericRecord const& record) {
    assert(std::abs(record.first.total_bytes_) != 0);

    
    if (record.first.total_bytes_ < 0) {
        // empty record
        auto total_bytes = std::abs(record.first.total_bytes_);
        std::vector<uint8_t> buffer(total_bytes);
        auto* ptr = buffer.data();
        ::XXX::core::common::put_i32(&ptr, record.first.total_bytes_);

        sink_->Write(total_bytes, (void*)buffer.data());
    } else {
        assert(record.second.size() == record.first.total_bytes_);
        sink_->Write(record.first.total_bytes_, (void*)record.second.data());
    }
}

void RecordWriter::WriteAt(GenericRecord const& record) {
    assert(record.first.total_bytes_ == record.second.size());

    auto pos = record.first.seek_key_;
    sink_->WriteAt(pos, record.first.total_bytes_, (void*)record.second.data());
}

void RecordWriter::TryWriteAt(int64_t const pos, GenericRecord const& record) {
    sink_->SeekTo(pos);

    this->TryWrite(record);
}

}}}
