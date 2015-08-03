//segtable.h
#ifndef HEADER_SEGTABLE
#define HEADER_SEGTABLE

#include "std.h"
#include <string.h>



#define LEV0_NUM 5
#define MAX_LEV 7
#define LEVELS_SUMMARY_ENTRY 100

#define TABLE_END_RESERVED 20
#define TABLE_END_ZERO 4
#define TABLE_END_KV_BYTES 4
#define TABLE_END_KV_NUM 4

#define FINDER_KEY_LENGTH 20
#define FINDRE_NUMBER_LENGTH 10
#define FINDER_ENTRY_LENGTH 40
#define SERIAL_BASE 0

#define KV_LENGTH 64

int put(char* key,char* value);
int merge();

int merge1();
int merge2();

struct KNODE{
	char * key;
	struct KNODE * next;
	
	char *value;
};
struct ATABLE{
	int kv_bytes;//totabl bytes
	int kv_num;//kv numbers
	struct KNODE *key_head;//the key link list head


};

struct LEV0{
	char *table;
	char *next;


};

#endif