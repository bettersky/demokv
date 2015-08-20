
#include "segtable.h"
extern int flag_width[];
//extern struct KNODE *active_chain_head;

extern uint64_t put_counter;
extern struct ATABLE *active_table;//defined in egtable.h

struct KNODE * search_insert_point(int curr_flag_level, struct KNODE *bigger_flag_point, char *searching_key){
	int i;
	struct KNODE *head_recorder=active_table->key_head;//bigger_flag_point;
	//printf(">>>>>search_insert_point begin, put_counter=%d>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n",put_counter);
	//printf("in search_insert_point, curr_flag_level=%d, searching_key=%s\n",curr_flag_level,  searching_key );
	

	if(bigger_flag_point==NULL){//exception
		printf("search_insert_point, error, bigger_flag_point is NULL, this should not occur, exit\n");
		exit(5);
	}
	struct KNODE* stay_flag=bigger_flag_point;
	struct KNODE* searching_flag=bigger_flag_point;//->flag[curr_flag_level];//may be NULL
	
	int step=0;
//print_knode_chain("",active_chain_head);
	while(searching_flag->flag[curr_flag_level]!=NULL){
		step++;
		//printf("curr_flag_level=%d, step=%d, flag_width[curr_flag_level+1]=%d\n",curr_flag_level, step,flag_width[curr_flag_level+1]);
		
		int com_res=strcmp(searching_key,searching_flag->flag[curr_flag_level]->key);
		if(com_res>0){
			//add new flag --begin
			if(step>=FLAG_LEVEL_PUFFER ){//set the searching_flag->flag[curr_flag_level] as a bigger flag
				//printf("insert flag ---------------,put_counter=%d\n",put_counter);
				if(head_recorder->flag[curr_flag_level+1]==NULL){//if //active_table->key_head
					//printf("curr_max+++++++++, active_table->key_head=%p, bigger_flag_point=%p, head_recorder=%p\n",
							//&(active_table->key_head),bigger_flag_point,head_recorder);
					active_table->curr_max++;
				}
				searching_flag->flag[curr_flag_level]->flag[curr_flag_level+1]=bigger_flag_point->flag[curr_flag_level+1];
				bigger_flag_point->flag[curr_flag_level+1]= searching_flag->flag[curr_flag_level];
				bigger_flag_point=bigger_flag_point->flag[curr_flag_level+1];//update bigger_flag_point
				step=0;//
			}
			//add new flag --end
			searching_flag=searching_flag->flag[curr_flag_level];
			continue;
		}
		else if(com_res<0){
			if(curr_flag_level==0){
				return searching_flag;
			}
			else{
				return search_insert_point(curr_flag_level-1, searching_flag, searching_key);
			}
		}
		else if(com_res==0){
			return searching_flag->flag[curr_flag_level];
		}
	
	}
	//now the searching_flag->flag[curr_flag_level] is NULL
	if(curr_flag_level==0){
		return searching_flag;
	}
	else{
		return search_insert_point(curr_flag_level-1, searching_flag, searching_key);
	}
	//return searching_flag;//the searching_flag->flag[curr_flag_level] is NULL
	
	
	//printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<search_insert_point end, put_counter=%d\n",put_counter);


}