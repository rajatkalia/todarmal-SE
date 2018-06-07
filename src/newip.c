#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	pid_t retfork; // ret value of fork
	FILE *fp;
	struct stat filebuf;
	char *hostels[]={"kumaon", "kara", "ara", "jwala", "vindy",
		"kailash", "nil", "shiva", "himadri"};
	// and now their corresponding ip's
	char *ipof[]={"10.144.", "10.156.", "10.152.", "10.148.", "10.140.",
		"10.164.", "10.160.", "10.136.", "10.172."};
	int i,j,retval;
	char dbfile[256],lockfile[256],command[256];
	char iprange[20],ipjunk[20];
	
	i=j=0;
	for(i=0;i<9;i++) // 9 hostels in all
	{
		retfork=fork();
		if(retfork==-1)
		{
			fprintf(stderr,"Error fork failed for %s ",hostels[i]);
			perror("");
		}
		if(retfork==0)
		{
			fflush(stdout);
			strcpy(dbfile,"../files/hostels/");
			strcat(dbfile,hostels[i]);
			strcpy(lockfile,dbfile);
			strcat(dbfile,".db"); // with nmap output
			strcat(lockfile,".lock"); // lock file
			// lock is ON when nmap is running
			fp=fopen(lockfile,"w");
			// set lock as nmap is starting
			if(fp==NULL)
			{
				fprintf(stderr,"Error Can't lock %s ",hostels[i]);
				perror("");
			}
			fclose(fp); // just touch the file
			for(j=0;j<256;)
			{
				retval=stat(lockfile,&filebuf);
				if(retval<0)
					sleep(10); // no lock
				else{
					strcpy(iprange,ipof[i]);
					sprintf(ipjunk,"%d",j);
					strcat(iprange,ipjunk);
					strcat(iprange,".*");

					retfork=fork();
					if(retfork==-1)
						perror("Error fork FAILED for nmap");
					if(retfork==0)
					{
						close(fileno(stdout));
						open("/dev/null",O_RDONLY);
						retval=execl("/usr/bin/nmap","nmap","-p","139","-P0","-n","--host_timeout","1500","-oG",dbfile,iprange,NULL);
						if(retval<0)
							perror("Error in nmap exec ");
						exit(0);
						// will process ever reach this
					}
					waitpid(retfork,NULL,0);
					// now remove lockfile and wait	
					strcpy(command,"rm ");
					strcat(command,lockfile);
					system(command);
					j++;
					j=j%256; // get back to start
				}
			} // end of for(j=0;j<256;);
			exit(0); // should never reach this (as j=j%256)
		} // end if(retfork==0);
	} // end of for(i=0;i<9;i++);
}
