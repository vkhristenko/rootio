#ifndef XXX_CORE_PHYS_DECLS_H
#define XXX_CORE_PHYS_DECLS_H

#include <inttypes.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

#include "XXX/core/common/aux.hpp"

namespace XXX { namespace core { namespace phys {

using namespace ::XXX::core::common;

struct Seekable {
    virtual void SeekTo(uint64_t const pos) = 0;
    virtual uint64_t Pos() const = 0;
};

struct Closeable {
    virtual void Close() = 0;
};

class SourceInterface : public Seekable, public Closeable {
public:
    explicit SourceInterface(std::string const& path) : path_{path} {}

    virtual ~SourceInterface() {}

    virtual void Read(uint64_t const nbytes, void *data) = 0;

    virtual void ReadAt(uint64_t const pos, uint64_t const nbytes, void *data) = 0;

protected:
    std::string path_;
};

class SinkInterface : public Seekable, public Closeable {
public:
    explicit SinkInterface(std::string const& path) : path_{path} {}

    virtual ~SinkInterface() {}

    virtual void Write(uint64_t const nbytes, void *data) = 0;

    virtual void WriteAt(uint64_t const pos, uint64_t const nbytes, void *data) = 0;

protected:
    std::string path_;
};

class OutputFile : public SinkInterface {
public:
    explicit OutputFile(std::string const& path)
        : SinkInterface{path}
        , fd_{open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 
            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)}
    {}

    ~OutputFile() override {}

    // FIXME should be the same for both interfaces
    void SeekTo(uint64_t const pos) override { lseek(fd_, pos, SEEK_SET); }
    uint64_t Pos() const override { return lseek(fd_, 0, SEEK_CUR); }
    
    void Write(uint64_t nbytes, void *data) override { write(fd_, (void*)data, nbytes); }

    void WriteAt(uint64_t const pos, uint64_t const nbytes, void *data) override {
        this->SeekTo(pos);
        this->Write(nbytes, data);
    }
   
    void Close() override { close(fd_); }

protected:
    int fd_;
};

class InputFile : public SourceInterface {
public:
    explicit InputFile(std::string const& path) 
        : SourceInterface{path}
        , fd_{open(path_.c_str(), O_RDONLY, 0)}
    {}

    ~InputFile() override {}

    void SeekTo(uint64_t const pos) override { lseek(fd_, pos, SEEK_SET); }
    uint64_t Pos() const override { return lseek(fd_, 0, SEEK_CUR); }

    void Read(uint64_t const nbytes, void *data) override { read(fd_, data, nbytes); }

    void ReadAt(uint64_t const pos, uint64_t const nbytes, void *data) override {
        this->SeekTo(pos);
        this->Read(nbytes, data);
    }

    void Close() override { close(fd_); }

protected:
    int fd_;
};

struct Object {
    void SerTo(uint8_t ** buffer) const {
        put_version(buffer, version_);
        put_u32(buffer, id_);
        put_u32(buffer, bits_);
    }

    void DeserFrom(uint8_t** buffer) {
        version_ = get_version(buffer);
        id_ = get_u32(buffer);
        bits_ = get_u32(buffer);
    }

    static uint32_t StaticSize() {
        return 10u;
    }

    uint16_t version_;
    uint32_t id_;
    uint32_t bits_;
};

struct FreeSegment {
    void SerTo(uint8_t** buffer) const {
        put_version(buffer, version_);
        if (version_ > 1000u) {
            put_u64(buffer, begin_);
            put_u32(buffer, end_);
        } else {
            put_u32(buffer, static_cast<uint32_t>(begin_));
            put_u32(buffer, static_cast<uint32_t>(end_));
        }
    }

    void DeserFrom(uint8_t** buffer) {
        version_ = get_version(buffer);
        begin_ = version_ > 1000u ? get_u64(buffer) : get_u32(buffer);
        end_ = version_ > 1000u ? get_u64(buffer) : get_u32(buffer);
    }

    static uint32_t StaticSize() { return 10; }
    uint32_t Size() const { return version_ > 1000u ? StaticSize() + 8 : StaticSize();  }

