#include "cJSON.h"
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>

// 因为websocket基于http，所以先创建一个tcp套接字
int createTcpSocket(int *sock)
{
    // 初始化WinSock服务，否则将无法进行socket连接
    WSADATA data;
    WORD sockVersion = MAKEWORD(2,2);
    WSAStartup(sockVersion, &data);

    *sock = socket(AF_INET, SOCK_STREAM, 0);
    return *sock != 0;
}

// 发出连接请求
int connAddress(int *sock, const char *host, u_short port)
{
    struct sockaddr_in netAddr;
    memset(&netAddr, 0, sizeof(struct sockaddr_in));
    netAddr.sin_family = AF_INET;
    netAddr.sin_addr.s_addr = inet_addr(host);
	netAddr.sin_port = htons(port);

    return connect(*sock, (struct sockaddr *)&netAddr, sizeof(struct sockaddr)) == 0;
}

// 发起websocket握手请求
int handShake(int *sock)
{
    char request[] = "GET /ws HTTP/1.1\n\
Host: 127.0.0.1:6700\n\
Connection: Upgrade\n\
Upgrade: websocket\n\
Sec-WebSocket-Version: 13\n\
Sec-WebSocket-Key: Bt4+Nfq12qxyxHslV2iFFg==\n\n";
    send(*sock, request, strlen(request), 0);

    char response[1024*2];
    return recv(*sock, response, 1024*2, 0) != 0;
}

// Ws客户端初始化
int WsInit(char *WS_URL)
{
    int ip[4];
    int port;
    char host[20];
    sscanf(WS_URL, "ws://%d.%d.%d.%d:%d/ws", &ip[0], &ip[1], &ip[2], &ip[3], &port);
    sprintf(host, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    int ws;

    if (!createTcpSocket(&ws))
    {
        printf("Warn: socket服务创建失败\n");
    }
    printf("Info: socket服务创建成功\n");
    
    if (!connAddress(&ws, host, port))
    {
        printf("Warn: %s连接失败\n", WS_URL);
    }
    printf("Info: %s连接成功\n", WS_URL);

    if (!handShake(&ws))
    {
        printf("Warn: 握手失败，WebSocket初始化失败\n\n");
    }
    printf("Info: 握手成功，WebSocket初始化成功\n\n");

    return ws;
}

// Ws发送数据(OPCODE_TEXT)
void WsSend(int *ws, char sendData[])
{
    short dataLen = strlen(sendData);
    char realData[dataLen+8];
    realData[0] = 1<<7;
    realData[0] = realData[0] | 0<<6;
    realData[0] = realData[0] | 0<<5;
    realData[0] = realData[0] | 0<<4;
    realData[0] = realData[0] | 1;
    realData[1] = 1<<7;
    realData[1] = realData[1] | 126; 
    
    char temps[2];
    memcpy(temps, (char*)&dataLen, sizeof(short));
    realData[2] = temps[1];
    realData[3] = temps[0];

    realData[4] = 1;
    realData[5] = 1;
    realData[6] = 1;
    realData[7] = 1;

    for (size_t i = 0; i < dataLen; i++)
    {
        realData[i+8] = sendData[i] ^ 1;
    }
    send(*ws, realData, dataLen+8, 0);
}

// Ws接收数据(OPCODE_TEXT)并解析为CJSON
cJSON * WsRecv(int *ws)
{
    char src[1024*5];
    int srcSize = recv(*ws, src, 1024*5, 0);
    int idx = strchr(src, '{') - src;
    char dest[srcSize-idx];
    memcpy(dest, src+idx, srcSize-idx);
    return cJSON_Parse(dest);
}