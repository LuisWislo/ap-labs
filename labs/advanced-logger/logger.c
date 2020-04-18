#include <stdio.h>
#include "logger.h"
#include<stdarg.h>		
#include<string.h>
#include<stdlib.h>
#include<signal.h>
 #include <syslog.h>

int is_stdout = 1;

//Luis Wilson A00226649

int initLogger(char *logType) {

    char color[13];
    sprintf(color, "%c[%d;%d;%dm", 0x1B, 0, 37, 40); ///white over black
    printf("%s", color);

    if(strcmp(logType, "stdout") == 0) {
        printf("\nInitializing Logger on: %s\n", logType);
        is_stdout = 1;
    } else if(strcmp(logType, "syslog") == 0) {
        printf("\nInitializing Logger on: %s\n", logType);
        is_stdout = 0;
    } else {
        printf("\nInvalid init option %s. Please input 'stdout' or 'syslog'\n", logType);
    }

    return 0;
}

int infof(const char *format, ...) {
    int r = 0;
    char color[13];
    sprintf(color, "%c[%d;%d;%dm", 0x1B, 0, 36, 40); ///cyan over black
    printf("%s", color);
    va_list args;
    va_start(args, format);

    if(is_stdout) {
        r = vprintf(format, args);
    } else {
        vsyslog(LOG_INFO, format, args);
    }
    
    va_end(args);
    return r;
}

int warnf(const char *format, ...) {
    int r = 0;
    char color[13];
    sprintf(color, "%c[%d;%d;%dm", 0x1B, 0, 33, 40); // yellow over black
    printf("%s", color);
    va_list args;
    va_start(args, format);

    if(is_stdout) {
        r = vprintf(format, args);
    } else {
        vsyslog(LOG_WARNING, format, args);
    }

    va_end (args);
    return r;
}

int errorf(const char *format, ...) {
    int r = 0;
    char color[13];
    sprintf(color, "%c[%d;%d;%dm", 0x1B, 0, 35, 40); // magenta over black
    printf("%s", color);
    va_list args;
    va_start(args, format);
    
    if(is_stdout) {
        r = vprintf(format, args);
    } else {
        vsyslog(LOG_ERR ,format, args);
    }

    va_end (args);
    return r;
}

int panicf(const char *format, ...) {
    int r = 0;
    char color[13];
    sprintf(color, "%c[%d;%d;%dm", 0x1B, 0, 31, 40); // red over black
    printf("%s", color);
    va_list args;
    va_start(args, format);
    
    if(is_stdout) {
        r = vprintf(format, args);
    } else {
        vsyslog(LOG_EMERG, format, args);
    }

    va_end (args);
    return raise (SIGABRT);
    //return r;
}

