#include <stdlib.h>
#include <string.h>
#include "excludelist.h"

struct excludelist *init_excludelist(struct excludelist *list) {
	// struct excludelist *out = malloc(sizeof(struct excludelist));
	list->entries = malloc(DEFAULT_LIST_SIZE *sizeof(char*));
	list->size = 0;
	list->cap = DEFAULT_LIST_SIZE;
	return  list;
}

void addexclude(const char *ex, struct excludelist *list) {
	char *entry = malloc((strlen(ex) + 1) * sizeof(char));
	strcpy(entry,ex);
	if (list->size + 1 == list->cap) {
		list->entries = (char**) realloc(list->entries, (list->cap * 1.5)*sizeof(char*));
	}
	list->entries[list->size] = entry;
	list->size++;
}

int matchexclude(const char *s, struct excludelist *list) {
	// TODO: implement a trie, maybe use a file in ~ to represent and 
	// check for changes between it and ~/.efind.excludes with mod times
	// but a naive implementation for now
	
	// given that this uses a bitfield for masking, we can only have up to
	// sizeof(unsigned int) excludes
}

void destroy_excludelist(struct excludelist *list) {
	unsigned int i;
	for (i = 0; i < list->size; i++) {
		free(list->entries[i]);
	}
	free(list->entries);
	free(list);
}
