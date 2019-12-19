#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <regex.h>
#include <sys/types.h>

uint8_t ignor_ = 0;     //i - option: ignor
uint8_t extended = 0;   //E - option: extended information
uint32_t errors = 0;

regex_t pattern;    //regular pattern to find

void compile_pattern(const char *pat);
void process(const char *name, FILE *fp);
void usage(void);

int main(int argc, char **argv) {
        int c;
        int i;
        FILE *fp;
	    myname = argv[0];
        while ((c = getopt(argc, argv, ":iE")) != -1) {
                switch (c) {
                    case 'i':
		                ignore_case = 1;
                        break;
                    case 'E':
			            extended = 1;
                        break;
                    case '?':
                        usage();
                        break;
	    	    }
    	}
}


