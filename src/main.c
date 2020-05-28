#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>
#include <errno.h>
#include "filenamebuffer.h"
#include "chartrie.h"

#define TERM_COLOR_RED() printf("\e[31m")
#define TERM_COLOR_GREEN() printf("\e[32m")
#define TERM_COLOR_YELLOW() printf("\e[33m")
#define TERM_COLOR_BLUE() printf("\e[34m")
#define TERM_COLOR_CYAN() printf("\e[36m")
#define TERM_COLOR_BRIGHTRED() printf("\e[1;31m")
#define TERM_COLOR_BRIGHTGREEN() printf("\e[1;32m")
#define TERM_COLOR_BRIGHTYELLOW() printf("\e[1;33m")
#define TERM_COLOR_BRIGHTBLUE() printf("\e[1;34m")
#define TERM_COLOR_BRIGHTMAGENTA() printf("\e[1;35m")
#define TERM_COLOR_BRIGHTCYAN() printf("\e[1;36m")
#define TERM_COLOR_WHITE() printf("\e[37m")
#define TERM_COLOR_RESET() printf("\e[0m")

#define MAX_DEPTH 		20

#define HIDE_HIDDEN 	0x01
#define USE_EXCLUDES 	0x02
#define CLEAN_PRINT 	0x04
#define COLOR_PRINT 		0x08
#define RECURSIVE_LIST	0x10
#define FOLLOW_SYMLINKS	0x20

unsigned int flag;
unsigned int depth;
unsigned char lasttype;
struct filenamebuffer fb;
chartrie *trie;
char *fbstart;
char dirpath[PATH_MAX+1];
char fbpath[PATH_MAX+1];

// forward declaring listdir
int listdir(const char *);

void printerror(const char *format, ...) {
	va_list args;
	if (flag & COLOR_PRINT) TERM_COLOR_BRIGHTRED();
	printf("Error: ");
	vprintf(format, args);
	putchar('\n');
	if (flag & COLOR_PRINT) TERM_COLOR_RESET();
}

void printwarning(const char *format, ...) {
	va_list args;
	if (flag & COLOR_PRINT) TERM_COLOR_BRIGHTMAGENTA();
	printf("Warning: ");
	vprintf(format, args);
	putchar('\n');
	if (flag & COLOR_PRINT) TERM_COLOR_RESET();
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

int processargs(int argc, char **argv) {
	
	// returns: negative values for errors, positive values for no ops, 0 for continue

	// initial conditions
	flag = 0;
	flag |= HIDE_HIDDEN;
	flag |= USE_EXCLUDES;
	flag |= CLEAN_PRINT;
	flag |= COLOR_PRINT;
	flag |= FOLLOW_SYMLINKS;
	flag |= RECURSIVE_LIST;

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
			// setting the base pointer to skip over the first directory
			if (flag & CLEAN_PRINT) {
				unsigned int i = 0;
				while (dirpath[i] != '\0') i++;
				fbstart = dirpath + i;
			}
		}
	}

	return 0;

}

void init() {

	// reset terminal color
	TERM_COLOR_RESET();

	lasttype = DT_UNKNOWN;

	fbstart = NULL;
	dirpath = NULL;

	depth = 0;
	fb.seq = fbpath;
	fb.pos = 0;

	// sets up our excludes trie if we need it
	if (flag & USE_EXCLUDES) {
		trie = makenode('\0');
	
		// TODO: use buf for opening the file and building the tree 
		// to save space...
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
	if (flag & COLOR_PRINT) {
		const char *term = getenv("TERM");
		if (strstr(term,"xterm-color") || strstr(term,"-256color")) 
			 ; //we're cool
		else 
			flag &= ~COLOR_PRINT;
	}

	return;
}

void endit() {
	// if we were using color printing, ensure that we reset 
	// the terminal color
	if (flag & COLOR_PRINT) TERM_COLOR_RESET();
}

void printdir(struct dirent *dp) {

	// prints a directory from the filenamebuffer
	
	// TODO: print out the parent directory once every so often so that with
	// larger outputs, we can still see what the parent folder is
	// maybe use $LINES
	// FIXME: when not using the . directory as the main argument, directories will
	// print out with their parent directories using clean printing
	
	unsigned char type = dp->d_type;

	if (flag & COLOR_PRINT && type != lasttype) {
		switch (type) {
			case DT_DIR:
				TERM_COLOR_BRIGHTBLUE();
				break;
			case DT_LNK:
				TERM_COLOR_BRIGHTCYAN();
				break;
			case DT_CHR:
				TERM_COLOR_BRIGHTYELLOW();
				break;
			default:
				TERM_COLOR_WHITE();
		}

		lasttype = type;
	}

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
	} else {
		printf("%s",fb.seq);
	}
	if (type == DT_DIR) putchar('/');
	if (type == DT_REG) putchar('*');
	if (type == DT_LNK) {
		printf(" -> ");
		// FIXME: this is bad
		char *rp = realpath(fb.seq,NULL);
		printf("%s",rp);
		// we can get rid of malloc!
		free(rp);
	}
	putchar('\n');
}

int listdir(const char *path) {

	//printf("attempting to access directory %s\n",path);

	if (depth >= MAX_DEPTH) return 0;

	if (flag & USE_EXCLUDES) {
		if (match(path,trie)) return 0;
	}

	// opening stream and erro checking
	DIR *dirpointer = opendir(path);
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
				printerror("real path path exceeds maximum path length (PATH_MAX)");
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

	// FIXME: I don't think closedir means anything once we've looped
	// through all the files
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
			continue; // do nothing
		if (flag & USE_EXCLUDES && match(dp->d_name,trie))
			continue; 
		if (flag & HIDE_HIDDEN && dp->d_name[0] == '.') 
			continue;	

		append(dp->d_name,&fb);

		printdir(dp);

		if (flag & RECURSIVE_LIST && 
				(dp->d_type == DT_DIR || 
				 (dp->d_type == DT_LNK && (flag & FOLLOW_SYMLINKS)))) {
			depth++;
			listdir(fb.seq);
			depth--;
		}
		removelast(&fb);
	}

	closedir(dirpointer);
	return 0;
}

int main(int argc, char **argv) {

	int r = processargs(argc, argv);
	if (r < 0) return r;
	else if (r) return 0;
	init();


	printwarning("test warning");
	printerror("test erro");

	append(dirpath,&fb);
	listdir(dirpath);

	endit();
	return 0;

}
