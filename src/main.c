#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include "filenamebuffer.h"
#include "excludelist.h"

#define MAX_DEPTH 20
#define HIDE_HIDDEN 0x01
#define USE_EXCLUDES 0x02

/* flag bits
 * 0: hides hidden files
 * 1: enables file/folder exclusion
 * */

unsigned int flag;
unsigned int depth;
struct filenamebuffer fb;
struct excludelist ex;

// forward declaring listdir
int listdir(const char *);

void printerror(const char *msg) {
	printf("Error: %s\n",msg);
}

void printwarning(const char *msg) {
	printf("Warning: %s\n",msg);
}

void printusage() {
	printf("Enter a directory.\n");
}

void init() {
	depth = 0;
	fb.seq = malloc((PATH_MAX + 1) * sizeof(char));
	fb.pos = 0;

	if (flag & USE_EXCLUDES) {
		// looks for ~/.les.excludes
		FILE *excludesfp = fopen("~/.les.excludes","r");

		if (excludesfp == NULL) {
			// TODO: finish warnings here...
			printwarning("excludes file could not be located, proceeding without");
		}


	}

	init_excludelist(&ex);

	return;
}

void endit() {
	free(fb.seq);
}

int listdir(const char *name) {

	//printf("attempting to access directory %s\n",name);

	if (depth >= MAX_DEPTH) return 0;

	if (flag & USE_EXCLUDES) {

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

		// recurse to subdirectories
		if ((strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0) || matchexclude(dp->d_name))
			; // do nothing
		else {
			append(dp->d_name,&fb);
			printf("%s\n",fb.seq);
			depth++;
			listdir(fb.seq);
			depth--;
			removelast(&fb);
		}
	}

	closedir(dirpointer);
	return 0;
}

int main(int argc, char **argv) {

	flag = 0x02;	// use excludes

	init();

	addexclude(".git",&ex);

	char *dirpath;

	if (argc < 2 || argc > 3) {
		printusage();
	 	return -1;
	}
	if (argv[1][0] == '\0') {
		printusage();
		return -1;
	}

	dirpath = argv[1];

	append(dirpath,&fb);
	listdir(dirpath);

	endit();
	return 0;

}
