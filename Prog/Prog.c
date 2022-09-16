#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<stdbool.h>

#define PATH "/etc/Andreyka/"

int main(int C, char **V){
  printf("%d", C);
  if(C==1){
    printf("hello\n");
    return 0;
  }
  if(C==3){
  	printf("%s", V[1]);
  	bool test = false;
  	char* path_t;
  	if (!strcmp(V[1], "-t")) {
  		test = true;
	    path_t = PATH "time.conf";  	
  	}
  	if (!strcmp(V[1], "-f")) {
  		test = true;
	    path_t = PATH "filename.conf";  	
  	}
  	if (test) {
  		printf(path_t);
		FILE *in = fopen(path_t, "w");
		fprintf(in, V[2]); 
		fclose(in);
    	return 0;
    }
    else {
    	return -1;
    }
  }
  return -1;
}
