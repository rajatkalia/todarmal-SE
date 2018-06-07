#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
  	char mpoint[256],command[256],*name,*share,*ip;
	int check=-1;
	if(argc<4)
	{
		fprintf(stderr,"Incorrect number of parameters ");
		exit(1);
	}
	// 1 -> ip # 2 -> name # 3 -> share # 4 -> pid
	ip=argv[1];
	name=argv[2];
	share=argv[3];

	strcpy(mpoint,"/mnt/tmp/mpoint_");
	strcat(mpoint,argv[4]);
	strcpy(command,"smbmount //");
        strcat(command,"\"");
        strcat(command,name);
        strcat(command,"\"/\"");
        strcat(command,share);
        strcat(command,"\" ");
        strcat(command,mpoint); // local mount point
        strcat(command," -o ip=");
        strcat(command,ip);
        strcat(command,",guest,ro");

	check=setuid(0);
	if(check<0) perror("Setuid Error ");
   	check=system(command);
}
