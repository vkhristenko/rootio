#include <iostream>

namespace rootio { namespace capi {

extern "C" {
#include "rootio/core/localfs.h"
#include "rootio/core/simulations.h"
}

}}

#include "TH1F.h"
#include "TBufferFile.h"

rootio::capi::generic_record_t make_hist_record(
        rootio::capi::localfs_file_context_t const& ctx, 
        rootio::capi::directory_t const& dir) {
    rootio::capi::generic_record_t record;

    // create and fill a histogram
    TH1F hist {"hist", "hist", 10, 0, 10};
    for (int i=0; i<10; ++i)
        hist.Fill(i);
    TBufferFile buffer{TBuffer::kWrite, hist.GetSize()};
    hist.Streamer(buffer);
    auto raw = buffer.Buffer();

    // create a record
    record.blob = raw;
    rootio::capi::string_t class_name, obj_name, obj_title;
    rootio::capi::ctor_nomemcopy_pstring(&class_name, "TH1F", 4);
    rootio::capi::ctor_nomemcopy_pstring(&obj_name, "hist", 4);
    rootio::capi::ctor_nomemcopy_pstring(&obj_title, "hist", 4);
    rootio::capi::ctor_withnames_key(&record.key,
                       &class_name,
                       &obj_name,
                       &obj_title);
    record.key.seek_pdir = dir.seek_dir;
    record.key.key_bytes = rootio::capi::size_key(&record.key);
    record.key.obj_bytes = buffer.Length();
    record.key.total_bytes = record.key.obj_bytes + record.key.key_bytes;

    return record;
}

rootio::capi::keys_list_record_t make_keys_list_record_for_dir(
        rootio::capi::localfs_file_context_t const& ctx, 
        rootio::capi::directory_t const& dir, 
        rootio::capi::generic_record_t &rec) {
    rootio::capi::keys_list_record_t record;
    record.length = 1;
    record.pkeys = &rec.key;

    rootio::capi::ctor_key(&record.key);
    record.key.seek_pdir = dir.seek_dir;
    record.key.key_bytes = rootio::capi::size_key(&record.key);
    record.key.obj_bytes = 4 + rootio::capi::size_key(&rec.key);
    record.key.total_bytes = record.key.obj_bytes + record.key.key_bytes;

    return record;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "no filename specified" << std::endl;
        return 0;
    }

    std::string filename {argv[1]};
    auto ctx = rootio::capi::localfs_open_to_write(filename.c_str());

    // simulate required records
    auto streamer_record = 
        rootio::capi::simulate_streamer_record(&ctx.structure);
    auto free_segments_record = 
        rootio::capi::simulate_free_segments_record(&ctx.structure);

    // data records
    auto hist_record = make_hist_record(ctx, ctx.structure.top_dir_rec.dir);
    auto keys_list_record = make_keys_list_record_for_dir(ctx, 
        ctx.structure.top_dir_rec.dir, hist_record);
    rootio::capi::localfs_write_generic_record(&ctx, &hist_record);
    rootio::capi::localfs_write_keys_list_record_for_dir(&ctx, &keys_list_record,
        &ctx.structure.top_dir_rec.dir);

    // sequence of actions upon closure
    rootio::capi::localfs_write_streamer_record(&ctx, &streamer_record);
    rootio::capi::localfs_write_free_segments_record(&ctx, &free_segments_record);
    rootio::capi::localfs_close_from_write(&ctx);

    return 0;
}
