#include "segtable.h"
#include "flash.h" //for device
#include <stdio.h>
#include <math.h>  //pow

extern struct ATABLE *active_table;

int merge1();
int merge2();
int merge(){
	printf("___I am merge \n");
	//step a: recursively find a not full level, if the result is lev0 go to step 1, else go to step b;
	int max_level=MAX_LEV;
	int i;
	FILE *levels_summary=fopen("../ftl/levels_summary", "r+");
	printf("levels_summary=%p\n", levels_summary);
	for(i=0;i<max_level;i++){
		fseek(levels_summary, i*LEVELS_SUMMARY_ENTRY, SEEK_SET);
		int tables_num ;
		fscanf(levels_summary, "%d", tables_num);
		if(tables_num < LEV0_NUM* (i+1) ){//here i+1 should be replaced by pow(10, i)
			break;
		}
	}
	if(i==0){
		//go to step 1
		merge1();
	}
	else{
		//go to step b
		merge2();
	}
	//step b: select a table from the higher level, the hightest could be level 0, then merge it to this not full level
			 //the merging steps is like from step 3. When selecting, an intact table would be prior. 
	
	
	//step 1: make a new pointer point to the active table's space, then malloc a new space for the active table
	
	
	//step 2: construct an ordered table from the old active table's data
	
	//step 3: select acrossed tables from lev0
	
	//step 4: merge

}

int merge1(){
//step 1
	char *active_table_old=active_table;
	active_table=NULL;
	active_table=malloc(sizeof(struct ATABLE));//this should be able to serve new writes immediatly
//step 2	
	
}
int merge2(){

}