#ifndef XXX_CORE_PHYS_DECLS_H
#define XXX_CORE_PHYS_DECLS_H

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "XXX/core/common/aux.hpp"

namespace XXX { namespace core { namespace phys {

using namespace ::XXX::core::common;

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

struct GenericRecord {
    Key key_;
    uint8_t * blob_;
};

}}}

#endif // 
