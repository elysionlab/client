#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*받는프로토콜*/
#define UP                                                                                                      "UP"
#define DOWN                                                                                    "DOWN"
#define CNT                                                                                             "CNT"
#define AVG                                                                                             "AVG"
#define SIZE                                                                                            "SIZE"
#define EXIT                                                                                            "EXIT"
#define DIV                                                                                                     "@"


#define UPLOAD                                                                                          "upload"

/*전송프로토콜*/
#define R_NOKEY                                                                         "10000"
#define R_ERROR                                                                         "20000"
#define R_QUIT                                                                                  "90000"

/*파일*/
#define LOG_FILE                                                                                "/tmp/manager.log"

/*서버 포트*/
#define S_PORT                                                                                  7100

/*함수선언*/
//소켓관련
int                                             AcceptTCPConnection(int servSock);
int                                             CreateTCPServerSocket(unsigned short port);

//쓰레드관련
void*                                   ThreadMain(void * threadArgs);
void                                            ThreadExit(void * args);

//정보및찾기
void                                            DiskSizeProc(void * args);
void                                            LoadAvgProc(void * args);
void                                            UpProc(void * args);
void                                            DownProc(void * args);

//로그관리
void                                            FileLog(char* log);
/*소켓정보*/
struct ThreadArgs
{
        int clntSock;
        char* order;
        char* value;
};

