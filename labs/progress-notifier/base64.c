#include <stdio.h>
#include "logger.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define BASE46_H
#define CHUNK_SIZE_ENCODE 3
#define CHUNK_SIZE_DECODE 4


char base46_map[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                     'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                     'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

//Taken from: https://github.com/elzoughby/Base64/blob/master/base64.c
char* base64_encode(char* plain) {

    char counts = 0;
    char buffer[3];
    char* cipher = malloc(strlen(plain) * 4 / 3 + 4);
    int i = 0, c = 0;

    for(i = 0; plain[i] != '\0'; i++) {
        buffer[counts++] = plain[i];
        if(counts == 3) {
            cipher[c++] = base46_map[buffer[0] >> 2];
            cipher[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
            cipher[c++] = base46_map[((buffer[1] & 0x0f) << 2) + (buffer[2] >> 6)];
            cipher[c++] = base46_map[buffer[2] & 0x3f];
            counts = 0;
        }
    }

    if(counts > 0) {
        cipher[c++] = base46_map[buffer[0] >> 2];
        if(counts == 1) {
            cipher[c++] = base46_map[(buffer[0] & 0x03) << 4];
            cipher[c++] = '=';
        } else {                      // if counts == 2
            cipher[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
            cipher[c++] = base46_map[(buffer[1] & 0x0f) << 2];
        }
        cipher[c++] = '=';
    }

    cipher[c] = '\0';   /* string padding character */
    return cipher;
}

//Taken from: https://github.com/elzoughby/Base64/blob/master/base64.c
char* base64_decode(char* cipher) {

    char counts = 0;
    char buffer[4];
    char* plain = malloc(strlen(cipher) * 3 / 4);
    int i = 0, p = 0;

    for(i = 0; cipher[i] != '\0'; i++) {
        char k;
        for(k = 0 ; k < 64 && base46_map[k] != cipher[i]; k++);
        buffer[counts++] = k;
        if(counts == 4) {
            plain[p++] = (buffer[0] << 2) + (buffer[1] >> 4);
            if(buffer[2] != 64)
                plain[p++] = (buffer[1] << 4) + (buffer[2] >> 2);
            if(buffer[3] != 64)
                plain[p++] = (buffer[2] << 6) + buffer[3];
            counts = 0;
        }
    }

    plain[p] = '\0';    /* string padding character */
    return plain;
}

int encode() {

}

int decode() {

}

int proceed(char *which, char *input_file) {
    int fd = open(input_file, O_RDONLY, 0600);
    char *output_name = strcmp(which, "encode") == 0 ? "encoded.txt" : "decoded.txt";

    int output = open(output_name,  O_CREAT | O_WRONLY, 0600);

    if(fd == -1) {
        errorf("Failed to open file.\n");
        return -1;
    }

    int chunk_size;

    if(strcmp(which, "encode") == 0) {
            chunk_size = CHUNK_SIZE_ENCODE;
        } else {
            chunk_size = CHUNK_SIZE_DECODE;
        }

    char buffer[chunk_size];
    int r;

    while(r = read(fd, buffer, chunk_size) > 0){
        char *to_do; 
        if(strcmp(which, "encode") == 0) {
            to_do = base64_encode(buffer);
        } else {
            to_do = base64_decode(buffer);
        }
        int len = strlen(to_do);
        write(output, to_do, len);
    }
}

int main(int argc, char** argv){


    if(argc != 3) {
        warnf("Unsupported number of parameters. Usage: ./base64 --encode/--decode <input_file>\n");
    } else {

        if(strcmp(argv[1], "--encode") == 0) {
            proceed("encode", argv[2]);
        } else if(strcmp(argv[1], "--decode") == 0) {
            proceed("decode", argv[2]);
        } else {
            warnf("Unsupported parameter. Usage: ./base64 --encode/--decode <input_file>\n");
        }
    }
    return 0;
}
