#include <stdio.h>
#include "functions.h"

int main(int argc, char* argv[]) {
char in_name[] = "test1.txt";
char out_name[] = "output1.bin";
char *in_name_p=in_name;
char *out_name_p=out_name;
   converter(in_name_p,out_name_p);
   printf("Завершение программы! \n");

return 0;
}


