#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <wiringPi.h>
#include <pcf8591.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>


#define         PCF     120
#define         D0pin   0
#define         Buzz    1
#define         PORT 3490
#define         MAXDATASIZE 100


void Print(int x)
{
        switch(x)
        {
                case 1:
                        printf("\n*********\n"  );
                        printf(  "* Safe~ *\n"  );
                        printf(  "*********\n\n");
                break;
                case 0:
                        printf("\n***************\n"  );
                        printf(  "* Danger Gas! *\n"  );
                        printf(  "***************\n\n");
                break;
                default:
                        printf("\n**********************\n"  );
                        printf(  "* Print value error. *\n"  );
                        printf(  "**********************\n\n");
                break;
        }
}
/* Camera Shot */
void shot()
{
        pid_t pid;
        pid=fork();
        if(pid==0)
                execvp("/home/pi/./camera.sh",NULL);
        else
                wait(NULL);
}
/* TURN on the LED at Raspberrypi*/
void ledon()
{
        char *mesg[] = {"WARNING",0};
        pid_t pid;
        pid=fork();
        if(pid==0)
        {       execvp("/home/pi/./spiscroll",mesg);
        }
        else
                wait(NULL);
}
/* TURN Off the LED at RaspberryPI*/
void ledoff()
{
        pid_t pid;
        pid=fork();
        if(pid==0)
                execvp("/home/pi/./off",NULL);
        else
                wait(NULL);
}
/* Send the Message to ARTIK when it's WARNING DETECTION */
void warningsocket(int sockfd)
{
        char Data[MAXDATASIZE] = "WARNING DETECTION";
        // memory allocation for reading buffer

        int length = strlen(Data);

  if (send(sockfd, Data, length+1, 0) == -1) {
            perror("send");
        }


}
/* Send the Message to ARTIK when it's ISSAFE */
void oksocket(int sockfd)
{
        char Data[MAXDATASIZE] = "ISSAFE";
        int length = strlen(Data);

        if(send(sockfd, Data, length+1,0) == -1){
                perror("send");
        }
}

int main()
{
        int analogVal;
        int tmp, status, count;

        if(wiringPiSetup() == -1){
                printf("setup wiringPi failed !");
                return 1;
        }
        int sockfd, numbytes;
    char *send_data;
    struct hostent *he;
    struct sockaddr_in their_addr;


    if ((he=gethostbyname("10.168.168.103")) == NULL) {
        herror("gethostbyname");
        exit(1);
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(PORT);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(their_addr.sin_zero), 8);
    if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }

    printf("============ [PORT] : %d ============\n", PORT);
    printf("======== Connecting to ARTIK ========\n");

        // Setup pcf8591 on base pin 120, and address 0x48
        pcf8591Setup(PCF, 0x48);

        pinMode (D0pin, INPUT);
        pinMode (Buzz,  OUTPUT);

        status = 0;
        count = 0;
        while(1) // loop forever
        {
                analogVal = analogRead(PCF+0);
                printf("analogVal : %d\n", analogVal);

                tmp = digitalRead(D0pin);
                printf("status : %d , tmp : %d\n",status,tmp);
                printf("count : %d\n",count);
                if (tmp != status)
                {
                        Print(tmp);
                        if(tmp==0) /*tmp = 0 is WARNING DETECTION */
                         {
                                 shot();
                                 ledon();
                                 warningsocket(sockfd);

                         }
                        else if(tmp==1) /* tmp = 1 is SAFE */
                        {
                                ledoff();
                                oksocket(sockfd);
                        }
                        status = tmp;
                }

                if (status == 0)
                {
                        count ++;
                }
                else
                {
                        count = 0;
                }
                delay (2000);
        }
        close(sockfd);
        return 0;
}

