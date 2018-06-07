#include "myutil.h"
#include <sys/stat.h>
#include <time.h>

struct listnode* renew(void);
void rewrite(struct listnode*);

char *temp_file="../files/tmp/ipmerge.db";
char *ipdb="../ipshare.db";

int main(){
	char *ipdir="../files/hostels/";
	char *hostels[]={"kumaon", "kara", "ara", "jwala", "vindy",
		"kailash", "himadri", "nil", "shiva"};
	struct stat filebuf; // for stat
	struct listnode* iplist; // linked list
	char filenamelock[256],filenamedb[256];
	char input[256];
	FILE *in,*out;
	int ret,i;

	i=ret=0;
	// copy ipdb to temp file
	in=fopen(ipdb,"r");
	if(in==NULL) perror("Error ipmerge (open ipdb for copy) ");	
	out=fopen(temp_file,"w");
	if(out==NULL) perror("Error ipmerge (open tmp file for copy) ");
	
	while((ret=fgetc(in))!=EOF) fputc(ret,out); // char by char transfer
	
	fclose(in);
	// ipdb copied in temp file
	// FILE *out is still open
	
	for(i=0;i<9;i++){ // 9 hostels in all
			strcpy(filenamelock,ipdir);
			strcat(filenamelock,hostels[i]);
			strcpy(filenamedb,filenamelock);
			strcat(filenamelock,".lock");
			strcat(filenamedb,".db");
			ret=stat(filenamelock,&filebuf); // check for lock
			if(ret>=0) continue; // skip files which are locked
			// no lock .. so we will merge
			in=fopen(filenamedb,"r");
			if(in == NULL){
			       	fprintf(stderr,"Error ipmerge read %s:",filenamedb);
				perror("");
			}	

			while(fgets(input,256,in) != NULL){
				// skip those lines which don't have netbios
				if((char *)strstr(input,"netbios") == NULL) continue;
				// skip those lines which don't have open
				if((char *)strstr(input,"open")==NULL) continue;
				fputs(input,out);
			}
			fclose(in);
			in=fopen(filenamelock,"w"); // renew the lock
			if(in == NULL){
				fprintf(stderr,"Error ipmerge renew lock %s:",filenamelock);
				perror("");
			}
			fclose(in); // lock renewed
			// if lock is there, then next nmap can start
	}
	fclose(out); // all new ip's added to temp file
	iplist=renew(); // will read temp_file and return linked list of ip's
	rewrite(iplist); // will rewrite ip database (real merge)
	freelist(iplist);
	unlink(temp_file);
}

struct listnode* renew(){ // form a linked list
	struct listnode *iplist=NULL; // start pointer
	FILE *in;
	char input[256],ip[20],ltime[20];
	char *token;
	
	in=fopen(temp_file,"r");
	while(fgets(input,256,in) != NULL){
		if((char *)strstr(input,"netbios") != NULL) continue;
		// will first process old enteries (not having netbios)
		token=(char *)strtok(input," \n"); // time
		strcpy(ltime,token);
		token=(char *)strtok(NULL," \n"); // ip
		strcpy(ip,token);
		iplist=addtolist(iplist,ip,ltime); // insert key->value
		// we don't allow duplicates
	}
	if(fseek(in,0,0)!=0) perror("Error file seek failed ");
	// change current time to string
	sprintf(ltime,"%d",(int)time(NULL));
		
	while(fgets(input,256,in) != NULL){
		if((char *)strstr(input,"netbios") == NULL) continue;
		// will process new enteries (having netbios)
		token=(char *)strtok(input," \n"); // time
		token=(char *)strtok(NULL," \n"); // ip
		strcpy(ip,token); // save ip in var ip
		iplist=addtolist(iplist,ip,ltime); // update if already exists
	}
	fclose(in);
	return(iplist);
}

void rewrite(struct listnode* iplist){
	FILE *out;
	struct listnode* p=NULL;
	int ctime,ltime; // ltime -> last time
	// time for printing results	
	out=fopen(ipdb,"w");
	ctime=(int)time(NULL);
	if(out==NULL)
		perror("Error ipmerge (writing temp file) ");

	for(p=iplist;p!=NULL;p=p->next){
		ltime=(int)strtol(p->value,NULL,0);
		if((ctime - ltime) > 3600*24*14 )
		{
			printf("Deleting stale ip %s\n",p->key);
			fflush(stdout);
			continue;
		}
		fprintf(out,"%s %s\n",p->value,p->key);
	}
	fclose(out);
}
