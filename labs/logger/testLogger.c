#include <stdio.h>
#include<stdlib.h>

int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);
//Luis Wilson A00226649
int main() {
    char c = 'h';
    int d = 34;
    float f = 37.78;
    unsigned int o = 50;
    unsigned int x = 33;
    char *s = "a string";

    infof("%c, %d, %f, %o, %x, %s in blue\n", c ,d ,f ,o ,x ,s);
    warnf("%c, %d, %f, %o, %x, %s in yellow\n", c ,d ,f ,o ,x ,s);
    errorf("%c, %d, %f, %o, %x, %s in magenta\n", c ,d ,f ,o ,x ,s);
    panicf("%c, %d, %f, %o, %x, %s in red\n", c ,d ,f ,o ,x ,s);
    
    return 0;
}