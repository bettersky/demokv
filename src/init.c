#include <fcntl.h>    //provides O_RDONLY etc.
#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "flash.h"


int flash_init(void * args){
	char *device_name=(char *) args;  
	int fd=-1;
	fd=open(device_name,O_RDWR);
}