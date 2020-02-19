#include <stdio.h>

#define   LOWER  0       /* lower limit of table */
#define   UPPER  300     /* upper limit */
#define   STEP   20      /* step size */

/* print Fahrenheit-Celsius table */
//Luis Fernando Wilson ELias A00226649

int main(int argc, char **argv)
{
    int fahr;

    if(argc == 1){
        for (fahr = LOWER; fahr <= UPPER; fahr = fahr + STEP)
        printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));

    }
    else if(argc == 2){
        fahr = atoi(argv[1]);
        printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
    }
    else if(argc == 4){
        for (fahr = atoi(argv[1]); fahr <= atoi(argv[2]); fahr = fahr + atoi(argv[3]))
        printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
    }
    else {
        printf("Unsupported number of arguments.\n");
    }

    return 0;
}