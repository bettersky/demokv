#include "segtable.h"
#include "flash.h" //for device

extern struct DEVICE device;//defined in flash.h
extern struct ATABLE *active_table;//defined in egtable.h
extern char *levels_summary;
extern put_counter;
//extern struct KNODE * active_chain_head;

int put(char* key,char* value){
	//printf("I am put\n");
	int key_size=strlen(key);
	int value_size=strlen(value);
	if(key_size==0) {
		printf("!!!!KEY is blank. In put\n");
		exit(1);	
	}
	//printf("key=%s, key size=%d,value=%s,value size=%d\n",key,key_size,value, value_size);
	//decide whether need to merge --begin
	if(active_table->kv_bytes +key_size+value_size +2 +TABLE_END_RESERVED > device.segment_bytes){//merge to lev0
		//merge to lev0 code --begin
		//printf("___________-I am put,levels_summary=%p \n",levels_summary );
		//exit(1);
		merge();
		//merge to lev0 code --end
		//put(key,value);//after merging, call put() again to finish putting
	}
	//decide whether need to merge --end
	//put in --begin
	struct KNODE *new_kv=(struct KNODE *)malloc(sizeof(struct KNODE));
	memset(new_kv,0, sizeof(struct KNODE));
	strcpy(new_kv->key, key);
	strcpy(new_kv->value, value);
	
	//using flag_alg --begin
	
	//int cmp_res;
	if(active_table->key_head->flag[0]!=NULL){		//active_table->key_head replaces  active_chain_head
			char *searching_key=key;
			struct KNODE* insert_point=search_insert_point(active_table->curr_max, active_table->key_head, searching_key);
			int com_res=strcmp(key, insert_point->key);
			if(com_res!=0){//then must be >0 and less than the next key, so we insert
				new_kv->flag[0]=insert_point->flag[0];
				insert_point->flag[0]=new_kv;
			}
			else if(com_res==0){
				//printf("YYYYYYYYYYYYYY, equal\n");
			}
	}
	else{
		//active_chain_head->next=new_kv;
		active_table->key_head->flag[0]=new_kv;
	
	}
	
	active_table->kv_num++;
	active_table->kv_bytes+=key_size+value_size + 2; //2 '\0' characters 
	//print_knode_chain("",active_table->key_head);
	return 1;

	//using flag_alg --end
	
/*	
	new_kv->next=active_table->key_head;
	active_table->key_head=new_kv;
	//put in --end
	
	active_table->kv_num++;
	active_table->kv_bytes+=key_size+value_size + 2; //2 '\0' characters 
	
	printf("%llu\r",put_counter);

	//printf("I am put, atable size=%d\n",active_table->size);

	
	return 0;
	*/
}