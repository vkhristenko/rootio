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

#include <vector>

#include "XXX/core/common/aux.hpp"

namespace XXX { namespace core { namespace phys {

using namespace ::XXX::core::common;

struct Seekable {
    virtual void SeekTo(int64_t const pos) = 0;
    virtual int64_t Pos() const = 0;
};

struct Closeable {
    virtual void Close() = 0;
};

struct Reservable {
    virtual void Reserve(int64_t const nbytes) = 0;
    virtual void Reserve(int64_t const pos, int64_t const nbytes) = 0;
};

class SourceInterface : public Seekable, public Closeable {
public:
    explicit SourceInterface(std::string const& path) : path_{path} {}

    virtual ~SourceInterface() {}

    virtual void Read(int64_t const nbytes, void *data) = 0;

    virtual void ReadAt(int64_t const pos, int64_t const nbytes, void *data) = 0;

protected:
    std::string path_;
};

class SinkInterface : public Seekable, public Closeable , public Reservable {
public:
    explicit SinkInterface(std::string const& path) : path_{path} {}

    virtual ~SinkInterface() {}

    virtual void Write(int64_t const nbytes, void *data) = 0;

    virtual void WriteAt(int64_t const pos, int64_t const nbytes, void *data) = 0;

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

    ~OutputFile() override { this->Close(); }

    // FIXME should be the same for both interfaces
    void SeekTo(int64_t const pos) override { lseek(fd_, pos, SEEK_SET); }
    int64_t Pos() const override { return lseek(fd_, 0, SEEK_CUR); }
    
    void Write(int64_t nbytes, void *data) override { write(fd_, (void*)data, nbytes); }

    void WriteAt(int64_t const pos, int64_t const nbytes, void *data) override {
        this->SeekTo(pos);
        this->Write(nbytes, data);
    }

    void Reserve(int64_t const nbytes) override {
        std::vector<uint8_t> buffer(nbytes, 0);
        this->Write(nbytes, buffer.data());
    }

    void Reserve(int64_t const pos, int64_t const nbytes) override {
        std::vector<uint8_t> buffer(nbytes, 0);
        this->WriteAt(pos, nbytes, buffer.data());
    }
  
protected:
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

    ~InputFile() override { this->Close(); }

    void SeekTo(int64_t const pos) override { lseek(fd_, pos, SEEK_SET); }
    int64_t Pos() const override { return lseek(fd_, 0, SEEK_CUR); }

    void Read(int64_t const nbytes, void *data) override { read(fd_, data, nbytes); }

    void ReadAt(int64_t const pos, int64_t const nbytes, void *data) override {
        this->SeekTo(pos);
        this->Read(nbytes, data);
    }

protected:
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

    void DeserFrom(uint8_t const** buffer) {
        version_ = get_version(buffer);
        id_ = get_u32(buffer);
        bits_ = get_u32(buffer);
    }

    static uint32_t StaticSize() {
        return 10u;
    }

    int16_t version_;
    uint32_t id_;
    uint32_t bits_;
};

struct FreeSegment {
    void SerTo(uint8_t** buffer) const {
        put_version(buffer, version_);
        if (version_ > 1000) {
            put_i64(buffer, begin_);
            put_i64(buffer, end_);
        } else {
            put_i32(buffer, static_cast<int32_t>(begin_));
            put_i32(buffer, static_cast<int32_t>(end_));
        }
    }

    void DeserFrom(uint8_t const** buffer) {
        version_ = get_version(buffer);
        begin_ = version_ > 1000 ? get_i64(buffer) : get_i32(buffer);
        end_ = version_ > 1000 ? get_i64(buffer) : get_i32(buffer);
    }

    static uint32_t StaticSize() { return 10; }
    uint32_t Size() const { return version_ > 1000 ? StaticSize() + 8 : StaticSize();  }

    int16_t version_;
    int64_t begin_, end_;
};

struct Datime {
    void SerTo(uint8_t **buffer) const {
        put_u32(buffer, raw_);
    }

