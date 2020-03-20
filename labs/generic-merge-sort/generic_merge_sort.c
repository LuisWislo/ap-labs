#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Luis Wilson A00226649

void copy(void *src[], void *dest[], int elements) {
    int i;

    for (i = 0; i < elements; i++) {
        dest[i] = src[i];
    }
    
}

int numcmp(char *s1, char *s2) {
    double v1, v2;

    v1 = atof(s1);
    v2 = atof(s2);

    if(v1 < v2) 
        return -1;
    if(v1 > v2)
        return 1;
    return 0;
}

void merge(void *src[], int left, int mid, int right, void *dest[], int (*compare)(void *, void *)) {
    int i = left;
    int j = mid;
    int k;

    for (k = left ; k < right; k++) {
        if(i < mid && (j>= right || (*compare)(src[i], src[j]) <= 0)) {
            dest[k] = src[i];
            i++;
        }
        else{
            dest[k] = src[j];
            j++;
        }   
    }
}

void split(void *src[], int left, int right, void *dest[], int (*compare)(void *, void *)) {
    if(right - left < 2) return;
    int mid = (left + right)/2;
    split(dest, left, mid, src, compare);
    split(dest, mid, right, src, compare);
    merge(src, left, mid, right, dest, compare);
}



void mergesort(void *src[], int elements, int (*compare)(void *, void *)) {
    void *dest[elements]; 
    copy(src, dest, elements);
    split(dest, 0, elements, src, compare);
}

int main(int argc, char *argv[])
{
    int numeric = 0;
    int elements = 0;

    if(argc > 1) {

        if(strcmp(argv[1], "-n") == 0) {
            numeric = 1;
            elements--;
            if(argc<=2){
                printf("Not enough arguments entered. Usage: ./a.out -n(optional) argv[0] argv[1] ...\n");
                exit(1);
            }
        }
        elements += argc - 1;

        void *input[elements];
        int i, j;

        for(i=argc-elements, j=0; i<argc; i++,j++) {
            input[j] = argv[i];
        }

        numeric ? mergesort(input, elements, (int (*)(void*, void*))(numcmp)) : mergesort(input, elements, (int (*)(void*, void*))(strcmp));
        
        int k;

        for(k=0; k<elements; k++){
            printf("%s\n", (char*) input[k]);
        }

    } else {
        printf("Not enough arguments entered. Usage: ./a.out -n(optional) argv[0] argv[1] ...\n");
    }
    
}