#ifndef mops_core_simulations_h
#define mops_core_simulations_h

#include "rootio/core/logical_structure.h"

struct streamer_record_t        simulate_streamer_record(
    struct logical_structure_t*);
struct free_segments_record_t   simulate_free_segments_record(
    struct logical_structure_t*);

#endif // mops_core_simulations_h
