#include "aux.h"
#include <inttypes.h>

void deallocate_char_p(char *ptr) 
{
    deallocate((void*) ptr);
}

void deallocate(void *ptr) 
{
    free(ptr);
}

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

uint8_t get_u8(char **ptr) {
    uint8_t value = (uint8_t)**ptr;
    (*ptr)++;
    return value;
}

uint16_t get_u16(char ** ptr) {
    uint16_t value;
    swap_16(*ptr, (char*)&value);
    *ptr+=2;
    return value;
}

void put_u16(char **ptr, uint16_t value) {
    swap_16((char*)&value, *ptr);
    *ptr+=2;
}

int16_t get_i16(char **ptr) {
    return  (int16_t)get_u16(ptr);
}

void put_i16(char **ptr, int16_t value) {
    put_u16(ptr, (uint16_t)value);
}

int32_t get_i32(char **ptr) {
    int value;
    swap_32(*ptr, (char*)&value);
    *ptr+=4;
    return value;
}

void put_i32(char **ptr, int32_t value) {
    swap_32((char*)&value, *ptr);
    *ptr+=4;
}

uint32_t get_u32(char ** ptr) {
    return (uint32_t)get_i32(ptr);
}

void put_u32(char **ptr, uint32_t value) {
    put_i32(ptr, (int32_t)value);
}

uint64_t get_u64(char **ptr) {
    uint64_t value;
    swap_64(*ptr, (char*)&value);
    *ptr+=8;
    return value;
}

void put_u8(char **ptr, uint8_t value) {
    **ptr = (char)value;
    (*ptr)++;
}

void put_u64(char **ptr, uint64_t value) {
    swap_64((char*)&value, *ptr);
    *ptr+=4;
}

int64_t get_i64(char **ptr) {
    return (int64_t)get_u64(ptr);
}

void put_i64(char **ptr, int64_t value) {
    put_u64(ptr, (uint64_t) value);
}

double get_f64(char **ptr) {
    return (double)get_u64(ptr);
}

void put_f64(char **ptr, double value) {
    put_u64(ptr, (uint64_t)value);
}

float get_f32(char **ptr) {
    return (float)get_u32(ptr);
}

void put_f32(char **ptr, float value) {
    put_u32(ptr, (uint32_t)value);
}

uint32_t get_version(char **ptr) {
    uint16_t v = get_u16(ptr);
    if ((v & 0x4000) == 0) 
        return (uint32_t)v;

    uint32_t nbytes = ((v & 0x3fff) << 16) + get_u16(ptr);
    uint16_t res = get_u16(ptr);
    return res;
}

void put_version(char **ptr, uint16_t version) {
    put_u16(ptr, version);
}

int get_string(char **src, char **dest) {
    uint8_t size = **src; (*src)++;
    uint8_t minus_one = -1;

    // If the string is empty
    if (size == 0) {
        (*dest) = malloc(1);
        (*dest)[0] = '\0';
        return 0;
    }
    
    // if more input is needed
    if (size == minus_one) {
        int size = get_i32(src); 
        (*dest) = malloc(size+1);
        (*dest)[size] = '\0';
        memcpy(*dest, *src, size);
        src+=size;
        return size;
    }

    // allocate size bytes and perform mem copy
    (*dest) = malloc(size+1);
    (*dest)[size] = '\0';
    memcpy(*dest, *src, size);
    *src+=size;

    return size;
}

void put_string(char **pbuf, char const* str, int size) {
    if (size == 0) {
        **pbuf = 0;
        (*pbuf)++;
        return;
    } else if (size < 255) {
        **pbuf = (char) size;
        (*pbuf)++;
    } else {
        **pbuf = -1; (*pbuf)++;
        put_i32(pbuf, (int32_t) size);
    } 

    memcpy(*pbuf, str, size);
    *pbuf+=size;
}
