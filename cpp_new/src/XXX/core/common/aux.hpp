#ifndef mops_core_aux_h
#define mops_core_aux_h

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <string>

namespace XXX { namespace core { namespace common {

// 
// swap the byte order
//
void swap_16(uint8_t const*src, uint8_t *dest);
void swap_32(uint8_t const*src, uint8_t *dest);
void swap_64(uint8_t const*src, uint8_t *dest);

//
// 
//
uint8_t get_u8(uint8_t const**ptr);
uint16_t get_u16(uint8_t const** ptr);
int16_t get_i16(uint8_t const**ptr);
int32_t get_i32(uint8_t const**ptr);
uint32_t get_u32(uint8_t const** ptr);
uint64_t get_u64(uint8_t const**ptr);
int64_t get_i64(uint8_t const**ptr);
double get_f64(uint8_t const**ptr);
float get_f32(uint8_t const**ptr);
int16_t get_version(uint8_t const**ptr);
int get_string(uint8_t const**src, uint8_t **dest);

void put_u8(uint8_t **ptr, uint8_t);
void put_u16(uint8_t **ptr, uint16_t);
void put_u32(uint8_t **ptr, uint32_t);
void put_u64(uint8_t **ptr, uint64_t);
void put_i16(uint8_t **ptr, int16_t);
void put_i32(uint8_t **ptr, int32_t);
void put_i64(uint8_t **ptr, int64_t);
void put_version(uint8_t **ptr, int16_t);
void put_string(uint8_t **ptr, uint8_t const*, int);

std::string get_string(uint8_t const**buffer);
void put_string(uint8_t **buffer, std::string const&);

}}}

#endif // mops_core_aux_h
