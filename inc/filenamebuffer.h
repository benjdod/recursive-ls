#ifndef _FILENAMEBUFFER_H
#define _FILENAMEBUFFER_H

struct filenamebuffer {
	char *seq;
	unsigned int pos;
	unsigned int last;
};

void append(const char *, struct filenamebuffer *);
void removelast(struct filenamebuffer *);

#endif
