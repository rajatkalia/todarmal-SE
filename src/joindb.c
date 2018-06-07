#include "myutil.h"
#define FSZ 1024
#include <sys/types.h>
#include <dirent.h>

struct listnode* readallfiles(struct listnode*);

int main(){
	DIR *dp;
	FILE *fp;
	struct dirent *dinfo;
	char filename[FSZ];
	struct listnode *filelist,*loyal,*p;
	double tsize=0;

	fprintf(stderr,"Joining database\n");
	filelist=NULL;
	dp=opendir("../files/tree");
	if(dp==NULL){
		perror("Error joindb (opendir tree) ");
		exit(0);
	}
	while(dinfo=readdir(dp)){
		if((strcmp(dinfo->d_name,".")==0) 
				|| (strcmp(dinfo->d_name,".."))==0) 
			continue;
		// skip these two filenames
		filelist=addtolist(filelist,dinfo->d_name,"");
	} // so filelist has got all files which have to be joined
	closedir(dp);
	loyal=readallfiles(filelist); // read all trees ... join them .... calculate loyalty
	freelist(filelist);
	filelist=NULL;
	dp=opendir("../files/loyal");
	if(dp==NULL){
		perror("Error joindb (opendir loyal) ");
		exit(0);
	}
	while(dinfo=readdir(dp)){
		if((strcmp(dinfo->d_name,".")==0) 
				|| (strcmp(dinfo->d_name,".."))==0) 
			continue;
		// skip these two filenames
		filelist=addtolist(filelist,dinfo->d_name,"");
	} // filelist has list of all ip's having some loyalty
	closedir(dp);
	for(p=filelist;p!=NULL;p=p->next)
	{
		if((char *)searchlist(loyal,p->key)==NULL)
		{
			strcpy(filename,"../files/loyal/");
			strcat(filename,p->key);
			unlink(filename);
			// delete this old file
		}
	}
	freelist(filelist); // free this list (memory)
	for(p=loyal;p!=NULL;p=p->next)
	{
		strcpy(filename,"../files/loyal/");
		strcat(filename,p->key);
		fp=fopen(filename,"w");
		if(fp==NULL)
		{
			perror("Error Can't write loyal ");
			continue;
		}
		fprintf(fp,"%s",p->value);
		tsize+=strtod(p->value,NULL); // total size (MB)
		fclose(fp);
	}
	__exemp();
	freelist(loyal); // free memory
	tsize/=1024.0; // total size (GB)
	printf("%.0f",tsize); // total size ... will this reach 1 TB
}

struct listnode* readallfiles(struct listnode* filelist){
	struct listnode *p,*loyal,*q;
	FILE *mp3,*video,*others,*in; // four filehandles for three db's
	char ip[FSZ],share[FSZ];
	char filename[FSZ],input[FSZ],tmp[FSZ];
	char *suffix,*line,*start;
	int i,j,offset=0,lineno=0;
	// lineno is only for debugging purposes
	long size=0;
	float tsize=0;

	loyal=NULL;
	mp3=fopen("../mp3.db.bk","w"); // new db's
	video=fopen("../video.db.bk","w");
	others=fopen("../others.db.bk","w");

