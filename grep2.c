
#define _GNU_SOURCE 1
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <regex.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

const uint32_t BUF_SIZE = 1024; 
char *myname;			/* for error messages */
int ignore_case = 0;		/* -i option: ignore case */
int count_case = 0;      /* -c option: count case */
int extended = 0;		/* -E option: use extended RE's */
int recursive_case = 0;
int errors = 0;			/* number of errors */

regex_t pattern;		/* pattern to match */

void compile_pattern(const char* pat);
void process(const char* name, int fd);
void bad_options(void);
void recursive(const char* name, int fd);

int main(int argc, char **argv) {
	int short_command;
	int fd;

	myname = argv[0];
	while ((short_command = getopt(argc, argv, ":icRE")) != -1) {
		switch (short_command) {
		case 'i':
			ignore_case = 1;
			break;
		case 'E':
			extended = 1;
			break;
        case 'c':
            count_case = 1;
            break;
		case 'R':
			recursive_case = 1;
			break;
		case '?':
			bad_options();
			break;
		}
	}

	if (optind == argc) {	
		bad_options();
	}

	compile_pattern(argv[optind]);	/* compile the pattern */
	if (errors) {			/* compile failed */
		return 1;
	}
	else {
		optind++;
	}
	for (int i = optind; i < argc; i++) {
		if (strcmp(argv[i], "-") == 0) {
			bad_options();
		}
		else if ((fd = open(argv[i], O_RDONLY)) != -1) {
			if (recursive_case) {
				recursive(argv[i], fd);
			}
			else {
				process(argv[i], fd);
			}
			close(fd);
		} else {
			fprintf(stderr, "%s: %s: could not open: %s\n", argv[0], argv[i], strerror(errno));
			errors++;
		}
	}
	regfree(&pattern);
	return errors != 0;
}

void compile_pattern(const char *pat) {
	int flags = REG_NOSUB;	/* nmatch and pmatch would be ignored */
	int ret;
	char error[BUF_SIZE];
	if (ignore_case) {
		flags |= REG_ICASE;
	}
	if (extended) {
		flags |= REG_EXTENDED;
	}
	ret = regcomp(&pattern, pat, flags);
	if (ret != 0) {
		(void) regerror(ret, &pattern, error, sizeof(error));
		fprintf(stderr, "%s: pattern `%s': %s\n", myname, pat, error);
		errors++;
	}
}
void process(const char* name, int fd) {
	printf("OK7\n");
	char* buf = NULL;
	size_t size = 0;
	char error[BUF_SIZE];
	int ret;
    int count = 0;
	FILE* file = fdopen(fd, "r");
	if (file == NULL) {
		return;
	}
	printf("OK8\n");
	while (getline(&buf, &size, file) != -1) {
		printf("OK10\n");
		ret = regexec(&pattern, buf, 0, NULL, 0);
		printf("OK9\n");
		if (ret != 0) {
			if (ret != REG_NOMATCH) {
				(void) regerror(ret, & pattern, error, sizeof error);
				fprintf(stderr, "%s: file %s: %s\n", myname, name, error);
				free(buf);
				++errors;
				return;
			}
		} else {
				if (!count_case) {
					printf("%s: %s", name, buf);	/* print matching lines */
				}
				++count;
		}
	}
	free(buf);
	printf("OK10\n");
    if (count_case) {printf("%d\n", count);}
}

void bad_options() {
	fprintf(stderr, "bad_options: %s [-i] [-c] [-E] [-R] pattern [ files ... ]\n", myname);
	exit(1);
}

void recursive(const char* name, int fd) {
        DIR* dir = fdopendir(fd);
		printf("OK1\n");
        struct dirent* dir_info;
        struct stat st;
		printf("OK2\n");
        while ((dir_info = readdir(dir))) {
               if (strcmp(dir_info->d_name, ".") == 0 || strcmp(dir_info->d_name,  "..") == 0) {
                       continue;
					   printf("OK3\n");
               } 
               else {
                       fstatat(fd, dir_info->d_name, &st, 0);
                       int new_fd = open(dir_info->d_name, O_RDONLY);
					   printf("OK4\n");
                       if (S_ISDIR(st.st_mode) && new_fd != -1) {
						   	   printf("OK5\n");
                               recursive(name, new_fd);
                               close(new_fd);
                       }
                       else if (S_ISREG(st.st_mode)){
						   		printf("OK6\n");
                                process(name, new_fd);
								break;
                       }
               }
        }
        closedir(dir);
}
