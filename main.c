#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

#define SHOW_HIDDEN 0x01

unsigned int flag;

void printerror(const char *msg) {
	printf("Error: %s\n",msg);
}

void printusage() {
	printf("Enter a directory.\n");
}

int main(int argc, char **argv) {

	flag = 0;

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

	DIR *dirpointer = opendir(dirpath);

	// error checking for opening the directory stream
	if (errno) {
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

			return 0;
		}

		printf("%s\n",dp->d_name);
	}

	closedir(dirpointer);

	return 0;

}