    uint16_t version_;
    uint64_t begin_, end_;
};

struct Datime {
    void SerTo(uint8_t **buffer) const {
        put_u32(buffer, raw_);
    }

    void DeserFrom(uint8_t **buffer) {
        raw_ = get_u32(buffer);
    }

    static uint32_t StaticSize() { return 4; }

    uint32_t raw_;
};

struct Key {
    void SerTo(uint8_t **buffer) const {
        put_u32(buffer, total_bytes_);
        put_version(buffer, version_);
        put_u32(buffer, obj_bytes_);
        date_time_.SerTo(buffer);
        put_u16(buffer, key_bytes_);
        put_u16(buffer, cycle_);
        if (version_ > 1000u)
            put_u64(buffer, seek_key_);
        else 
            put_u32(buffer, static_cast<uint32_t>(seek_key_));
    }

    void DeserFrom(uint8_t **buffer) {
        total_bytes_ = get_u32(buffer);
        version_ = get_version(buffer);
        obj_bytes_ = get_u32(buffer);
        date_time_.DeserFrom(buffer);
        key_bytes_ = get_u16(buffer);
        cycle_ = get_u16(buffer);
        if (version_ > 1000u)
            seek_key_ = get_u64(buffer);
        else 
            seek_key_ = get_u32(buffer);
    }

    static uint32_t StaticSize() { return Datime::StaticSize() + 18; }
    uint32_t Size() const { return version_ > 1000u ? StaticSize() + 4 : StaticSize(); }
    
    uint32_t total_bytes_;
    int16_t version_;
    uint32_t obj_bytes_;
    Datime date_time_;
    uint16_t key_bytes_;
    uint16_t cycle_;
    uint64_t seek_key_;
};

struct SimpleFileHeader {
    void SerTo(uint8_t **buffer) const {
        put_u32(buffer, version_);
        put_u32(buffer, begin_);
        if (version_ > 1000000u) {
            put_u64(buffer, end_);
            put_u64(buffer, seek_free_);
            put_u32(buffer, nbytes_free_);
            put_u32(buffer, nfree_);
        } else {
            put_u32(buffer, end_);
            put_u32(buffer, seek_free_);
            put_u32(buffer, nbytes_free_);
            put_u32(buffer, nfree_);
        }
    }

    void DeserFrom(uint8_t **buffer) {
        version_ = get_u32(buffer);
        begin_ = get_u32(buffer);
        if (version_ > 1000000u) {
            end_ = get_u64(buffer);
            seek_free_ = get_u64(buffer);
            nbytes_free_ = get_u32(buffer);
            nfree_ = get_u32(buffer);
        } else {
            end_ = get_u32(buffer);
            seek_free_ = get_u32(buffer);
            nbytes_free_ = get_u32(buffer);
            nfree_ = get_u32(buffer);
        }
    }

    uint32_t version_;
    uint32_t begin_;
    uint64_t end_;
    uint64_t seek_free_;
    uint32_t nbytes_free_;
    uint32_t nfree_;
};

class PhysWriter {
public:
    using GenericRecord = std::pair<>

    explicit PhysWriter(SinkInterface *sink) : sink_{sink} {}

    ~PhysWriter() { sink_->Close(); delete sink_; }

    void WriteFileHeader(SimpleFileHeader const&);

    void Write();

    void Write

protected:
    SinkInterface *sink_;
};

class PhysReader {
public:
    using GenericRecord = std::pair<Key, std::vector<uint8_t>>;
    using FreeSegmentsRecord = std::pair<Key, std::vector<FreeSegment>>;

    explicit PhysReader(SourceInterface *source) : source_{source} {}

    ~PhysReader() { source_->Close(); delete source_; }

    SimpleFileHeader ReadFileHeader();

    GenericRecord Read();

    GenericRecord ReadN(uint64_t const);

    GenericRecord ReadAt(uint64_t const pos);

    GenericRecord ReadNAt(uint64_t const pos, uint64_t const nbytes);

    GenericRecord ReadByKey(Key const&);

    FreeSegmentsRecord ReadFreeSegmentsRecord(SimpleFileHeader const&);

protected:
    SourceInterface *source_;
};

}}}

#endif // 
