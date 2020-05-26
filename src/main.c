#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>
#include <errno.h>
#include "filenamebuffer.h"
#include "chartrie.h"

#define MAX_DEPTH 20
#define HIDE_HIDDEN 0x01
#define USE_EXCLUDES 0x02
#define CLEAN_PRINT 0x04
#define ESC_COLOR 0x08

#define TERM_BRIGHTRED_COLOR() printf("\033[1;31m");
#define TERM_CYAN_COLOR() printf("\033[36m");
#define TERM_RESET_COLOR() printf("\033[0m");

/* flag bits
 * 0: hides hidden files
 * 1: enables file/folder exclusion
 * */

unsigned int flag;
unsigned int depth;
struct filenamebuffer fb;
chartrie *trie;
char dirpath[PATH_MAX];

// forward declaring listdir
int listdir(const char *);

void printerror(const char *format, ...) {
	va_list args;
	if (flag & ESC_COLOR) TERM_BRIGHTRED_COLOR();
	printf("Error: ");
	vprintf(format, args);
	putchar('\n');
	if (flag & ESC_COLOR) TERM_RESET_COLOR();
}

void printwarning(const char *format, ...) {
	va_list args;
	if (flag & ESC_COLOR) TERM_CYAN_COLOR();
	printf("Warning: ");
	vprintf(format, args);
	putchar('\n');
	if (flag & ESC_COLOR) TERM_RESET_COLOR();
}

void printprompt() {
	printf("Enter a directory.\n");
}

void printusage() {
	printf("efind [options] directory\n");
}

void printversion() {
	printf("efind (v. 0.1.0)\n");
	printf("a new and improved find utility...\n");
}

void printinvalidoption(const char *badopt) {
	printf("Invalid option: \"%s\"",badopt);
}

// returns: negative values for errors, positive values for no ops, 0 for continue
int processargs(int argc, char **argv) {

	// initial conditions
	flag = 0;
	flag = HIDE_HIDDEN | USE_EXCLUDES | CLEAN_PRINT;

	if (argc < 2) {
		printprompt();
	 	return -1;
	}
	if (argv[1][0] == '\0') {
		printprompt();
		return -1;
	}

	unsigned int i;
	unsigned int j;
	const char *arg;
	
	for (i = 1; i < argc; i++) {
		arg = argv[i];

		if (arg[0] == '-') {
			j = 1;
			while (arg[j] != '\0') {
				switch (arg[j]) {
					case 'h':
						printusage();
						return 1;
					case 'v':
						printversion();
						return 2;
					case 'a':
						// show hidden files
						flag &= ~HIDE_HIDDEN;
						break;
					case 'A':
						// show hidden files and ignore excludes
						flag &= ~HIDE_HIDDEN;
						flag &= ~USE_EXCLUDES;
						break;
					case 'f':
						flag &= ~CLEAN_PRINT;
						break;
					default:
						printinvalidoption(arg);
						return -1;
				}
				j++;
			}
		} else {
			strcpy(dirpath,arg);
		}
	}

	return 0;

}

void init() {
	depth = 0;
	fb.seq = malloc((PATH_MAX + 1) * sizeof(char));
	fb.pos = 0;

	// sets up our excludes trie if we need it
	if (flag & USE_EXCLUDES) {
		trie = makenode('\0');
		char buf[PATH_MAX + 1];
		char fpbuf[PATH_MAX + 1];

		// putting the path together for the excludes file
		strcpy(fpbuf,getenv("HOME"));
		short i = 0;
		while (fpbuf[i] != '\0') i++;
		fpbuf[i++] = '/';
		fpbuf[i] = '\0';
		strcat(fpbuf+i, ".efind.excludes");

		// TODO: move this opening stuff to a new function somewhere else
		FILE *excludesfp = fopen(fpbuf,"r");
		if (excludesfp) {
			errno = 0;
			while (fgets(buf,PATH_MAX,excludesfp)) {
				addstrictstring(buf,trie);
			}

		//	printf("added strings");
			if (errno) {
				// fgetc error checking
			}

			fclose(excludesfp);
		} else {
			// TODO: finish warnings here...
			printwarning("excludes file could not be located, proceeding without.\nEnsure that there is a .efind.excludes file in your home directory");
		}
	}

	// handles coloring flag
	if (flag & ESC_COLOR) {
		const char *term = getenv("TERM");
		if (strstr(term,"xterm-color") || strstr(term,"-256color")) {
			; //we're cool
		} else {
			flag &= ~ESC_COLOR;
		}
	}

	return;
}

void endit() {
	free(fb.seq);
}

int listdir(const char *name) {

	//printf("attempting to access directory %s\n",name);

	if (depth >= MAX_DEPTH) return 0;

	if (flag & USE_EXCLUDES) {
		if (match(name,trie)) return 0;
	}

	// opening stream and erro checking
	DIR *dirpointer = opendir(name);
	if (dirpointer == NULL) {

		// don't return an error for failed accesses if we're recursing
		if (errno == ENOENT || errno == ENOTDIR) {
			if (depth) {
				return 0;
			}
		}

		switch (errno) {
			case EACCES:
				printerror("access to the directory is denied");
				break;
			case ELOOP:
				printerror("too many symbolic links were encountered to resolve the path");
				break;
			case ENAMETOOLONG:
				printerror("real path name exceeds maximum path length (PATH_MAX)");
				break;
			case ENOENT:
				printerror("directory does not exist");
				break;
			case ENOTDIR:
				printerror("path is not a directory");
				break;
			case EMFILE:
				printerror("currently at file descriptor limit (OPEN_MAX)");
				break;
			case ENFILE:
				printerror("too many files are currently open on the system");
				break;
			default:
				printerror("error opening the directory");
		}

		closedir(dirpointer);
		return errno;
	}

	// main loop
	while (dirpointer) {

		errno = 0;
		struct dirent *dp = readdir(dirpointer);
		if (dp == NULL) {
			if (errno) {
				switch (errno) {
					case EOVERFLOW:
						printerror("cannot read one or more values in the directory stream");
						break;
					case EBADF:
						printerror("directory stream is not open");
						break;
					case ENOENT:
						printerror("directory stream is invalid");
						break;
					default:
						printerror("error reading directory");
				}
				closedir(dirpointer);
				return errno;
			}

			closedir(dirpointer);
			return 0;
		}

		if ((strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0))
			; // do nothing
		else {
			if (flag & USE_EXCLUDES) {
				if (match(dp->d_name,trie)) continue;
			}
			if (flag & HIDE_HIDDEN) {
				if (dp->d_name[0] == '.') continue;
			}
			append(dp->d_name,&fb);

			if (flag & CLEAN_PRINT) {
				unsigned int i = 0;
				unsigned int dcount = 0;
				char c;
				while ((c = fb.seq[i]) != '\0') {
					if (dcount > depth) putchar(c);
					else putchar(' ');
					if (c == '/') dcount++;
					i++;
				}
				if (dp->d_type == DT_DIR) putchar('/');
				putchar('\n');
			} else {
				printf("%s\n",fb.seq);
			}

			if (dp->d_type == DT_DIR) {
				depth++;
				listdir(fb.seq);
				depth--;
			}
			removelast(&fb);
		}
	}

	closedir(dirpointer);
	return 0;
}

int main(int argc, char **argv) {

	int r = processargs(argc, argv);
	if (r < 0) return r;
	else if (r) return 0;

	init();

	append(dirpath,&fb);
	listdir(dirpath);

	endit();
	return 0;

}