    void DeserFrom(uint8_t const** buffer) {
        raw_ = get_u32(buffer);
    }

    static uint32_t StaticSize() { return 4; }

    uint32_t raw_;
};

struct Key {
    void SerTo(uint8_t **buffer) const {
        put_i32(buffer, total_bytes_);
        put_version(buffer, version_);
        put_i32(buffer, obj_bytes_);
        date_time_.SerTo(buffer);
        put_i16(buffer, key_bytes_);
        put_i16(buffer, cycle_);
        if (version_ > 1000)
            put_i64(buffer, seek_key_);
        else 
            put_i32(buffer, static_cast<int32_t>(seek_key_));
    }

    void DeserFrom(uint8_t const** buffer) {
        total_bytes_ = get_i32(buffer);
        version_ = get_version(buffer);
        obj_bytes_ = get_i32(buffer);
        date_time_.DeserFrom(buffer);
        key_bytes_ = get_i16(buffer);
        cycle_ = get_i16(buffer);
        if (version_ > 1000)
            seek_key_ = get_i64(buffer);
        else 
            seek_key_ = get_i32(buffer);
    }

    static uint32_t StaticSize() { return Datime::StaticSize() + 18; }
    uint32_t Size() const { return version_ > 1000 ? StaticSize() + 4 : StaticSize(); }
    
    int32_t total_bytes_;
    int32_t version_;
    int32_t obj_bytes_;
    Datime date_time_;
    int16_t key_bytes_;
    int16_t cycle_;
    int64_t seek_key_;
};

struct SimpleFileHeader {
    void SerTo(uint8_t **buffer) const {
        put_i32(buffer, version_);
        put_i32(buffer, begin_);
        if (version_ > 1000000) {
            put_i64(buffer, end_);
            put_i64(buffer, seek_free_);
            put_i32(buffer, nbytes_free_);
            put_i32(buffer, nfree_);
        } else {
            put_i32(buffer, end_);
            put_i32(buffer, seek_free_);
            put_i32(buffer, nbytes_free_);
            put_i32(buffer, nfree_);
        }
    }

    void DeserFrom(uint8_t const** buffer) {
        version_ = get_i32(buffer);
        begin_ = get_i32(buffer);
        if (version_ > 1000000) {
            end_ = get_i64(buffer);
            seek_free_ = get_i64(buffer);
            nbytes_free_ = get_i32(buffer);
            nfree_ = get_i32(buffer);
        } else {
            end_ = get_i32(buffer);
            seek_free_ = get_i32(buffer);
            nbytes_free_ = get_i32(buffer);
            nfree_ = get_i32(buffer);
        }
    }

    int32_t version_;
    int32_t begin_;
    int64_t end_;
    int64_t seek_free_;
    int32_t nbytes_free_;
    int32_t nfree_;
};

class RecordWriter {
public:
    using GenericRecord = std::pair<Key, std::vector<uint8_t>>;

    explicit RecordWriter(std::shared_ptr<SinkInterface> const& sink) 
        : sink_{sink} 
    {}

    ~RecordWriter() {}

    void Write(GenericRecord const&);

    void TryWrite(GenericRecord const&);

    // with seek + write
    void WriteAt(GenericRecord const&);

    void TryWriteAt(int64_t const, GenericRecord const&);

protected:
    std::shared_ptr<SinkInterface> sink_;
};

class RecordReader {
public:
    using GenericRecord = std::pair<Key, std::vector<uint8_t>>;

    explicit RecordReader(std::shared_ptr<SourceInterface> const& source) 
        : source_{source} 
    {}

    ~RecordReader() {}

    GenericRecord TryRead();

    GenericRecord ReadN(int64_t const);

    GenericRecord TryReadAt(int64_t const pos);

    GenericRecord ReadNAt(int64_t const pos, int64_t const nbytes);

    GenericRecord ReadByKey(Key const&);

    //FreeSegmentsRecord ReadFreeSegmentsRecord(SimpleFileHeader const&);

protected:
    std::shared_ptr<SourceInterface> source_;
};

}}}

#endif // 
