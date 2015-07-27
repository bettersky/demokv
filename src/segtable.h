//segtable.h
#include <stdint.h> 
#include <string.h>
#define LEV0_NUM 5
#define MAX_LEV 7
#define LEVELS_SUMMARY_ENTRY 100

#define FINDER_KEY_LENGTH 20
#define FINDRE_NUMBER_LENGTH 10
#define FINDER_ENTRY_LENGTH 40

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

struct LEV0{
	char *table;
	char *next;


};