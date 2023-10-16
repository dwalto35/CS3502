#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX 10240
#define NUM_THREADS 10

int n1, n2;
char *s1, *s2;
FILE *fp;
int totalNum = 0; 
int countArray[NUM_THREADS] = {0};

int num_substring(int t, char *start, int len) {
    int count = 0;

    for (int i = 0; i <= len - n2; i++) {
        int j;
        for (j = 0; j < n2; j++) {
            if (start[i + j] != s2[j]) {
                break;
            }
        }
        if (j == n2) {
            count++;
        }
    }

    return count;
}

void *calSubStringThread(void *threadid) {
    long tid = (long)threadid;
    int len_per_thread = n1 / NUM_THREADS;
    char *start = s1 + tid * len_per_thread;
    int count = num_substring(tid, start, len_per_thread);
    
    // Display a message for each substring thread
    printf("The number of substring thread %ld find is: %d\n", tid, count);

    countArray[tid] = count;

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int t, rc;

    if ((fp = fopen("strings.txt", "r")) == NULL) {
        printf("ERROR: can't open strings.txt!\n");
        return 1;
    }

    s1 = (char *)malloc(sizeof(char) * MAX);
    if (s1 == NULL) {
        printf("ERROR: Out of memory!\n");
        return -1;
    }

    s2 = (char *)malloc(sizeof(char) * MAX);
    if (s2 == NULL) {
        printf("ERROR: Out of memory\n");
        return -1;
    }

    s1 = fgets(s1, MAX, fp);
    s2 = fgets(s2, MAX, fp);
    n1 = strlen(s1);
    n2 = strlen(s2) - 1;

    if (s1 == NULL || s2 == NULL || n1 < n2) {
        return -1;
    }

    for (t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], NULL, calSubStringThread, (void *)(size_t)t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    for (t = 0; t < NUM_THREADS; t++) {
        totalNum += countArray[t];
    }

    printf("The number of substrings is: %d\n", totalNum);
    return 0;
}
