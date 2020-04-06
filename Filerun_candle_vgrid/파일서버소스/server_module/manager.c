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

/*�޴���������*/
#define UP                                                                                                      "UP"
#define DOWN                                                                                    "DOWN"
#define CNT                                                                                             "CNT"
#define AVG                                                                                             "AVG"
#define SIZE                                                                                            "SIZE"
#define EXIT                                                                                            "EXIT"
#define DIV                                                                                                     "@"


#define UPLOAD                                                                                          "upload"

/*������������*/
#define R_NOKEY                                                                         "10000"
#define R_ERROR                                                                         "20000"
#define R_QUIT                                                                                  "90000"

/*����*/
#define LOG_FILE                                                                                "/tmp/manager.log"

/*���� ��Ʈ*/
#define S_PORT                                                                                  7100

/*�Լ�����*/
//���ϰ���
int                                             AcceptTCPConnection(int servSock);
int                                             CreateTCPServerSocket(unsigned short port);

//���������
void*                                   ThreadMain(void * threadArgs);
void                                            ThreadExit(void * args);

//������ã��
void                                            DiskSizeProc(void * args);
void                                            LoadAvgProc(void * args);
void                                            UpProc(void * args);
void                                            DownProc(void * args);

//�αװ���
void                                            FileLog(char* log);
/*��������*/
struct ThreadArgs
{
        int clntSock;
        char* order;
        char* value;
};

//��������
int main(int argc, char * argv[])
{
        int servSock;
        int clntSock;
        pthread_t threadID;
        struct ThreadArgs * threadArgs;
        void *(*pThreadMain)(void *);
        pThreadMain = ThreadMain;

        //�������ϻ���
        servSock = CreateTCPServerSocket(S_PORT);

        //���۷α� ����
        FileLog("Start OK");

        //���ξ�����(Ŭ���̿�Ʈ���Ͻ� ����)
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

//���Ͼ�����
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
                        ThreadExit((void *)threadArgs);    // ���ϸ޼�������
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


//���ε� ������
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

//�ٿ�ε� ������
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

//��ũ�뷮üũ
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

//��ũ �ε忡������ ��հ�
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

//����������
void ThreadExit(void * args)
{
        struct ThreadArgs * threadArgs = (struct ThreadArgs *) args;
        write(threadArgs->clntSock,R_QUIT,sizeof(R_QUIT));
        close(threadArgs->clntSock);
		bzero(threadArgs,sizeof(threadArgs));
        pthread_exit(NULL);
}

//�������� 
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

//Ŭ���̿�Ʈ���� 
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
        
//�α����ϻ���
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
