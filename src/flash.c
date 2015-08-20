#include "flash.h"
#include "segtable.h"
extern struct DEVICE device;
extern char *levels_summary;



extern uint64_t serials_base[]; //all levels' base serial number. 0 is reserved
extern uint64_t serials_width[];// all levels' serial width. decided by the LEV)_NUM and LEV_PUFFER
extern char *seg_bit_maps[];

char *read_seg(uint64_t serial_num){
	//should mapped to physic segment
	
	uint64_t phy_seg=ftl_addr_map(serial_num, "read");
	
	//printf("I am read_seg, serial_num=%d,phy_seg=%d, key=%s \n",serial_num,phy_seg,device.mmap_begin + phy_seg*device.segment_bytes);
	
	//print_table("in read_seg", device.mmap_begin + phy_seg*device.segment_bytes);
	
	//printf("in read_seg,device.mmap_begin + phy_seg*device.segment_bytes=%p\n ",device.mmap_begin + phy_seg*device.segment_bytes);
	//exit(1);
	return device.mmap_begin + phy_seg*device.segment_bytes;

}

int discard_seg(uint64_t serial){


}

uint64_t *allocate_serial(int lev, int num){
	uint64_t * serials=(uint64_t*)malloc(sizeof(uint64_t)*num);
	uint64_t i;
	uint64_t j=0;
	//printf("in allocate_serial, lev=%d, serials_width[lev]=%d, num=%d, ( *(int*) ( levels_summary+ lev*LEVELS_SUMMARY_ENTRY) )=%d\n",
			//lev,serials_width[lev],num,( *(int*) ( levels_summary+ lev*LEVELS_SUMMARY_ENTRY) ));
	//print_bit_map("before allocate_serial,  bit map",seg_bit_maps[lev],serials_width[lev]);
	//printf("in allocate_serial, 222222222\n");
	for(i=0;i<serials_width[lev];i++){
		
		if(seg_bit_maps[lev][i]==0){//blank and can be allocated
			serials[j++]=serials_base[lev]+i;
			seg_bit_maps[lev][i]=1;//will be used 
			
			if(j==num) break;
		}	
	}
	//print_bit_map("after allocate_serial,  bit map",seg_bit_maps[lev],serials_width[lev]);

	if(j!=num){
		printf("error in allocate_serial. exit. j=%d,num=%d,lev=%d, serials_width[lev]=%d\n",j,num, lev,serials_width[lev]);
		exit(2);
	}
	return serials;
}


int write_seg(char *seg_buffer, uint64_t serial_num){
	
	//should mapped to physic segment
	//printf("i am write_seg,device.mmap_begin =%p ,serial_num=%d,seg_buffer=%s\n",device.mmap_begin,serial_num ,seg_buffer);
	uint64_t phy_seg=ftl_addr_map(serial_num, "write");
	
	memcpy(device.mmap_begin + phy_seg*device.segment_bytes, seg_buffer, device.segment_bytes);
	

}



uint64_t ftl_addr_map(uint64_t serial_num, char *arg){
	uint64_t phy_seg=serial_num;	
	
	if(phy_seg>device.segment_total-10){//reserved
		printf("error in ftl_addr_map, phy_seg>device.segment_total, exit\n");
		exit(1);
	}
	
	return phy_seg;
}

int set_bit_map(int lev, uint64_t serial_num){
	if(serial_num<serials_base[lev] || serial_num>serials_base[lev]+serials_width[lev]){
		printf("error, set_bit_map, serial_num out scop, exit\n");
		exit(3);
	}
	seg_bit_maps[lev][serial_num-serials_base[lev]] =1;
}

int clear_bit_map(int lev, uint64_t serial_num){
	if(serial_num<serials_base[lev] || serial_num>serials_base[lev]+serials_width[lev]){
		printf("error, clear_bit_map, serial_num out scop, exit\n");
		exit(3);
	}
	seg_bit_maps[lev][serial_num-serials_base[lev]] =0;
}