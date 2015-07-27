#include "segtable.h"
#include "flash.h" //for device

extern struct DEVICE device;//defined in flash.h
extern struct ATABLE *active_table;//defined in egtable.h
int put(char* key,char* value){
	//printf("I am put\n");
	int key_size=strlen(key);
	int value_size=strlen(value);
	//printf("key=%s, key size=%d,value=%s,value size=%d\n",key,key_size,value, value_size);
	//decide whether need to merge --begin
	if(active_table->size+key_size+value_size +2 > device.segment_bytes){//merge to lev0
		//merge to lev0 code --begin
		merge();
		//merge to lev0 code --end
		//put(key,value);//after merging, call put() again to finish putting
	}
	//decide whether need to merge --end
	//put in --begin
	struct KNODE *new_kv=malloc(sizeof(struct KNODE));
	new_kv->key=key;
	new_kv->value=value;
	
	new_kv->next=active_table->key_head;
	active_table->key_head=new_kv;
	
	//put in --end
	
	active_table->kv_num++;
	active_table->size+=key_size+value_size + 2; //2 '\0' characters 
	//printf("I am put, atable size=%d\n",active_table->size);

}