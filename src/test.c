#include <stdio.h>

#include "main.h"
#include "flash.h"
#include "segtable.h"

struct LEV0 * lev0_head;

char * lev0_tables[LEV0_NUM];

struct DEVICE device;

struct ATABLE *active_table;

char *table_finder_0;


int test(){
	printf("I m test \n");
	char *args="/dev/sdp";
	flash_init(args);
	
	kv_write();

}