#ifndef mops_core_aux_h
#define mops_core_aux_h

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

//
// TODO: use libc free directly
//
void deallocate_char_p(char *ptr);
void deallocate(void *);

// 
// swap the byte order
//
void swap_16(char *src, char *dest);
void swap_32(char *src, char *dest);
void swap_64(char *src, char *dest);

//
// 
//
uint8_t get_u8(char **ptr);
uint16_t get_u16(char ** ptr);
int16_t get_i16(char **ptr);
int32_t get_i32(char **ptr);
uint32_t get_u32(char ** ptr);
uint64_t get_u64(char **ptr);
int64_t get_i64(char **ptr);
double get_f64(char **ptr);
float get_f32(char **ptr);
uint32_t get_version(char **ptr);
int get_string(char **src, char **dest);

void put_u8(char **ptr, uint8_t);
void put_u16(char **ptr, uint16_t);
void put_u32(char **ptr, uint32_t);
void put_u64(char **ptr, uint64_t);
void put_i16(char **ptr, int16_t);
void put_i32(char **ptr, int32_t);
void put_i64(char **ptr, int64_t);
void put_version(char **ptr, uint16_t);
void put_string(char **ptr, char const*, int);

#endif // mops_core_aux_h
