#include "std.h"
#include <math.h>
#include "main.h"
#include "flash.h"
#include "segtable.h"

struct LEV0 * lev0_head;

char * lev0_tables[LEV0_NUM];//pointers to the lev0 tables in memory

struct DEVICE device;//stores the deive segment size, segment toal and other related things

struct ATABLE *active_table;//the table for serving put requestes

char *table_finder_0;//a pointer point to the table_finder_0 ftl file
char *levels_summary;//a pointer point to the levels_summary ftl file

uint64_t serials_base[MAX_LEV]; //all levels' base serial number. 0 is reserved
uint64_t serials_width[MAX_LEV];// all levels' serial width. decided by the LEV)_NUM and LEV_PUFFER

char *seg_bit_maps[MAX_LEV];//pointers of all levels' segment space bit map. but currently they are byte map ^^


struct FINDER_ENTRY *tip_tables_entry[MAX_LEV]; //pointers of the last tables of every level if the level is full. 0 is reserved to lev 0
													//NULL indicates the level is not full
												//update 8.8: don't requir the level full.
struct FINDER_ENTRY *first_tables_entry[MAX_LEV];//pointers of the first table of every level if the level is not blank. 0 is reserved to lev 0
														//NULL indicates the level is blank


														
int test(){
	printf("I m test \n");
	char *args="/dev/sdp";
	flash_init(args);
	pow(3,4);
	kv_write();

}