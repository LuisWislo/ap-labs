#include <stdio.h>
#include <stdlib.h>
//Luis Wilson A00226649

static char daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

static char *monthnames[] = {"non", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};


void month_day(int year, int yearday, int *pmonth, int *pday){
    int leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
    int i;

    for(i=1; i<13; i++){
        if(yearday<=daytab[leap][i])
            break;
        yearday-=daytab[leap][i];
    }

    *pday = yearday;
    *pmonth = i;
}

int main(int argc, char **argv) {

    if(argc != 3){
        printf("Unsupported number of parameters.\n");
    }
    else{

        int year = atoi(argv[1]);
        int yearday = atoi(argv[2]);

        int validYear = year>0;

        if(validYear){
        
            int leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
            int validYearDay = (leap && (yearday > 0 && yearday<367)) || (!leap && (yearday > 0 && yearday < 366));

            if(validYearDay){
                //proceed to function
                int theMonth, theDay, *pMonth, *pDay;
                pMonth = &theMonth;
                pDay = &theDay;

                month_day(year, yearday, pMonth, pDay);

                char leading = theDay<10? '0' : '\0';

                printf("%s %c%d, %d\n", monthnames[theMonth], leading, theDay, year);

            }
            else{
                printf("Invalid yearday for given year.\n");
            }
        }
        else{
            printf("Invalid year.\n");
        }
    }

    return 0;
}
