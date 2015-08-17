
#ifndef HEADER_TOOLS
#define HEADER_TOOLS
#include "segtable.h"



int generate(int limit);

void print_table(char *str, char *table);

void print_atable(char *str, struct ATABLE *table);

uint64_t simple_pow(int base , int exp); 

void print_bit_map(char *,char * bit_map,int size);

void generate_string(char *value, int max);

#endif