#include "myutil.h"
#include <dirent.h>
#include <sys/stat.h>

void tree(char *);

int main(int argc, char *argv[])
{
	if(argc!=2)
	{
		fprintf(stderr,"Wrong number of arguments\n");
		exit(0);
	}
	tree(argv[1]);
}

void tree(char *dir)
{
	char dirname[256],junkname[256],suffix[20];
	char *suffixptr;
	struct dirent *dinfo;
	struct stat filebuf;
	struct listnode *filelist=NULL;
	struct listnode *dirlist=NULL;
	struct listnode *p=NULL;
	int ret=0,count=0;
	DIR *dp;
		
	count=0;
	strcpy(junkname,dir);
	strcat(junkname,"/NETSTAT.EXE");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/TEMP");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/Temporary Internet Files");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/SYSTEM");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/HIMEM.SYS");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/Desktop");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/Local Settings");
	if(stat(junkname,&filebuf)>=0) count++;
	

	//printf("Checking dir %s\n",dir);	
	//printf("first count is %d\n",count);	
	if(count>=3)
	{
		// this comp is sharing windows folder on windows 98	
		strcpy(junkname,dir);
		strcat(junkname,"/Desktop");
		if(stat(junkname,&filebuf)>=0){
			tree(junkname);
		}
		return;
	}
	
	count=0;
	strcpy(junkname,dir);
	strcat(junkname,"/Messenger");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/Internet Explorer");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/KaZaA");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/Common Files");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/Accessories");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/Uninstall Information");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/Yahoo!");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/NetMeeting");
	if(stat(junkname,&filebuf)>=0) count++;
	strcpy(junkname,dir);
	strcat(junkname,"/Plus!");
	if(stat(junkname,&filebuf)>=0) count++;


	//printf("Sec count is %d\n",count);
	if(count>=4)
	{
		// this comp is sharing program files folder on windows 98
		return;
	}

	dp=opendir(dir);
	if(dp==NULL) return;
	while(dinfo=readdir(dp))
	{
		if(strcmp(dinfo->d_name,".")==0
				|| strcmp(dinfo->d_name,"..")==0)
			continue;
		filelist=addtolist(filelist,dinfo->d_name,"");
	}
	closedir(dp);
	
	for(p=filelist;p!=NULL;p=p->next)
	{
		strcpy(dirname,dir);
		strcat(dirname,"/");
		strcat(dirname,p->key);
		ret=lstat(dirname,&filebuf); // dirname -> filename
		if(ret<0) continue;
		if(S_ISREG(filebuf.st_mode)){
			if((int)filebuf.st_size < 4096)
				continue;
			// skip files less than 4k in size
			suffixptr=strrchr(dirname,'.');
			if(suffixptr!=NULL)
			{
				suffixptr+=1;
				strcpy(suffix,suffixptr);
				for(ret=0;ret<strlen(suffixptr);ret++) suffix[ret]=tolower(suffix[ret]);
			}

			if((int)filebuf.st_size < 1024*1024)
			{
				// look for last "." in filename
				if(suffixptr==NULL)
					continue;
				// no suffix and less than 1 MB ... drop it
				if(strcmp(suffix,"zip")!=0
					&& strcmp(suffix,"exe")!=0
					&& strcmp(suffix,"doc")!=0
					&& strcmp(suffix,"ppt")!=0
					&& strcmp(suffix,"xls")!=0
					&& strcmp(suffix,"bmp")!=0
					&& strcmp(suffix,"jpg")!=0
					&& strcmp(suffix,"pdf")!=0
					&& strcmp(suffix,"ps")!=0
					&& strcmp(suffix,"z")!=0)
				{	
					// size less than 1 MB
					// suffix not one of these
					// so drop it
					continue;
				}
			}
			if((suffixptr!=NULL) && (strcmp(suffix,"tmp")==0
				|| strcmp(suffix,"psd")==0
				|| strcmp(suffix,"temp")==0
				|| strcmp(suffix,"sys")==0
				|| strcmp(suffix,"swp")==0
				|| strcmp(suffix,"cab")==0
				|| strcmp(suffix,"pak")==0
				|| strcmp(suffix,"pk3")==0
				|| strcmp(suffix,"drs")==0
				|| strcmp(suffix,"jar")==0
				|| strcmp(suffix,"wav")==0
				|| strcmp(suffix,"dll")==0
				|| strcmp(suffix,"eml")==0))
			{
				continue;
			}
			printf("\[%d] %s\n",(int)filebuf.st_size,dirname);
		}
		else if(S_ISDIR(filebuf.st_mode))
		{
			dirlist=addtolist(dirlist,dirname,"");
		}
	}
	freelist(filelist);
	for(p=dirlist;p!=NULL;p=p->next)
		tree(p->key);
	freelist(dirlist);
}
