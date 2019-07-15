#ifndef XXX_CORE_BASELOGIC_H
#define XXX_CORE_BASELOGIC_H

#include "XXX/core/phys/Decls.hpp"

using namespace ::XXX::core::phys;

namespace XXX { namespace core { namespace baselogic {

struct BlobInfo {
    std::string className_;
    std::string objName_;
    std::string objTitle_;

    void SerTo(uint8_t **buffer) {
    }

    void DeserFrom(uint8_t const** buffer) {
    }
};

struct LinkableKey {
    ::XXX::core::phys::Key physKey_; // contains link to "itself": Record for that key
    int64_t seek_pdir_;         // link to the parent Directory Header Record
    BlobInfo blobInfo_;
    
    void SerTo(uint8_t **buffer) {
    }

    void DeserFrom(uint8_t const** buffer) {
    }
};

struct DirectoryHeader {
    int32_t version_;
    Datime date_time_c_;
    Datime date_time_m_;
    int32_t nbytes_keys_;
    int32_t nbytes_name_;
    int64_t seek_dir_;          // link to the "itself": Directory Header Record
    int64_t seek_parent_;       // link to the parent Directory Header Record
    int64_t seek_keys_;         // link to the Directory Keys List Record

    void SerTo(uint8_t **buffer) const {

    }

    void DeserFrom(uint8_t const** buffer) {

    }
};

using LinkableKeysVector = std::vector<LinkableKey>;

}}}

#endif // XXX_CORE_BASELOGIC_H
