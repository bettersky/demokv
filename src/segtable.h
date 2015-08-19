//segtable.h
#ifndef HEADER_SEGTABLE
#define HEADER_SEGTABLE

#include "std.h"
#include <string.h>


//lev 0 
#define LEV0_NUM 5
#define MAX_LEV 7
#define LEVELS_SUMMARY_ENTRY 128

#define TABLE_END_RESERVED 20
#define TABLE_END_ZERO 4
#define TABLE_END_KV_BYTES 4
#define TABLE_END_KV_NUM 4

#define FINDER_KEY_LENGTH 32
#define FINDRE_NUMBER_LENGTH 10
#define FINDER_ENTRY_LENGTH 64

#define SERIAL_BASE 0 //beginning of serial number
//lev 0
//#define KV_LENGTH 64
#define LEV_PUFFER 10

#define MAX_KEY_LENGTH 40	//for KNODE
#define MAX_VALUE_LENGTH 1024 //for KNODE

#define FLAG_LEVEL_MAX 50
#define FLAG_LEVEL_PUFFER 5


//#define FLAG_WIDTH 2048
int put(char* key,char* value);
int merge();

int merge1(int full_lev);
int merge2(int full_lev);



struct KNODE{
	//char * key;
	char key[MAX_KEY_LENGTH];
	//struct KNODE * next;
	//struct KNODE * pre;
	//struct KNODE * flag_next;
	
	struct KNODE *flag[FLAG_LEVEL_MAX];//flag[0] is equivalent to *next
	//char *value;
	char value[MAX_VALUE_LENGTH];
	//int curr_max;
};



struct ATABLE{
	int kv_bytes;//totabl bytes
	int kv_num;//kv numbers
	struct KNODE key_head;//the key link list head
	int curr_max;


};

struct LEV0{
	char *table;
	char *next;


};

struct FINDER_ENTRY{
	char first_key[FINDER_KEY_LENGTH];
	char last_key[FINDER_KEY_LENGTH];
	uint64_t serial_num;//for lev0, 
	
	struct FINDER_ENTRY *next;
	struct FINDER_ENTRY *pre;
	
	char * table;//only for lev0
	
};

struct KNODE * search_insert_point(int curr_flag_level, struct KNODE *bigger_flag_point, char *searching_key);
#endif