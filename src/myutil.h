#include <stdio.h>
#include <stdlib.h>

struct listnode{
	struct listnode* next;
	char key[256];
       	char value[256];
	};

void listprint(struct listnode*);
struct listnode* addtolist(struct listnode*, char*, char*);
