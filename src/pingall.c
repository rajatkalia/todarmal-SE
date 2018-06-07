#include "myutil.h"
#include <dirent.h>
#include <sys/stat.h>

int main()
{
	char filename[256],*token,command[256];
	FILE *fp;
	struct listnode *iplist,*p,*uplist;
	DIR *dp;
	struct stat filebuf;
	struct dirent *dinfo;
	int ret,up;
	
	dp=opendir("../files/tree");
	if(dp==NULL)
		perror("Error pingall (opendir tree) ");
	iplist=NULL;
	while(dinfo=readdir(dp))
	{
		if(strcmp(dinfo->d_name,".")==0 
				|| strcmp(dinfo->d_name,"..")==0)
			continue; // skip them
		strcpy(filename,"../files/tree/");
		strcat(filename,dinfo->d_name);
		ret=stat(filename,&filebuf);
		if(ret<0) continue;
		if(filebuf.st_size==0) continue;
		token=(char *)strtok(dinfo->d_name,"%\n");
		// token has got ip address
		iplist=addtolist(iplist,token,"");
	}
	//printf("All ip's read\n");
	closedir(dp);
	uplist=NULL;
	for(p=iplist;p!=NULL;p=p->next)
	{
		up=0;
		strcpy(command,"ping -c 1 -w 2 -n ");
		strcat(command,p->key);
		strcat(command," 2>/dev/null");
		//printf("pinging %s\n",p->key);
		fp=popen(command,"r");
		while(fgets(filename,256,fp)!=NULL)
		{
			if((char *)strstr(filename,"icmp_seq")!=NULL)
				up=1;
		}
		pclose(fp);
		strcpy(filename,"../files/ping/");
		strcat(filename,p->key);
		if(up==1)
		{
			fp=fopen(filename,"w");
			if(fp==NULL) perror("Error adding cameup ip ");
			fclose(fp);
			uplist=addtolist(uplist,p->key,"");
		}
		else
		{
			ret=stat(filename,&filebuf);
			if(ret>=0)
			{
				ret=unlink(filename);
				if(ret<0)
					perror("Error Deleting gonedown ip ");
			}
		}
	}
	//printf("Up list formed\n");
	freelist(iplist);
	iplist=NULL;
	dp=opendir("../files/ping");
	if(dp==NULL)
		perror("Error Opening ping dir ");
	while(dinfo=readdir(dp))
	{
		if(strcmp(dinfo->d_name,".")==0
				|| strcmp(dinfo->d_name,"..")==0)
			continue;
		if((struct listnode*)searchlist(uplist,dinfo->d_name)==NULL)
		{
			strcpy(filename,"../files/ping/");
			strcat(filename,dinfo->d_name);
			ret=unlink(filename);
			if(ret<0)
				perror("Error deleting gonedown ip ");
		}
	}
	//printf("Stale ip's deleted\n");
	freelist(uplist);
}
