#include "aux.h"

void swap_16(char *src, char *dest) {
    dest[0] = src[1];
    dest[1] = src[0];
}

void swap_32(char *src, char *dest) {
    dest[0] = src[3];
    dest[1] = src[2];
    dest[2] = src[1];
    dest[3] = src[0];
}

void swap_64(char *src, char *dest) {
    dest[0] = src[7];
    dest[1] = src[6];
    dest[2] = src[5];
    dest[3] = src[4];
    dest[4] = src[3];
    dest[5] = src[2];
    dest[6] = src[1];
    dest[7] = src[0];
}

uint16_t get_u16(char ** ptr) {
    uint16_t value;
    swap_16(*ptr, (char*)&value);
    *ptr+=2;
    return value;
}

int16_t get_i16(char **ptr) {
    return  (int16_t)get_u16(ptr);
}

int32_t get_i32(char **ptr) {
    int value;
    swap_32(*ptr, (char*)&value);
    *ptr+=4;
    return value;
}

uint32_t get_u32(char ** ptr) {
    return (uint32_t)get_i32(ptr);
}

uint64_t get_u64(char **ptr) {
    uint64_t value;
    swap_64(*ptr, (char*)&value);
    *ptr+=8;
    return value;
}

int64_t get_i64(char **ptr) {
    return (int64_t)get_u64(ptr);
}

double get_f64(char **ptr) {
    return (double)get_u64(ptr);
}

float get_f32(char **ptr) {
    return (float)get_u32(ptr);
}

uint32_t get_version(char **ptr) {
    uint16_t v = get_u16(ptr);
    if ((v & 0x4000) == 0) 
        return (uint32_t)v;

    uint32_t nbytes = ((v & 0x3fff) << 16) + get_u16(ptr);
    uint16_t res = get_u16(ptr);
    return res;
}

void get_string(char **src, char **dest) {
    int8_t size = **src; (*src)++;

    // If the string is empty
    if (size == 0) {
        (*dest) = malloc(1);
        (*dest)[0] = '\0';
        return;
    }
    
    // if more input is needed
    if (size == -1) {
        int size = get_i32(src); 
        (*dest) = malloc(size);
        memcpy(*dest, *src, size);
        src+=size;
        return;
    }

    // allocate size bytes and perform mem copy
    (*dest) = malloc(size);
    memcpy(*dest, *src, size);
    *src+=size;
}
