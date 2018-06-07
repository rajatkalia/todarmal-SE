#include <stdio.h>
#include <stdlib.h>
#include "myutil.h"

struct listnode *searchlist(struct listnode *,char *);

void listprint(struct listnode *start){
	struct listnode *p=NULL;
	printf("--------------\n");
	for(p=start;p!=NULL;p=p->next)
		printf("%p#%s#%s#\n",p,p->key,p->value);
	printf("--------------\n");
}

struct listnode *addtolist(struct listnode *start, char* key, char* value){
	struct listnode *p=NULL;
	struct listnode *q=malloc(sizeof(struct listnode));
	if(q==NULL)
		perror("Error Malloc failed in addtolist ");
	strcpy(q->key,key);
	strcpy(q->value,value);
	q->next=NULL;
	if(start==NULL)
	       	return(q); // list was empty .. so we created it
	p=searchlist(start,key);
	if(p!=NULL)
	{
		strcpy(p->value,value); // update the value
		free(q); // free unused space
		return(start);
	}
	for(p=start;p->next!=NULL;p=p->next) ;
	p->next=q; // add new element to the end of list
	return(start);
}

struct listnode *searchlist(struct listnode *start, char* key){
	struct listnode *p=NULL;
	if(start==NULL)
	       	return(NULL); // list was empty
	for(p=start;p!=NULL;p=p->next){
		if(strcmp(p->key,key)==0){ // found a match
		       	return(p);
		}
	}
	return(NULL); // not found
}

struct listnode *deletefromlist(struct listnode *start, char *key)
{
	// not tested code ... caution
	struct listnode *p,*q;
	p=q=NULL;
	p=searchlist(start,key);
	if(p==NULL)
	{
		fprintf(stderr,"Error : %s not found in list\n",key);
		return(start);
	}
	if(p==start)
	{
		q=p->next;
		free(p);
		return(q);
	}
	for(p=start;p->next!=NULL;)
		if(strcmp(p->next->key,key)==0) break;
	if(strcmp(p->next->key,key)!=0)
	{
		printf("What the hell is this\n");
		listprint(start);
		printf("was looking for %s",key);
	}
	q=p->next;
	p->next=p->next->next;
	free(q);
	return(start);
}

void freelist(struct listnode *start) // reclaim memory used by list
{
	struct listnode *p,*q;
	p=q=NULL;
	for(p=start;p!=NULL;)
	{
		q=p->next;
		free(p);
		p=q;
	}
}
