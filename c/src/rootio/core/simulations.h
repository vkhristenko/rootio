#ifndef mops_core_simulations_h
#define mops_core_simulations_h

#include "rootio/core/iolayer.h"

void simulate_streamer_record(struct localfs_file_context_t*);
void simulate_free_segments_record(struct localfs_file_context_t*);

#endif // mops_core_simulations_h
