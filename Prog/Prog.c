#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define PATH "/etc/Hello_from_kernel_dir/"

int main(int argc, char **argv){
  if(argc == 1) {
    printf("-f to change the filepath\n-t to change the timer (s)\n\
ex:\nsudo ./Prog -f ~/filename123\nsudo ./Prog -t 5\n");
    return 0;
  }
  if(argc == 3) {
  	char* conf_path;
  	if (!strcmp(argv[1], "-t")) {
  		for (int i = 0; argv[2][i] != '\0'; ++i) {
  			if (!isdigit(argv[2][i]) || argv[2][0] == '0') {
  				printf("Invalid number\n");
  				return -1;
  			}
  		}
	    conf_path = PATH "time.conf"; 	
	    FILE *out = fopen(conf_path, "w");
		fprintf(out, "%s000", argv[2]); 
		fclose(out);
    	return 0;
  	}
  	if (!strcmp(argv[1], "-f")) {
	    conf_path = PATH "filename.conf";
	    FILE *out = fopen(conf_path, "w");
		fprintf(out, "%s", argv[2]); 
		fclose(out);
    	return 0;  	
  	}
	printf("Invalid arg name\n");
	return -1;
  }
  printf("Invalid arg count\n");
  return -1;
}
