#include "myutil.h"
#include <sys/stat.h>
#include <dirent.h>

void rem_spaces(char *str)
{
	int flag,i,j;
	// remove initial spaces
	for(flag=i=j=0;i<strlen(str);i++)
	{
		if(isspace(str[i]) && flag==0)
			continue;
		flag=1;
		str[j++]=str[i];
	}
	str[j]='\0';
	//remove trailing spaces
	for(i=strlen(str)-1;i>=0;i--)
	{
		if(isspace(str[i]))
			str[i]='\0';
		else break;
	}
}

int buildtree(char *share,char *name,char *ip, char *pid)
{
	char treefile[256],command[256],mpoint[256];
	int ret,success,pprotect;
	FILE *fp;
	
	success=pprotect=0;
	// pprotect = password protect
	strcpy(mpoint,"/mnt/tmp/mpoint_");
	strcat(mpoint,pid);
	strcpy(treefile,"../files/tmp/tree_");
	strcat(treefile,pid);
	ret=mkdir(mpoint,S_IRUSR | S_IWUSR | S_IXUSR);
	if(ret<0)
		perror("Error in creating mount pt ");
	strcpy(command,"/bin/tobumount ");
	strcat(command,ip);
	strcat(command," \"");
	strcat(command,name);
	strcat(command,"\" \"");
	strcat(command,share);
	strcat(command,"\" ");
	strcat(command,pid);
	fp=popen(command,"r"); // mount and see output of smbmount
	if(fp==NULL)
		perror("smbmount pipe error ");
	while(fgets(command,256,fp)!=NULL)
	{
		//if((char *)strstr(command,"ERRbadpw")!=NULL
		//		||(char *)strstr(command,"ERRnoaccess")!=NULL)
		if((char *)strstr(command,"ERRbadpw")!=NULL)
			pprotect=1; // has got some passwd
		else printf("SMBMNTOUT : %s",command);
	}
	//if(pprotect==1)
	//	printf("%s (%s) -> %s is passwd protected\n",ip,name,share);
	pclose(fp);
	fp=fopen("/proc/mounts","r");
	// read /proc/mounts to see if u have been able to mount the share
	if(fp==NULL)
		perror("Error Can't read /proc/mounts ");
	while(fgets(command,256,fp)!=NULL)
	{
		if((char *)strstr(command,pid)!=NULL)
			success=1; // so we have mounted that share
	}
	fclose(fp);
	if(success || pprotect)
	{
		if(success==1)
		{
			printf("Building tree %s (%s) -> %s\n",ip,name,share);
			fflush(stdout);
		}
		strcpy(command,"./tree ");
		strcat(command,mpoint); // local mount point
		strcat(command," > ");
		strcat(command,treefile);
		system(command); // build tree
		strcpy(command,"/bin/tobufree "); // a root setuid script
		strcat(command,pid);
		if(success==1) 
		{	
			system(command); // umount mpoint
		}
	}
	ret=rmdir(mpoint); // remove local mount pt
	if(ret<0)
		perror("Error in removing mount pt ");
	return(success || pprotect);
}

void writename(char *name,char *ip)
{
	/* takes name and ip and writes name to file "../files/names/ip" */
	FILE *fp;
	char filename[256];

	strcpy(filename,"../files/names/");
	strcat(filename,ip);
	fp=fopen(filename,"w");
	if(fp==NULL)
	{
		fprintf(stderr,"Error Can't write name %s for %s",name,ip);
		perror("");
		return;
	}
	fprintf(fp,"%s",name);
	fclose(fp);
}

int findname(char *name,char *ip)
{
	/* 
	 * uses system command nmblookup to find name
	 * nmblookup is provided by smbclient 
	 * for details go to http://www.samba.org
	 */
	FILE *pipe;
	char command[256],input[256],temp[5];
	int success=0,i;

	strcpy(command,"nmblookup -A ");
	// system command : nmblookup -A ip_address
	strcat(command,ip);
	pipe=popen(command,"r"); // run command and read output
	while(fgets(input,256,pipe)!=NULL)
	{
		if(((char *)strchr(input,'<')!=NULL) 
				&& ((char*)strchr(input,'>')!=NULL))
			success=1; // so comp was connected
		if((char *)strstr(input,"<20>")==NULL)
			continue; // skip lines not having <20>
		// process only lines with <20> in them
		// this line should have a name
		for(i=0;i<strlen(input);i++)
		{
			temp[0]=input[i];
			temp[1]=input[i+1];
			temp[2]=input[i+2];
			temp[3]=input[i+3];
			temp[4]='\0';
			if(strcmp(temp,"<20>")==0)
			{
				input[i]='\0';
				break;
			}
		}
		// input has still till <20> (excluding <20>)
		rem_spaces(input);
		strcpy(name,input); //we have name of comp now
	}
	pclose(pipe);
	if(success==1)
		writename(name,ip);
	return(success);
}

