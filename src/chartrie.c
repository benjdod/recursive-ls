#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "chartrie.h"

chartrie *makenode(char c) {
	chartrie *out = malloc(sizeof(chartrie));
	out->c = c;
	out->pos = 0;
	out->size = CHARTRIE_DEFAULT_SIZE;
	out->next = (chartrie*) malloc(CHARTRIE_DEFAULT_SIZE * sizeof(chartrie));
	out->next[0].c = '\0';
	out->next[0].next = NULL;
	return (chartrie*) out;
}

chartrie *gennode(char c, chartrie *loc) {
	loc->c = c;
	loc->pos = 0;
	loc->size = CHARTRIE_DEFAULT_SIZE;
	loc->next = (chartrie*) malloc(CHARTRIE_DEFAULT_SIZE * sizeof(chartrie));
	loc->next[0].c = '\0';
	loc->next[0].next = NULL;
	return loc;
}

void destroynode(chartrie *t) {
	if (!t->next) return;

	unsigned int i;
	for (i = 0; i < t->pos; i++) {
		destroynode(t->next + t->pos);
	}
	free(t->next);
	return;
}

chartrie *searchnode(char c, chartrie *t) {
	unsigned int i;
	for (i = 0; i < t->pos; i++) {
		if (t->next[i].c == c) return t->next + i;
	}
	return NULL;
}

chartrie *addtonode(char c, chartrie *t) {
	chartrie *p;
	if (p = searchnode(c,t)) return p;
	
	if (t->pos == t->size) {
		t->size *= 1.5;
		t->next = malloc(t->size * sizeof(chartrie*));
	}

	p = gennode(c, (t->next + t->pos));
	t->pos++;
	return p;
}

void addstring(const char *s, chartrie *head) {
	unsigned int i;

	for (i = 0; s[i] != '\0'; i++) {
		head = addtonode(s[i],head);
	}
}

void addstrictstring(const char *s, chartrie *head) {
	unsigned int i;
	char whtflg = 1;

	for (i = 0; s[i] != '\0'; i++) {
		if (whtflg) {
			if (isspace(s[i])) continue;
			if (s[i] == '#') return;
		}
		if (s[i] == '\n' || s[i] == '\r') continue;
		head = addtonode(s[i],head);
		whtflg = 0;
	}

	return;
}

unsigned int match(const char *s, chartrie *t) {
	chartrie *current = t;
	chartrie *temp;
	unsigned int i = 0;
	unsigned int count = 0;
	char c;

	while ((c = s[i]) != '\0') {
		//printf("searching for %c in %p\n",c,current);
		if (temp = searchnode(c,current)) {
			//printf("matched %c with %c at %p\n",c,temp->c, temp);
			current = temp;
			if (!current->next->c) {
				count++;
				//printf("match\n");
				current = t;
			} else {
				i++;
			}
		} else {
			if (current == t) i++;
			else current = t;
		}

	}
	//if (!current->next->c) count++;
	return count;
}

// TODO: functions for specialized matching
