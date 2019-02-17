#include <stdio.h>
#include <stdlib.h>

const int LINE_LEN = 16;




int main() {

    char line[LINE_LEN];
    printf("Enter a number...\n");
    fgets(line,LINE_LEN,stdin);
    printf("Number entered = %d\n", (int)strtol(line,NULL,10));
    return 0;
}