void writeshares(struct listnode *sharelist,char *ip)
{
	/*
	 * if comp is connected ... 
	 * then shares are writtern in ../files/shares/ip
	 * and tree of shares is built in next stage
	 * this stage also deletes the tree files which correspond to ...
	 * shared folders which are no longer shared
	 */
	
	FILE *fp;
	DIR *dp;
	struct dirent *dinfo;
	char filename[256],*fileip,*fileshare,command[256];
	struct listnode *p,*filelist;

	filelist=p=NULL;
	
	strcpy(filename,"../files/shares/");
	strcat(filename,ip);
	fp=fopen(filename,"w");
	// write list of shares ...
	if(fp==NULL)
	{
		fprintf(stderr,"Error Can't write shares for %s",ip);
		perror("");
		return;
	}
	// null list implies empty shares file
	// as file was opened with MODE "w"
	for(p=sharelist;p!=NULL;p=p->next)
		fprintf(fp,"%s\n",p->key);
	fclose(fp);
	dp=opendir("../files/tree");
	if(dp==NULL)
		perror("Error Can't open dir (sync share list) ");
	while(dinfo=readdir(dp))
	{
		if(strcmp(dinfo->d_name,".")==0
				|| strcmp(dinfo->d_name,"..")==0)
			continue; // skip these two files
		filelist=addtolist(filelist,dinfo->d_name,"");
	}
	closedir(dp);
	for(p=filelist;p!=NULL;p=p->next)
	{
		strcpy(filename,p->key);
		// filename has format ip_address%sharename
		fileip=(char *)strtok(filename,"%\n");
		if(strcmp(fileip,ip)!=0) continue;
		fileshare=(char *)strtok(NULL,"%\n");
		if((char *)searchlist(sharelist,fileshare)==NULL)
		{
			printf("Removing %s : no longer shared\n",p->key);
			strcpy(command,"rm \"");
			strcat(command,"../files/tree/");
			strcat(command,p->key);
			strcat(command,"\"");
			system(command);
		}
	}
	freelist(filelist); // regain memory
}

struct listnode *findshares(char *name,char *ip)
{
	struct listnode *sharelist=NULL;
	char command[256],input[256];
	FILE *pipe;
	int success=0;
	
	strcpy(command,"smbclient -L \"");
	strcat(command,name);
	strcat(command,"\" -I ");
	strcat(command,ip);
	strcat(command," -N");

	pipe=popen(command,"r"); // command used as junk space below
	while(fgets(input,256,pipe)!=NULL)
	{
		// add some smart error parsing here
		if((char *)strstr(input,"Domain=")!=NULL
				&& (char *)strstr(input,"OS=")!=NULL
				&& (char *)strstr(input,"Server=")!=NULL)
			success=1;
		if((char *)strstr(input,"Sharename")!=NULL
				&& (char *)strstr(input,"Type")!=NULL
				&& (char *)strstr(input,"Comment")!=NULL)
			success=1;
		strcpy(command,ip);
		strcat(command,":139");
		if((char *)strstr(input,command)!=NULL
				&& (char *)strstr(input,"(Connection refused")!=NULL)
			success=1; // so connection was refused
		if((char *)strstr(input,"Called name not present")!=NULL)
		{
			// this happens when name of the system has changed
			strcpy(command,"rm ../files/names/");
			strcat(command,ip);
			system(command);
			// name of comp removed 
			// next time i will find the new name
			// not doing it this time to avoidm race condition
			success=1;
		}
		if((char *)strstr(input,"Disk")==NULL) continue;
		//-> assumption that smbclient output will have Disk at 16th char
		//-> using smbclient version 2.2.1a
		if(input[16]!='D' || input[17]!='i' 
				|| input[18]!='s' || input[19]!='k')
		{
			fprintf(stderr,"Error : \"Disk\" in %s\n",input);
			continue;
		}
		input[16]='\0'; //+++ a very chutia assumption
		rem_spaces(input);
		sharelist=addtolist(sharelist,input,"");
	}
	pclose(pipe);
	//if(success==1)
		writeshares(sharelist,ip);
	return(sharelist);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	char ip[20],filename[256],name[20],sharename[256];
	char pidjunk[20],command[256];
	struct stat filebuf;
	int ret,i;
	time_t ctime;
	struct listnode *sharelist=NULL;
	struct listnode *p=NULL;

