#include "filenamebuffer.h"

void append(const char *s, struct filenamebuffer *fb) {
	if (fb->pos) {
		if (fb->seq[fb->pos] != '/') 
			fb->seq[fb->pos++] = '/';
	}
	fb->last = fb->pos;
	int i = 0;
	while (s[i] != '\0') {
		fb->seq[fb->pos++] = s[i++];
	}
	fb->seq[fb->pos] = '\0';
	return;
}

void removelast(struct filenamebuffer *fb) {
	while (fb->seq[fb->pos] != '/' && fb->pos) {;
		fb->pos--;
	}
	fb->seq[fb->pos] = '\0';
	return;
}

char *lastitem(char *space, unsigned int size, struct filenamebuffer *fb) {

	unsigned int i;
	unsigned int j;
	for (i = 0; i < fb->last; i++) {
		space[i] = ' ';
	}
	for (j = 0; i < fb->pos && i < size; i++) {
		space[i] = fb->seq[j++];
	}

	return space;
}
