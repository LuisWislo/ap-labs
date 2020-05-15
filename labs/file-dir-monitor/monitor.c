#define _XOPEN_SOURCE 500
#include <stdio.h>
#include "logger.h"
#include <sys/inotify.h>
#include <unistd.h>
#include <ftw.h>
#define NO_EVENTS 1024
#define EVENT_SIZE (sizeof (struct inotify_event))
#define LEN_NAME 20
#define BUFFER_SIZE (NO_EVENTS * (EVENT_SIZE + LEN_NAME))

int fd;

int nftw_function(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    int watch;

    watch = inotify_add_watch(fd, fpath, IN_CREATE | IN_MODIFY | IN_DELETE);

    /*if(watch < 0) {
        errorf("Unable to watch directory\n");
        return -1;
    }*/

    return 0;
}


int main(int argc, char** argv){
    initLogger("stdout");

    if (argc != 2) {
        warnf("Unsupported number of arguments.\n");
        infof("Usage: ./monitor </directory>\n");
        return -1;
    }

    char *directory = argv[1];

    int i;
    char buffer[4096];

    fd = inotify_init();

    if(fd < 0) {
        errorf("Error initializing inotify.\n");
        return -1;
    }

    int traversal;
    traversal = nftw(directory, nftw_function, 30, FTW_PHYS);

    if(traversal < 0) {
        errorf("Error traversing directories.\n");
        return -1;

    }

    while(1) {
        
        int j = 0;
        int readBytes = read(fd, buffer, sizeof(buffer));

        if(readBytes < 0) {
            errorf("Unable to read file descriptor\n");
            return -1;
        }

        while (j < readBytes) {
            struct inotify_event *ev = (struct inotify_event *) buffer;

            if(ev->mask & IN_CREATE) {
                infof("Created %s!\n",ev->name);
            } else if(ev->mask & IN_MODIFY) {
                infof("Modified %s!\n",ev->name);
            } else if(ev->mask & IN_DELETE) {
                infof("Deleted %s!\n",ev->name);
            }

            j+= EVENT_SIZE + ev->len;

        }

    }

    return 0;
}
