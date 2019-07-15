#ifndef XXX_CORE_PHYS_EXTENSIONS_H
#define XXX_CORE_PHYS_EXTENSIONS_H

#include "XXX/core/phys/Decls.hpp"

namespace XXX { namespace core { namespace phys {

using FileHeaderRecord = std::pair<SimpleFileHeader, std::vector<uint8_t>>;

FileHeaderRecord readFileHeader(
        std::shared_ptr<SourceInterface> const&);

void writeFileHeader(
        std::shared_ptr<SinkInterface> const& sink, 
        FileHeaderRecord const& fhr);

std::vector<FreeSegment> parseFreeSegmentsRecord(
        RecordReader::GenericRecord const& record,
        SimpleFileHeader const& fh);

}}}

#endif // XXX_CORE_PHYS_EXTENSIONS_H
