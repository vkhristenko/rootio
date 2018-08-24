#ifndef aux_h
#define aux_h

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

// 
// swap the byte order
//
void swap_16(char *src, char *dest);
void swap_32(char *src, char *dest);
void swap_64(char *src, char *dest);

//
// 
//
uint16_t get_u16(char ** ptr);
int16_t get_i16(char **ptr);
int32_t get_i32(char **ptr);
uint32_t get_u32(char ** ptr);
uint64_t get_u64(char **ptr);
int64_t get_i64(char **ptr);
double get_f64(char **ptr);
float get_f32(char **ptr);
uint32_t get_version(char **ptr);
void get_string(char **src, char **dest);

// 
// bdebug stuff
//
/*
struct Buffer {
    size_t nbytes;
    char *buffer;
};
void dump_buffer(struct Buffer buf, int nbytes_per_row) {
    printf("dumping buffer of size = %lu\n", buf.nbytes);
    for (size_t i=0; i<nbytes_per_row*3; i++)
        printf("-");
    printf("\n");
    for (size_t i=0; i<buf.nbytes; i++) {
        if (i%nbytes_per_row == 0 && i>0)
            printf("\n");
        printf("%02X ", buf.buffer[i]);
    }
    printf("\n");
}
void dump_raw(char *buf, int size, int nbytes_per_row) {
    printf("dumping buffer of size = %d\n", size);
    for (size_t i=0; i<nbytes_per_row*3; i++)
        printf("-");
    printf("\n");
    for (size_t i=0; i<size; i++) {
        if (i%nbytes_per_row == 0 && i>0)
            printf("\n");
        printf("%02X ", buf[i]);
    }
    printf("\n");
}
*/

#endif
