#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void *m;
char *b;
int f;
struct stat st;

void *madviseThread(void *arg) {
    for(int i = 0; i < 1000000; i++) madvise(m, 100, MADV_DONTNEED);
    return NULL;
}

void *writeThread(void *arg) {
    for(int i = 0; i < 1000000; i++) { 
        lseek(f, 0, 0); 
        write(f, b, strlen(b)); 
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if(argc < 3) { 
        printf("Usage: %s target source\n", argv[0]); 
        return 1; 
    }
    f = open(argv[1], O_RDONLY);
    fstat(f, &st);
    m = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, f, 0);
    b = malloc(st.st_size);
    FILE *fp = fopen(argv[2], "rb");
    fread(b, 1, st.st_size, fp);
    fclose(fp);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, madviseThread, NULL);
    pthread_create(&t2, NULL, writeThread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