	if(argc!=3)
	{
		fprintf(stderr,"Usage : driver ip_address sleep_time(min)\n");
		exit(1);
	}
	ret=0;
	ret=(int)strtol(argv[2],NULL,0);
	if(ret>0)
		sleep(ret*60);
	// check if host is up
	strcpy(ip,argv[1]);
	strcpy(command,"ping -c 1 -w 2 -n ");
	strcat(command,ip);
	strcat(command," 2> /dev/null");
	fp=popen(command,"r");
	while(fgets(filename,256,fp)!=NULL)
	{
		if((char *)strstr(filename,"icmp_seq")!=NULL)
			ret=1;
	}
	if(ret==0) exit(0); // host is down

	if(strcmp(ip,"10.136.1.211")==0) exit(1);
	time(&ctime); // current time
	
	strcpy(filename,"../files/names/");
	strcat(filename,ip);
	ret=stat(filename,&filebuf);
	strcpy(name,"");
	if((ret>=0) && (ctime - filebuf.st_mtime < 3600*12) 
			&& (strcmp(argv[2],"-1")!=0)){ 
		// file for name is there and is not so old
		fp=fopen(filename,"r");
		if(fp==NULL)
		{
			perror("Error in reading old name ");
			exit(0);
		}
		fgets(name,20,fp);
		rem_spaces(name);
		fclose(fp);
	}
	else
	{
		printf("Finding name of %s\n",ip);
		fflush(stdout);
		findname(name,ip); // returns 1 on success
	}

	if(strcmp(name,"")==0) 
	{
		writeshares(NULL,ip);
		exit(1); // comp has no name
	}
	
	// now that we have got name of comp
	// let's find the shares of that comp
	strcpy(filename,"../files/shares/");
	strcat(filename,ip);
	ret=stat(filename,&filebuf);
	time(&ctime);
	if((ret>=0) && (ctime-filebuf.st_mtime < 3600*4) 
			&& (strcmp(argv[2],"-1")!=0))
	{
		// we have info about shares
		fp=fopen(filename,"r");
		if(fp==NULL)
		{
			fprintf(stderr,"Error in reading old shares of %s",ip);
			perror("");
			exit(1);
		}
		while(fgets(sharename,256,fp)!=NULL)
		{
			rem_spaces(sharename);
			if(strcmp(sharename,"")==0) continue;
			// now sharename has no traling spaces
			sharelist=addtolist(sharelist,sharename,"");
		}
		fclose(fp);
	}
	else
	{
		printf("Finding share list of %s (%s) \n",ip,name);
		sharelist=findshares(name,ip);
	}
	if(sharelist==NULL) exit(0); // sharelist is NULL

	// cool ... we have the share list .. time to build tree.
	for(p=sharelist;p!=NULL;p=p->next)
	{
		strcpy(filename,"../files/tree/");
		strcat(filename,ip);
		strcat(filename,"%");
		strcat(filename,p->key);
		ret=stat(filename,&filebuf);
		time(&ctime);
		if((ret>=0) && (ctime-filebuf.st_mtime < 3600*6) 
				&& (strcmp(argv[2],"-1")!=0))
			continue; // skip this share 
		printf("Attempt to mount %s (%s) -> %s\n",ip,name,p->key);
		sprintf(pidjunk,"%d",(int)getpid());
		ret=buildtree(p->key,name,ip,pidjunk);
		if(ret==0)
			continue; // comp was not connected
		strcpy(filename,"../files/tmp/tree_");
		strcat(filename,pidjunk);
		
		strcpy(command,"mv ");
		strcat(command,filename); // tree file
		strcat(command," \"");
		strcpy(filename,"../files/tree/");
		strcat(filename,ip);
		strcat(filename,"%");
		strcat(filename,p->key);
		strcat(command,filename);
		strcat(command,"\"");
		system(command);
	}	
	freelist(sharelist);
}
