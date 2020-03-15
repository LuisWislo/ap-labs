#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

// Luis Wilson A00226649

#define REPORT_FILE "packages_report.txt"
#define MAX_LENGTH 1000
#define DATE_LENGTH 17
#define PACKAGE_NAME_LENGTH 50
#define ACTION_LENGTH 15
#define MANAGER_LENGTH 30
#define MAX_PACKAGES 5000
#define NUMBER_BUFFER_LENGTH 15

void analizeLog(char *logFile, char *report);

struct package {
    char name[PACKAGE_NAME_LENGTH];
    char install_date[DATE_LENGTH];
    char lastupdate_date[DATE_LENGTH];
    int many_updates;
    char removal_date[DATE_LENGTH];
};

struct package packages[MAX_PACKAGES];
int package_index = 0;
int global_installed = 0,
    global_removed = 0,
    global_upgraded = 0;

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Usage:./pacman-analizer.o pacman.log\n");
        return 1;
    }
    
    analizeLog(argv[1], REPORT_FILE);

    return 0;
}

void getContent(char *line, int *line_index, char stop_char, char *output){
    
    int output_index = 0;

    while(line[*line_index] != stop_char){
        output[output_index++] = line[*line_index];
        *line_index = *line_index + 1;
    }

    output[output_index] = '\0';
}

void to_int_date(char *date, int *year, int *month, int *day, int *hr, int *min) {
    int i, j=0;
    char container[5];

    for(i=0; i<4; i++){
        container[j++] = date[i];
    }
    container[j] = '\0';
    j=0;

    *year = atoi(container);

    for(i=5; i<7; i++){
        container[j++] = date[i];
    }
    container[j] = '\0';
    j=0;

    *month = atoi(container);
    // 2019-03-22 21:24

    for(i=8; i<10; i++){
        container[j++] = date[i];
    }
    container[j] = '\0';
    j=0;

    *day = atoi(container);

    for(i=11; i<13; i++){
        container[j++] = date[i];
    }
    container[j] = '\0';
    j=0;

    *hr = atoi(container);

    for(i=14; i<16; i++){
        container[j++] = date[i];
    }
    container[j] = '\0';
    j=0;

    *min = atoi(container);

}

//returns 0 if both are equal
//returns -1 if date_0 is greater
//returns 1 if date_1 is greater
int compare_date(char *date_0, char *date_1) {
    // 2019-03-22 21:24

    int year0 = 0, month0 = 0, day0 = 0, hr0 = 0, min0 = 0;
    int *pyear0 = &year0, *pmonth0 = &month0, *pday0 = &day0, *phr0 = &hr0, *pmin0 = &min0; 

    int year1 = 0, month1 = 0, day1 = 0, hr1 = 0, min1 = 0;
    int *pyear1 = &year1, *pmonth1 = &month1, *pday1 = &day1, *phr1 = &hr1, *pmin1 = &min1;

    to_int_date(date_0, pyear0, pmonth0, pday0, phr0, pmin0);
    to_int_date(date_1, pyear1, pmonth1, pday1, phr1, pmin1);

    //printf("%s as %d, %d, %d, %d, %d\n", date_0, year0, month0, day0, hr0, min0);
    //printf("%s as %d, %d, %d, %d, %d\n", date_1, year1, month1, day1, hr1, min1);

    if(year0 > year1) return -1;
    if(year0 < year1) return 1;
    if(month0 > month1) return -1;
    if(month0 < month1) return 1;
    if(day0 > day1) return -1;
    if(day0 < day1) return 1;
    if(hr0 > hr1) return -1;
    if(hr0 < hr1) return 1;
    if(min0 > min1) return -1;
    if(min0 < min1) return 1;
    return 0;
}

int is_in_array(char *pname){
    int i;
    for(i=0; i<package_index;i++){
        if(strcmp(packages[i].name, pname) == 0){
            return i;
        }
    }
    return -1;
}

void add_to_packages(char *date, char *pname, char *action){
    int index = is_in_array(pname);
    if(index == -1){
        global_installed++;
        strcpy(packages[package_index].name, pname);
        strcpy(packages[package_index].install_date, date);
        strcpy(packages[package_index].removal_date, "-");
        strcpy(packages[package_index].lastupdate_date, "-");
        packages[package_index].many_updates = 0;
        package_index++;
    } else {
        if(strcmp(action, "installed") == 0){ //means it was reinstalled
            global_installed++;
            strcpy(packages[index].install_date, date);
            strcpy(packages[index].removal_date, "-");

        } else if(strcmp(action, "reinstalled") == 0) {
            global_installed++;
            strcpy(packages[index].install_date, date);
            strcpy(packages[index].removal_date, "-");

        } else if(strcmp(action, "removed") == 0) {
            global_removed++;
            strcpy(packages[index].removal_date, date);
        } else if(strcmp(action, "upgraded") == 0) {
            global_upgraded++;
            packages[index].many_updates++;
            strcpy(packages[index].lastupdate_date, date);
        }
    } 
}

