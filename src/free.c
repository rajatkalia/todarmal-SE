#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
  	char mpoint[256];
	int check=-1;

	strcpy(mpoint,"/mnt/tmp/mpoint_");
	strcat(mpoint,argv[1]);
	check=setuid(0);
	if(check<0) perror("Setuid Error ");
   	check=execl("/bin/umount","umount",mpoint,NULL);
    	if(check<0) perror("Umount Error ");
}
