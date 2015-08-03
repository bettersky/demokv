#include "segtable.h"
#include "flash.h" //for device
#include "std.h"
#include <math.h>  //pow

extern struct ATABLE *active_table;
extern struct LEV0 * lev0_head;
extern char *table_finder_0;
extern char *lev0_tables[];
extern char *levels_summary;


int merge(){
	printf(">>>>>>>>>>>>>>>>>>>>>>>I am merge, levels_summary=%p \n", levels_summary);
	//step a: recursively find a not full level, if the result is lev0 go to step 1, else go to step b;
	int max_level=MAX_LEV;//7
	int i;
	
	
	while(1){//until the lev0 is not full
		for(i=0;i<max_level;i++){
			//fseek(levels_summary, i*LEVELS_SUMMARY_ENTRY, SEEK_SET);
			uint32_t tables_num ;
			tables_num=*(uint32_t*)(levels_summary +i*LEVELS_SUMMARY_ENTRY);
			//fscanf(levels_summary, "%d", &tables_num);
			printf("/////////////level %d: tables_num=%X ,levels_summary=%s \n",i,tables_num,levels_summary+i*LEVELS_SUMMARY_ENTRY);
			
			if(tables_num < LEV0_NUM* (i+1) ){//here i+1 should be replaced by pow(10, i)
				break;//break the for
			}
		}
		if(i==0){//lev 0 is not full
			int static merge1_counter=1;
			//go to step 1
			//printf("___________merge1_counter=%d\n",merge1_counter++);
			
			merge1();
			break;//break the while
			//printf("XXXXXXXXXXXXXXXXXXXXXX\n");
		}
		else{
			//go to step b
			//selected level is i-1, because level i is not full
			printf("comes to merge2, i=%d\n",i);
			exit(1);
			merge2();
		}
	}
		//step b: select a table from the higher level, the hightest could be level 0, then merge it to this not full level
				 //the merging steps is like from step 3. When selecting, an intact table would be prior. 
		
		
		//step 1: make a new pointer point to the active table's space, then malloc a new space for the active table
		
		
		//step 2: construct an ordered table from the old active table's data
		
		//step 3: select acrossed tables from lev0
		
		//step 4: merge
		
		printf("merge end\n");

}

int merge2(){
	printf("levels_summary=%p\n",levels_summary);
	*levels_summary ='V';
	exit(1);
}