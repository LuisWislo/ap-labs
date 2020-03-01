#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100000
//Luis Wilson A00226649

//taken from C-Basics slides
void reverse(int length, char word[]) {

    int i; 
    char tmp;

    for (i = 0;  i < length/2; i++) {
        tmp = word[i];
        word[i] = word[length - i - 1];
        word[length - i - 1] = tmp;
    }
    
    word[length] = '\0';
    printf("%s\n", word);
}

int main()
{
    char x;
    char word[MAX];
    int length = 0;

    while((x = getchar()) != EOF){
        
        if(x == '\n'){
            reverse(length, word);
            length = 0;
        } 
        else {
            word[length] = x;
            length++;
        }
    }
    return 0;
}
