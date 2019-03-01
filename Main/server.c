#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>

#define HIGH 1
#define LOW 0
#define INPUT 1
#define OUTPUT 0
#define MYPORT 3490
#define BACKLOG 10
#define MAXDATASIZE 100

int outputPin = 135; // Use GPIO pin 13

bool digitalPinMode(int pin, int dir)
{
   FILE * fd;
   char fName[128];

   // Exporting the pin to be used
   if(( fd = fopen("/sys/class/gpio/export", "w")) == NULL){
      printf("Error: unable to export pin\n");
      return false;
   }
   fprintf(fd, "%d\n", pin);
   fclose(fd);

   // Setting direction of the pin
   sprintf(fName, "/sys/class/gpio/gpio%d/direction", pin);
   if((fd = fopen(fName, "w")) == NULL){
      printf("Error: can't open pin direction\n");
      return false;
   }
   if(dir == OUTPUT) { fprintf(fd, "out\n"); }
   else {fprintf(fd, "in\n"); }

   fclose(fd);
   return true;
}

bool digitalWrite(int pin, int val)
{
   FILE * fd;
   char fName[128];

   // Open pin value file
   sprintf(fName, "/sys/class/gpio/gpio%d/value", pin);
   if((fd = fopen(fName, "w")) == NULL)
   {
      printf("Error: can't open pin value\n");
      return false;
   }
   if(val == HIGH) {fprintf(fd, "1\n"); }
   else {fprintf(fd, "0\n"); }

   fclose(fd);
   return true;
}

int setup()
{
   if (!digitalPinMode(outputPin, OUTPUT)) return -1;

   return 0;
}

void DecisionData(char buf[], struct tm *t1)
{
  int fdin;
   if( 0 < (fdin = open("Data.txt",O_WRONLY))) // Write the Data
    {}
   lseek(fdin,0,SEEK_END);
   write(fdin,buf,strlen(buf)); // write status
   sprintf(buf, " time : %d월 %d일 %d시 %d분 %d초\n",t1->tm_mon+1,t1->tm_mday, t1->tm_hour+14, t1->tm_min, t1->tm_sec);
   lseek(fdin,0,SEEK_END);
   write(fdin,buf,strlen(buf)); // write time
   close(fdin);
}

int main() {
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    ssize_t read;
    int sin_size;
    int numbytes;
    int on = 1;
    char buf[MAXDATASIZE];
    char *recv_data;
    int fdin;
    time_t timer;
    struct tm *t1;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero), 8);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("========= [PORT] : %d =========\n", MYPORT);
    printf("Server : waiting client\n");

    system("rm Data.txt");
    system("touch Data.txt");
    
    while(1) {
        // wait until connecting with client
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,&sin_size)) == -1) {
            perror("accept");
            continue;
        }

        if (!fork()) {
            while(1){
                if((numbytes=recv(new_fd, buf, MAXDATASIZE, 0))==-1) {
                  perror("recv");
                  exit(1);
                }
                buf[numbytes] = '\0';
                timer = time(NULL);
                t1 = localtime(&timer);
                printf("recv=%s\n", buf);
                if(!strcmp(buf,"ISSAFE")){ // If is safety area
                        digitalWrite(outputPin,LOW);
                        system("node send.js 0");
                }
                else if(!strcmp(buf,"WARNING DETECTION")){ // If smoke is Detected
                        digitalWrite(outputPin,HIGH);
                        system("node send.js 1");
                }
                else
                        continue; // When Data is loss, do it Again
              DecisionData(buf,t1);
            }

            close(new_fd);
            while(waitpid(-1,NULL,WNOHANG) > 0);
        }
    }

    return 0;
}