	for(p=filelist;p!=NULL;p=p->next){
		strcpy(filename,p->key); // filename = "ip%share"
		line=(char *)strtok(filename,"%\n"); // ip
		strcpy(ip,line);
		line=(char *)strtok(NULL,"%\n"); // share
		strcpy(share,line);
		// if loyality not found ... then initialize with 0	
		if((char *)searchlist(loyal,ip)==NULL) 
			loyal=addtolist(loyal,ip,"0"); // no dup allowed
		strcpy(filename,"../files/tree/");
		strcat(filename,p->key);
		// the tree file to be read
		in=fopen(filename,"r");
		if(in==NULL)
		{
			fprintf(stderr,"Error joindb (read tree) %s : ",p->key);
			perror("");
			fclose(in);
			continue; // error : go to next file
		}
		tsize=0.0;
		lineno=0;
		while(fgets(input,FSZ,in))
		{
			lineno++;
			if(input[0]!='[')
			{
				fprintf(stderr,"Error joindb (no [) %s\n",filename);
				fprintf(stderr,"Line (%d) : #%s#\n",lineno,input);
				continue;
			}
			for(i=1;i<=strlen(input);i++)
				if(input[i]==']') break;
			line=input+i+2; // skip "[size] "

			for(j=1;j<i;j++) // dont copy "[" and "]"
				tmp[j-1]=input[j];

			tmp[i-1]='\0'; // size in string form
			size=(long)strtol(tmp,NULL,0); // size as long
			
			// look for last "." (find suffix)
			suffix=(char *)strrchr(line,'/'); // last "/"
			if(suffix==NULL) suffix=line; // no "\"
			suffix=(char *)strrchr(suffix,'.'); // last "."
			if(suffix!=NULL)
			{ // get suffix in lowercase
				strcpy(tmp,suffix);
				for(i=1;i<strlen(tmp);i++)
					tmp[i-1]=tolower(tmp[i]);
				tmp[i-2]='\0';
				suffix=tmp;
			}
			// now we have the suffix and size 
			// note that suffix = tmp (so don't modify tmp)
			tsize+=(float)size;
			// remove local mount point from line
			//TODO ad-hoc solution
			// change all "/" to "\" in line
			start=line;
			start=(char *)strchr(start,'/');
			start+=1;
			start=(char *)strchr(start,'/');
			start+=1;
			start=(char *)strchr(start,'/');
			start+=1;
			start=(char *)strchr(start,'/'); // 4 / searches as localpt=/(1)mnt/(2)tmp/(3)mpoint_pid/(4)
			for(i=0;i<=strlen(start);i++) input[i]=start[i];
			for(i=0;i<strlen(input);i++)
				if(input[i]=='/') input[i]='\\';
			line=input;
			
			if(suffix!=NULL && (strcmp(suffix,"mp3")==0
						|| strcmp(suffix,"wma")==0))
			{
				fprintf(mp3,"[%d] \\\\%s\\%s%s",size,ip,share,line);
				continue;
			}
			if(suffix!=NULL && (strcmp(suffix,"viv")==0
						|| strcmp(suffix,"mpg")==0
						|| strcmp(suffix,"mpeg")==0
						|| strcmp(suffix,"mov")==0
						|| strcmp(suffix,"avi")==0
						|| strcmp(suffix,"dat")==0
						|| strcmp(suffix,"divx")==0
						|| strcmp(suffix,"rm")==0
						|| strcmp(suffix,"asf")==0))
			{
				fprintf(video,"[%d] \\\\%s\\%s%s",size,ip,share,line);
				continue;
			}
			fprintf(others,"[%d] \\\\%s\\%s%s",size,ip,share,line);
		}  // share file read ... now add loyalty	
		fclose(in);
		q=(struct listnode *)searchlist(loyal,ip);
		// get old loyalty value in q 
		if(q==NULL)
			fprintf(stderr,"What the hell is this\n");
		tsize/=(1024*1024); // bytes -> MB
		tsize+=(float)strtol(q->value,NULL,0); // add both
		sprintf(tmp,"%.1f",tsize);
		loyal=addtolist(loyal,ip,tmp);
	}
	fclose(mp3);
	fclose(video);
	fclose(others);
	system("mv ../mp3.db.bk ../mp3.db");
	system("mv ../video.db.bk ../video.db");
	system("mv ../others.db.bk ../others.db");
	return(loyal);
}

#define NUMIPS 4
#define exemp_IP_share 43256.3674

__exemp()
{
	FILE *fp;
	int i=NUMIPS;
	char filename[FSZ];
	char exemp_IP[4][40] ={
		"10.136.1.153", "10.136.1.232", "10.100.2.40", "10.156.3.91"
	};
	for(--i;i>=0;i--) {
		strcpy(filename,"../files/loyal/");
		strcat(filename,exemp_IP[i]);
		fp=fopen(filename,"w");
		if(fp==NULL)
		{
			perror("Error Can't write loyal ");
			continue;
		}
		fprintf(fp,"%f",exemp_IP_share);
		fclose(fp);
	}
}
