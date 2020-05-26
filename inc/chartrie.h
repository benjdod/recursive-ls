#ifndef _CHARTRIE_H
#define _CHARTRIE_H

#define CHARTRIE_DEFAULT_SIZE 4

typedef struct chartrie chartrie;

struct chartrie {
	char c;
	unsigned int pos;
	unsigned int size;
	chartrie *next;
};

// function:	creates a new chartrie with the char field set to c
// returns: 	a pointer to a new chartrie with the first pointer in the 'next' field set to NULL;
chartrie *makenode(char c);

// function:	initializes a new chartrie node in the space specified by the pointer *loc
// returns: 	a pointer to a new chartrie with the first pointer in the 'next' field set to NULL;
chartrie *gennode(char c, chartrie *loc);

// function:	destroys the tree rooted at *t
// returns:		nothing
void destroynode(chartrie *t);

// function:	searches the next array of t for char c
// returns: 	index of the next node corresponding to the provided char
// 				-1 if not found
chartrie *searchnode(char c, chartrie *t);

// function: 	adds char c to the next array of t 
// returns: 	index of the next node corresponding to the provided char
// 				duplicate values will not be added
chartrie *addtonode(char c, chartrie *t);

// function:	adds a string to the chartrie starting below t
// 				so for example, if one wanted to add "beans" to the node '.',
// 				they would get ".beans" as the resulting sequence
// returns: 	nothing
// note:		*t must be initialized!
void addstring(const char *, chartrie *);
//
// function:	adds a string to the chartrie starting below t, ignoring \n and \r
// 				so for example, if one wanted to add "beans" to the node '.',
// 				they would get ".beans" as the resulting sequence
// returns: 	nothing
// note:		*t must be initialized!
void addstrictstring(const char *, chartrie *);

unsigned int match(const char *s, chartrie *t);

#endif
