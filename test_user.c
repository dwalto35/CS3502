#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

int main(int argc, char *argv[])
{
    int fd;
    unsigned char *p_map;

    /* open proc file */
    fd = open("/proc/mydir/myinfo", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        exit(1);
    }
    else
    {
        printf("open successfully\n");
    }

    /* map p_map to the proc file and grant read & write privilege */
    p_map = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p_map == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        exit(1);
    }

    /* read data from p_map */
    printf("Data read from kernel space: ");
    for (int i = 0; i < 12; ++i)
    {
        printf("%d ", p_map[i]);
    }
    printf("\n");

    /* unmap p_map from the proc file */
    if (munmap(p_map, PAGE_SIZE) == -1)
    {
        perror("munmap");
        close(fd);
        exit(1);
    }

    close(fd);

    return 0;
}
