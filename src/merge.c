#include "segtable.h"
#include "flash.h" //for device
#include "std.h"
#include <math.h>  //pow

extern struct ATABLE *active_table;

extern char *levels_summary;

extern uint64_t serials_width[];

extern uint64_t put_counter;//in seq_write


int merge(){
	//printf(">>>>>>>>>>>>>>>>>>>>>>>I am merge, levels_summary=%p \n", levels_summary);
	//step a: recursively find a not full level, if the result is lev0 go to step 1, else go to step b;
	int max_level=MAX_LEV;//7
	int i;
	
	while(1){//until the lev0 is not full
		for(i=0;i<max_level;i++){
			//fseek(levels_summary, i*LEVELS_SUMMARY_ENTRY, SEEK_SET);
			uint32_t tables_num ;
			tables_num=*(uint32_t*)(levels_summary +i*LEVELS_SUMMARY_ENTRY);
			//fscanf(levels_summary, "%d", &tables_num);
			//printf("/////////////level %d: tables_num=%d ,levels_summary+i*LEVELS_SUMMARY_ENTRY=%p \n",i,tables_num,levels_summary+i*LEVELS_SUMMARY_ENTRY);
			
			if(tables_num<serials_width[i]){
				break;
			}
			
		}
		merge1(i-1);
		if(i==0){//lev 0 is not full				
			break;//break the while			
		}
		//else{
			//go to step b
			//selected level is i-1, because level i is not full
			//printf("comes to merge2, i=%d\n",i);		
			//merge1(i-1);
			//merge2(i-1);
		//}
	}
		//step b: select a table from the higher level, the hightest could be level 0, then merge it to this not full level
				 //the merging steps is like from step 3. When selecting, an intact table would be prior. 
		
		
		//step 1: make a new pointer point to the active table's space, then malloc a new space for the active table
		
		
		//step 2: construct an ordered table from the old active table's data
		
		//step 3: select acrossed tables from lev0
		
		//step 4: merge
		
		printf("In merge end, put_counter=%llu\r",put_counter);
		//printf("\n");

}

