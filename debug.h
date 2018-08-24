#ifndef debug_h
#define debug_h

#include <stdio.h>
#include <stdlib.h>

// 
// dump raw buffer content
//
void dump_raw(char *buf, int size, int nbytes_per_row);

//
// helpful debug macros
//
#define print_char(value) \
    printf(#value" = %c\n", value)

#define print_u32(value) \
    printf(#value" = %u\n", value)

#define print_u64(value) \
    printf(#value" = %"PRIu64"\n", value)

#define print_i32(value) \
    printf(#value" = %d\n", value)

#define print_i64(value) \
    printf(#value" = %"PRId64"\n", value)

#define print_f32(value) \
    printf(#value" = %f\n", value)

#define print_f64(value) \
    printf(#value" = %lf\n", value)

#define print_u16(value) \
    printf(#value" = %hu\n", value)

#define print_i16(value) \
    printf(#value" = %hu\n", value)

#define print_string(value) \
    printf(#value" = %s\n", value);

#endif // debug_h
