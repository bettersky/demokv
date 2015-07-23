

int flash_open(); //if the device is fit to flashkv, then call this function to obtain internal information and 
					//store them in some structures

int flash_formate();//this function use the device_info the formate the  device

int flash_init(void *);//this function obtain disk's external information and store it in a structure named device_info 