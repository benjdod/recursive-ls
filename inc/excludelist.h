#ifndef _EXCLUDEBUFFER_H
#define _EXCLUDEBUFFER_H

#define DEFAULT_LIST_SIZE 4

struct excludelist {
	char **entries;
	unsigned int size;
	unsigned int cap;
};

struct excludelist *init_excludelist(struct excludelist *);
void addexclude(const char *, struct excludelist *);
void destroy_excludelist(struct excludelist *);

#endif
