#include <stdio.h>

//Luis Wilson A00226649

int mystrlen(char*);
char *mystradd(char*, char*);
int mystrfind(char*, char*);

int main(int argc, char **argv) {

    if(argc != 4){
        printf("Unsupported number of arguments. Usage: ./main.o arg0 arg1 arg2");
    } else {
        int initialLength = mystrlen(argv[1]);
        char *concatenated = mystradd(argv[1], argv[2]);
        char *foundSub = mystrfind(concatenated, argv[3]) ? "yes" : "no";
        printf("Initial Length      : %d\n", initialLength);
        printf("New String          : %s\n", concatenated);
        printf("SubString was found : %s\n", foundSub);
    }
    return 0;
}
