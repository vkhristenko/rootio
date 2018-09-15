#include <iostream>

namespace root {

extern "C" {
#include "iolayer.h"
#include "simulations.h"
#include "aux.h"
}

}

#include "TBufferFile.h"
#include "TH1F.h"

using namespace root;

generic_record_t simulate_hist_record(llio_t const& llio, directory_t const& dir) 
{
    // create a hist and fill
    TH1F *hist = new TH1F("hist", "hist", 10, 0, 10);
    for (int i = 0; i<10; i++)
        hist->Fill(i);
    std::cout << "hist size = " << hist->GetSize() << std::endl;
    TBufferFile buffer(TBuffer::kWrite, hist->GetSize());
    hist->Streamer(buffer);
    char *raw = buffer.Buffer();
    std::cout << "buffer size = " << buffer.Length() << std::endl;

    // create a record (note)
    generic_record_t record;
    record.blob = raw;
    string_t class_name, obj_name, obj_title;
    ctor_nomemcopy_pstring(&class_name, "TH1F", 4);
    ctor_nomemcopy_pstring(&obj_name, "hist", 4);
    ctor_nomemcopy_pstring(&obj_title, "hist", 4);
    ctor_withnames_key(&record.key,
                       &class_name,
                       &obj_name,
                       &obj_title);
    record.key.seek_pdir = dir.seek_dir;
    record.key.key_bytes = size_key(&record.key);
    record.key.obj_bytes = buffer.Length();
    record.key.total_bytes = record.key.obj_bytes + record.key.key_bytes;

    buffer.DetachBuffer();
    return record;
}

keys_list_record_t simulate_keys_list_record_for_dir(llio_t const& llio,
    directory_t const& dir, generic_record_t &rec) 
{
    keys_list_record_t record;
    record.length = 1;
    record.pkeys = &rec.key;

    ctor_key(&record.key);
    record.key.seek_pdir = dir.seek_dir;
    record.key.key_bytes = size_key(&record.key);
    record.key.obj_bytes = 4 + size_key(&rec.key);
    record.key.total_bytes = record.key.obj_bytes + record.key.key_bytes;

    return record;
}

int main(int argc, char **argv) 
{
    std::cout << "hello world\n";
    if (argc < 2) {
        std::cout << "no input filename provided" << std::endl;
        exit(1);
    }

    std::string filename {argv[1]};
    llio_t llio = open_to_write(filename.c_str());

    simulate_streamer_record(&llio);
    simulate_free_segments_record(&llio);

    TH1F *hist = new TH1F("hist", "hist", 10, 0, 10);
    for (int i = 0; i<10; i++)
        hist->Fill(i);
    std::cout << "hist size = " << hist->GetSize() << std::endl;
    TBufferFile buffer(TBuffer::kWrite, hist->GetSize());
    hist->Streamer(buffer);
    std::cout << "buffer size = " << buffer.Length() << std::endl;

    // simulate the TH1D record
    auto hist_record = simulate_hist_record(llio, llio.top_dir_rec.dir);
    // simulate the keys list
    auto keys_list_record = simulate_keys_list_record_for_dir(llio, llio.top_dir_rec.dir,
        hist_record);
    // write the record with TH1D
    write_generic_record(&llio, &hist_record);
    // write the keys list record
    write_keys_list_record_for_dir(&llio, &keys_list_record, 
        &llio.top_dir_rec.dir);

    close_from_write(&llio);
}
