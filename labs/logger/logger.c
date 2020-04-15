#include<stdio.h> 
#include<stdarg.h>		
#include<string.h>
#include<stdlib.h>
#include<signal.h>

//Luis Wilson A00226649

int infof(const char *format, ...) {
    int r;
    char color[13];
    sprintf(color, "%c[%d;%d;%dm", 0x1B, 0, 36, 40); ///cyan over black
    printf("%s", color);
    va_list args;
    va_start(args, format);
    r = vprintf(format, args);
    va_end(args);
    return r;
}

int warnf(const char *format, ...) {
    int r;
    char color[13];
    sprintf(color, "%c[%d;%d;%dm", 0x1B, 0, 33, 40); // yellow over black
    printf("%s", color);
    va_list args;
    va_start(args, format);
    r = vprintf(format, args);
    va_end (args);
    return r;
}

int errorf(const char *format, ...) {
    int r;
    char color[13];
    sprintf(color, "%c[%d;%d;%dm", 0x1B, 0, 35, 40); // magenta over black
    printf("%s", color);
    va_list args;
    va_start(args, format);
    r = vprintf(format, args);
    va_end (args);
    return r;
}

int panicf(const char *format, ...) {
    int r;
    char color[13];
    sprintf(color, "%c[%d;%d;%dm", 0x1B, 0, 31, 40); // red over black
    printf("%s", color);
    va_list args;
    va_start(args, format);
    r = vprintf(format, args);
    va_end (args);
    raise (SIGABRT);
    return r;
}