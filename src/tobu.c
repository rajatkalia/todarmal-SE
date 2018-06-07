#include "myutil.h"
#include <dirent.h>

void rundriver()
{
	char input[256],command[256];
	FILE *fp;
	DIR *dp;
	struct dirent *dinfo;
	char *token;
	struct listnode *iplist=NULL;
	struct listnode *p=NULL;

	for(;;)
	{
		iplist=NULL;
		fp=fopen("../ipshare.db","r");
		if(fp==NULL)
		{
			fprintf(stderr,"Error tobu Can't read ipshare.db ");
			perror("");
			sleep(30);
			continue;
		}
		while(fgets(input,256,fp)!=NULL)
		{
			token=(char *)strtok(input," \n"); //time
			token=(char *)strtok(NULL," \n"); //ip
			iplist=addtolist(iplist,token,"");
		}
		fclose(fp);
		dp=opendir("/home/tobuset");
		if(dp==NULL)
			perror("Error opening /home/tobusetup : ");
		while(dinfo=readdir(dp))
		{
			if(strcmp(dinfo->d_name,".")==0
					|| strcmp(dinfo->d_name,".")==0)
				continue; // skip these two files
			strcpy(input,dinfo->d_name);
			//if(input[0]!='1' || input[1]!='0' || input[2]!='.')
			if(input[0]=='.')
				continue; // skip hidden files
			token=(char *)strtok(input,"_\n"); // ip
			if(token==NULL) continue; // how come no "_"
			iplist=addtolist(iplist,token,"");
		}
		closedir(dp);
		for(p=iplist;p!=NULL;p=p->next)
		{
			strcpy(command,"./driver ");
			strcat(command,p->key);
			strcat(command," 0");
			system(command);
			sleep(1);
		}
		freelist(iplist);		
	}
}

int main()
{
	pid_t retfork;
	int ret;
	FILE *fp;
	char *junk;
// ----------
	fp=popen("ps aux | grep newip | grep tobu | wc -l","r");
	if(fp==NULL)
	{
		perror("Error in wc ");
		exit(0);
	}
	junk=malloc(256*sizeof(char));
	fgets(junk,256,fp);
	ret=(int)strtol(junk,NULL,0);
	close(fp);
	free(junk);
	if(ret>2)
		exit(0);
// ---------
	retfork=fork();
	if(retfork==-1)
		perror("Error tobu Can't fork ");
	if(retfork==0)
	{
		ret=execl("./newip",NULL);
		if(ret<0)
		{
			perror("Error newip call failed ");
			exit(0);
		}
	}
	retfork=fork();
	if(retfork==-1)
		perror("Error tobu Can't fork ");
	if(retfork==0)
	{
		for(;;)
		{
			system("./joindb > ../files/total");
			sleep(60*20);
		}
	}
	retfork=fork();
	if(retfork==-1)
		perror("Error tobu Can't fork ");
	if(retfork==0)
	{
		for(;;)
		{
			system("./ipmerge");
			sleep(30);
		}
	}
	rundriver();
}