void processLine(char *line) {
    int length = strlen(line);
    int current = 0;
    int *pCurrent = &current;

    char date[DATE_LENGTH];
    char manager[MANAGER_LENGTH];
    char action[ACTION_LENGTH];
    

    if(line[current] == '['){ //else, drop line
        current++;
        getContent(line, pCurrent, ']', date);
        current+=3;
        getContent(line, pCurrent, ']', manager);
        current+=2;
        getContent(line, pCurrent, ' ', action);
        current++;

        if(!strcmp(action, "installed") || !strcmp(action, "removed") || !strcmp(action, "upgraded") || !strcmp(action, "reinstalled")) { 
            char pname[PACKAGE_NAME_LENGTH];
            getContent(line, pCurrent, ' ', pname);
            //printf("Analyzing this line: %s\n", line);
            add_to_packages(date, pname, action);
        }
    }
}

void write_to_file(int filedes) {

    char *toWrite;
    char fornumber[NUMBER_BUFFER_LENGTH];
    int len, i;

    toWrite = "Pacman Packages Report\n";
    len = strlen(toWrite);
    write(filedes, toWrite, len);
    
    toWrite = "----------------------\n";
    len = strlen(toWrite);
    write(filedes, toWrite, len);

    toWrite = "- Installed packages : ";
    len = strlen(toWrite);
    write(filedes, toWrite, len);
    sprintf(fornumber, "%d", global_installed);
    len = strlen(fornumber);
    write(filedes, fornumber, len);
    write(filedes, "\n", 1);


    toWrite = "- Removed packages   : ";
    len = strlen(toWrite);
    write(filedes, toWrite, len);
    sprintf(fornumber, "%d", global_removed);
    len = strlen(fornumber);
    write(filedes, fornumber, len);
    write(filedes, "\n", 1);

    toWrite = "- Upgraded packages  : ";
    len = strlen(toWrite);
    write(filedes, toWrite, len);
    sprintf(fornumber, "%d", global_upgraded);
    len = strlen(fornumber);
    write(filedes, fornumber, len);
    write(filedes, "\n", 1);

    toWrite = "- Current installed  : ";
    len = strlen(toWrite);
    write(filedes, toWrite, len);
    sprintf(fornumber, "%d", global_installed - global_removed);
    len = strlen(fornumber);
    write(filedes, fornumber, len);
    write(filedes, "\n", 1);

    write(filedes, "\n", 1);

    toWrite = "List of packages\n";
    len = strlen(toWrite);
    write(filedes, toWrite, len);

    toWrite = "----------------\n";
    len = strlen(toWrite);
    write(filedes, toWrite, len);

    for(i=0;i<package_index;i++){
        toWrite = "- Package Name        : ";
        len = strlen(toWrite);
        write(filedes, toWrite, len);

        toWrite = packages[i].name;
        len = strlen(toWrite);
        write(filedes, toWrite, len);

        write(filedes, "\n", 1);

        toWrite = "  - Install date      : ";
        len = strlen(toWrite);
        write(filedes, toWrite, len);

        toWrite = packages[i].install_date;
        len = strlen(toWrite);
        write(filedes, toWrite, len);

        write(filedes, "\n", 1);

        toWrite = "  - Last update date  : ";
        len = strlen(toWrite);
        write(filedes, toWrite, len);

        toWrite = packages[i].lastupdate_date;
        len = strlen(toWrite);
        write(filedes, toWrite, len);

        write(filedes, "\n", 1);

        toWrite = "  - How many updates  : ";
        len = strlen(toWrite);
        write(filedes, toWrite, len);

        sprintf(fornumber, "%d", packages[i].many_updates);
        len = strlen(fornumber);
        write(filedes, fornumber, len);

        write(filedes, "\n", 1);

        toWrite = "  - Removal date      : ";
        len = strlen(toWrite);
        write(filedes, toWrite, len);

        toWrite = packages[i].removal_date;
        len = strlen(toWrite);
        write(filedes, toWrite, len);

        write(filedes, "\n", 1);

    }

}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);

    int file = open(logFile, O_RDONLY, 0600);

    if(file == -1){
        printf("Failed to open file.\n");
        exit(1);
    } else {

        char buffer[1];
        char line[MAX_LENGTH];
        int ccounter = 0, r;

        while(r = read(file, buffer, 1) > 0) {
            if(*buffer == '\n'){
                line[ccounter] = '\0';
                processLine(line);
                ccounter = 0;
            } else {
                line[ccounter] = *buffer;
                ccounter++;
            }
            
        }
        line[ccounter] = '\0';
        processLine(line);
        close(file);

        if(r == 0){
            
            int filedes = open(report, O_CREAT | O_WRONLY, 0600);

            if(filedes < 0){
                printf("Failed to create/open output file.\n");
                exit(1);
            } else {
                write_to_file(filedes);
            }
            
        } else if(r == -1){
            printf("Error reading file.\n");
        }
    }

    printf("Report is generated at: [%s]\n", report);
}