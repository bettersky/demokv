//segtable.h
#include <stdint.h> 
#include <string.h>
#define LEV0_NUM 5
#define MAX_LEV 7
#define LEVELS_SUMMARY_ENTRY 100

int put(char* key,char* value);
int merge();

struct KNODE{
	char * key;
	struct KNODE * next;
	
	char *value;
};
struct ATABLE{
	uint64_t size;
	int kv_num;
	struct KNODE *key_head;


};
