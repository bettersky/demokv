#include <stdio.h>

#include "main.h"
#include "flash.h"
#include "segtable.h"

char * lev0[LEV0_NUM];

struct DEVICE device;

struct ATABLE *active_table;


int test(){
	printf("I m test \n");
	char *args="/dev/sdp";
	flash_init(args);
	
	write();

}