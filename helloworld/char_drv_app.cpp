#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{

    printf("LED GPIO START");
    // int fd;
    // char buffer[1024] = "this is user application";

    // fd = open("/dev/test", O_RDWR);

    // write(fd, buffer, strlen(buffer)+1);
    // read(fd, buffer, 1024);
    // printf("%s\n", buffer);

    char szPath[64] = {0,};
    int nLedBlude = 1;
    int nLedRed = 1;
    FILE* pFileBlue = nullptr;
    FILE* pFileRed = nullptr;
    int count = 0;
    sprintf(szPath, "/sys/class/gpio/gpio299/value");
    pFileBlue = fopen(szPath, "w");
    if(pFileBlue == NULL)
    {
        printf("Gpio blue set value error\n");
        return -1;
    }


    sprintf(szPath, "/sys/class/gpio/gpio301/value");
    pFileRed = fopen(szPath, "w");
    if(pFileRed == NULL)
    {
        printf("Gpio red set value error\n");
        return -1;
    }

    while(1)
    {
        ++count;
        
        nLedBlude = nLedBlude == 1 ? 0 : 1;
        nLedRed = nLedRed == 1 ? 0 : 1;

        printf("nLedBlude[%d]\n", nLedBlude);

        
        fprintf(pFileBlue, "%d", nLedBlude);
        fflush(pFileBlue);
        fprintf(pFileRed, "%d", nLedRed);
        fflush(pFileRed);
        sleep(5);
        if(count == 5)
            break;
    }

    
    fclose(pFileBlue);
    fclose(pFileRed);

    return 0;
}