//서버메인
int main(int argc, char * argv[])
{
        int servSock;
        int clntSock;
        pthread_t threadID;
        struct ThreadArgs * threadArgs;
        void *(*pThreadMain)(void *);
        pThreadMain = ThreadMain;

        //서버소켓생성
        servSock = CreateTCPServerSocket(S_PORT);

        //시작로그 생성
        FileLog("Start OK");

        //메인쓰레드(클라이엔트소켓시 생성)
        while(1) {
                clntSock = AcceptTCPConnection(servSock);
                if((threadArgs = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL) {
                        FileLog("malloc() failed");
                }
                threadArgs->clntSock = clntSock;
                if(pthread_create(&threadID, NULL, pThreadMain, (void *)threadArgs) != 0) {
                        FileLog("sock_pthread_create() failed");
                }
        }
        return 0;
}

//소켓쓰레드
void *ThreadMain(void * args)
{
        char msg[1024] = "";
        char* token;
        char* arr_msg[256];
        int i=0;
        struct ThreadArgs * threadArgs = (struct ThreadArgs *) args;
        pthread_detach(pthread_self());

        while(1){
                if(!read(threadArgs->clntSock,msg,sizeof(msg))) {
                        ThreadExit((void *)threadArgs);    // 소켓메세지읽음
                }

                if(strncmp(msg,"\r\n",sizeof("\r\n"))) {
                        i=0;
						arr_msg[i] = token = strtok(msg,DIV);
                        while (i++, token) {
                                arr_msg[i] = token = strtok(0,DIV);
                        }
                    
                        threadArgs->order=arr_msg[0];
                        if(arr_msg[1] !=NULL) {
                                threadArgs->value=arr_msg[1];
                        } else {
                                write(threadArgs->clntSock,R_ERROR,sizeof(R_ERROR));
                        }
        
                        if(strncmp(threadArgs->order,SIZE,sizeof(SIZE))==0) {
                                DiskSizeProc((void *)threadArgs);
                        } else if(strncmp(threadArgs->order,AVG,sizeof(AVG))==0) {
                                LoadAvgProc((void *)threadArgs);
                        } else if(strncmp(threadArgs->order,CNT,sizeof(CNT))==0) {
                                if(strncmp(threadArgs->value,UPLOAD,sizeof(UPLOAD))==0) UpProc((void *)threadArgs);
                                else DownProc((void *)threadArgs);
                        } else if(strncmp(threadArgs->order,UP,sizeof(UP))==0) {
                                UpProc((void *)threadArgs);
                        } else if(strncmp(threadArgs->order,DOWN,sizeof(DOWN))==0) {
                                DownProc((void *)threadArgs);
                        } else if(strncmp(threadArgs->order,EXIT,sizeof(EXIT))==0) {
                                ThreadExit((void *)threadArgs);
                        } else {
                                write(threadArgs->clntSock,R_ERROR,sizeof(R_ERROR));
                        }
                        
                } else {
                        write(threadArgs->clntSock,R_NOKEY,sizeof(R_NOKEY));
                }
                        
                bzero(msg,sizeof(msg));
                bzero(arr_msg,sizeof(arr_msg));
        }
}


//업로드 접속자
void UpProc(void * args)
{       
        char                    order[128]="";
        char                    send[10]="";
        FILE*           p;
        struct ThreadArgs * threadArgs = (struct ThreadArgs *) args;
        
        sprintf(order,"ftpwho | awk \'/STOR/ {print $2}\' | grep -c \'%s\'",threadArgs->value);
        p=popen(order,"r"); while(fgets(send, sizeof(send), p) != NULL) ; pclose(p);
        write(threadArgs->clntSock,send,sizeof(send));
		bzero(order,sizeof(order));
        bzero(send,sizeof(send));
}

//다운로드 접속자
void DownProc(void * args)
{
        char                    order[128]="";
        char                    send[10]="";
        FILE*           p;
        struct ThreadArgs * threadArgs = (struct ThreadArgs *) args;

        sprintf(order,"ftpwho | awk \'/RETR/ {print $2}\' | grep -c \'%s\'",threadArgs->value);
        p=popen(order,"r"); while(fgets(send, sizeof(send), p) != NULL) ; pclose(p);
        write(threadArgs->clntSock,send,sizeof(send));
        bzero(order,sizeof(order));
        bzero(send,sizeof(send));
}

//디스크용량체크
void DiskSizeProc(void * args)
{
        char                    order[128]="";
        char                    send[24]="";
        FILE*           p;
        struct ThreadArgs * threadArgs = (struct ThreadArgs *) args;
//printf("threadArgs->value:%s\n", threadArgs->value);
        sprintf(order,"df -m %s | grep '$*/' | awk \'{ print $4}\'",threadArgs->value,threadArgs->value);
//printf("order:%s\n", order);
        p=popen(order,"r"); while(fgets(send, sizeof(send), p) != NULL) ; pclose(p);
//printf("send:%s\n", send);
        write(threadArgs->clntSock,send,sizeof(send));
        bzero(order,sizeof(order));
        bzero(send,sizeof(send));
}

//디스크 로드에버러지 평균값
void LoadAvgProc(void * args)
{
        char                    order[128]="cat /proc/loadavg | awk \'{print $1}\'";
        char                    send[10]="";
        FILE*           p;
        struct ThreadArgs * threadArgs = (struct ThreadArgs *) args;
        p=popen(order,"r"); while(fgets(send, sizeof(send), p) != NULL) ; pclose(p);
        write(threadArgs->clntSock,send,sizeof(send));
        bzero(order,sizeof(order));
        bzero(send,sizeof(send));
}

//쓰레드종료
void ThreadExit(void * args)
{
        struct ThreadArgs * threadArgs = (struct ThreadArgs *) args;
        write(threadArgs->clntSock,R_QUIT,sizeof(R_QUIT));
        close(threadArgs->clntSock);
		bzero(threadArgs,sizeof(threadArgs));
        pthread_exit(NULL);
}

//서버소켓 
int CreateTCPServerSocket(unsigned short port)
{
        int sock;               
        struct sockaddr_in servAddr;
        
        if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
                FileLog("socket() failed");
                return 0;
        }
        
        bzero(&servAddr,sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(port);

        if(bind(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
        {
                FileLog("bind() failed");
                return 0;
        }

        if(listen(sock, 5) < 0) 
        {
                FileLog("listen() failed");
                return 0;
        }
        
        return sock;
}

//클라이엔트소켓 
int AcceptTCPConnection(int servSock)
{
        int                                     clntSock;
        struct                          sockaddr_in clntAddr;
        unsigned int            clntLen;
        
        clntLen = sizeof(clntAddr);
        
        if((clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntLen)) < 0)
        {
                FileLog("accept() failed");
                return 0;
        } 

        return clntSock;
}       
        
//로그파일생성
void FileLog(char* log)
{
        char                                    log_name[256];
        struct tm                       *timeStr;
        time_t                          aclock;
        FILE*                           fp;
        time(&aclock);
        timeStr = localtime(&aclock);
        asctime(timeStr);
        sprintf(log_name,"%s",LOG_FILE);
        if((fp=fopen(log_name, "a+"))==NULL) {
                FileLog("Log File Open Error");
        }

        fprintf(fp,"%d%02d%02d %02d:%02d:%02d => %s \n",
                timeStr->tm_year+1900,timeStr->tm_mon+1,timeStr->tm_mday,timeStr->tm_hour,timeStr->tm_min,timeStr->tm_sec,log);
        fclose(fp);
}
