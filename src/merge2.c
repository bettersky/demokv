#include "segtable.h"
#include "flash.h" //for device
#include "tools.h"

#include <stdio.h>
#include <math.h>  //pow

extern struct ATABLE *active_table;
extern struct LEV0 * lev0_head;
extern char *table_finder_0;
extern char *lev0_tables[];
extern char *levels_summary;

int merge2(){
	printf("I am merge 2, levels_summary=%p\n",levels_summary);
	int i;
	for(i=0;i<LEV0_NUM;i++){
		printf("lev0_tbales[%d]=%p\n",i, lev0_tables[i]);
	}
	
	print_table("lev0_tables[0]",lev0_tables[0]);
	print_table("lev0_tables[1]",lev0_tables[1]);
	print_table("lev0_tables[2]",lev0_tables[2]);
	print_table("lev0_tables[3]",lev0_tables[3]);
	print_table("lev0_tables[4]",lev0_tables[4]);
	exit(1);
}