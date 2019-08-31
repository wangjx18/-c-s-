#include <stdio.h>
#include <iostream>
#include <Winsock2.h>
#include <pthread.h>
using namespace std;
struct thread_data
{
    SOCKET sockConn;
    SOCKADDR_IN addrClient;
    int len;
};
void* fun(void *m);
int main()
{
    WORD wVersionRequested;//不是C++标准的类型，是微软SDK中的类型，WORD的意思为字，是2byte的无符号整数，表示范围0~65535.相当于c语言中2个char的范围；
    WSADATA wsaData;//这个结构被用来存储被WSAStartup函数调用后返回的Windows Sockets数据。
    int err;
    wVersionRequested = MAKEWORD( 1, 1);//makeword是将两个byte型合并成一个word型，一个在高8位(b)，一个在低8位(a) socket中适用于设置声明调用不同winsock版本，（1，1）是1.1版（2，2）是2.2版
    err = WSAStartup( wVersionRequested, &wsaData );//此函数在应用程序中初始化winsockDLL，只有此函数调用成功后，应用程序才可以调用Windows SocketsDLL中的其他API函数，否则后面的任何函数都将调用失败。
    if ( err != 0 )
    {
        return -1;//如果成功，WSAStartup函数返回0。否则，返回下面列表显示的错误码之一。
    }
    if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 )//接收的socket版本号不是1.1
    {
        cout<<wsaData.wVersion <<endl;
        WSACleanup( );//应用程序在完成对请求的Socket库的使用后，要调用WSACleanup函数来解除与Socket库的绑定并且释放Socket库所占用的系统资源。
        printf("banbenbudui\n");
        char a;
        a=scanf("%c",&a);
        return -1;
    }
    SOCKET sockSrv=socket(AF_INET,//socket（）是一个函数，创建一个套接字，AF_INET 表示用IPV4地址族，
                          SOCK_STREAM,//SOCK_STREAM 是说是要是用流式套接字
                          0);//0 是指不指定协议类型，系统自动根据情况指定，此时是tcp协议
    SOCKADDR_IN addrSrv;//ockaddr_in是系统封装的一个结构体，具体包含了成员变量：sin_family、sin_addr、sin_zero
    /*typedef struct sockaddr_in
    {

    #if(_WIN32_WINNT < 0x0600)
        short   sin_family;//定义是哪种地址族
    #else //(_WIN32_WINNT < 0x0600)
        ADDRESS_FAMILY sin_family;
    #endif //(_WIN32_WINNT < 0x0600)

        USHORT sin_port;//主要用来保存端口号
        IN_ADDR sin_addr;//主要用来保存IP地址信息
        CHAR sin_zero[8];//没有特殊含义
    } SOCKADDR_IN, *PSOCKADDR_IN;
    */
    addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//设置地址为本地
    addrSrv.sin_family=AF_INET;//设置地址类型为ipv4
    addrSrv.sin_port=htons(6000);//htons将数字转换为网络格式，网络字节顺序NBO（Network Byte Order），主机字节顺序（HBO，Host Byte Order）：
    bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));//绑定套接字，bind函数适配器
    listen(sockSrv,5);//监听套接字，设置最大监听数5
    SOCKADDR_IN addrClient;
    int len=sizeof(SOCKADDR);//len=16?
    printf("%d\n",len);
    int rc;
    pthread_t threads[5];
    int i=0;
    while(1)
    {
        SOCKET sockConn=::accept(sockSrv,(SOCKADDR*)&addrClient,&len);
        struct thread_data *c=new (struct thread_data);
        (*c).addrClient=addrClient;
        (*c).sockConn=sockConn;
        (*c).len=len;
        void  *l=(void*)c;
        rc = pthread_create(&threads[i++], NULL,
                            fun,
                            l);
        if (rc)
        {
            cout << "Error:无法创建线程," << rc << endl;
            exit(-1);
        }
    }
}
void* fun(void *m)
{
    struct thread_data *c;
    c=(struct thread_data *)m;
    SOCKET sockConn=(*c).sockConn;
    SOCKADDR_IN addrClient=(*c).addrClient;
    int len=(*c).len;
    char recvBuf[50];
    strcpy(recvBuf, "");
    char *strIp=inet_ntoa(addrClient.sin_addr);         //把获取的IP转换为主机字节序
    int strPort=ntohs(addrClient.sin_port);         //把获取的端口转换为主机字节序
    cout<<strIp<<"加入"<<endl;
    while(strcmp(recvBuf,"quit")!=0)
    {
        char sendBuf[50];
        sprintf(sendBuf,"Welcome %s to here!",inet_ntoa(addrClient.sin_addr));
        send(sockConn,sendBuf,strlen(sendBuf)+1,0);
        recv(sockConn,recvBuf,50,0);
        getpeername(sockConn, (SOCKADDR*)&addrClient,&len);
        cout<<strIp;
        printf("-%d:%s\n",strPort,recvBuf);
        strcpy(recvBuf,"");
    }
    cout<<strIp<<"已离开"<<endl;
    closesocket(sockConn);
    delete c;
    pthread_exit(NULL);
}
