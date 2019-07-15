#include "XXX/core/phys/Decls.hpp"

namespace tests { namespace extensions { namespace phys {

using namespace ::XXX::core::phys;

using FileHeaderRecord = std::pair<SimpleFileHeader, std::vector<uint8_t>>;

FileHeaderRecord readFileHeader(std::shared_ptr<SourceInterface> const& source) {
    SimpleFileHeader fh;
    std::vector<uint8_t> buffer(100);

    auto const pos = source->Pos();
    if (pos)
        source->ReadAt(0, 100, (void*)buffer.data());
    else
        source->Read(100, (void*)(buffer.data()));

    // FIXME treat various statuses
    auto* ptr = buffer.data();
    if (std::strncmp((char const*)ptr, "root", 4)) {
        std::cout << "*** not a root file ***" << std::endl;
        assert(0);
    }
    ptr+=4;
    fh.DeserFrom(&ptr);

    return {fh, std::move(buffer)};
}

void writeFileHeader(std::shared_ptr<SinkInterface> sink, FileHeaderRecord const& fhr) {
    // FIXME: not sure if this complication is necessary
    auto const pos = sink->Pos();
    if (pos)
        sink->WriteAt(0, 100, (void*)fhr.second.data());
    else
        sink->Write(100, (void*)(fhr.second.data()));
}

}}}
