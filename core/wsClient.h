#include "cJSON.h"
#include <stdio.h>
#include <WinSock2.h>

// 初始化WebSocket客户端
int WsInit(int *ws, const char *host, u_short port)
{
    // 初始化WinSock服务，否则将无法进行socket连接
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA data;
    WSAStartup(sockVersion, &data);

    // websocket基于http，所以我们先创建一个tcp协议的套接字
    *ws = socket(AF_INET, SOCK_STREAM, 0);
    if (*ws == 0)
    {
        printf("Warn: socket服务创建失败\n");
        return 0;
    }
    printf("Info: socket服务创建成功\n");

    // onebot中正向ws的默认连接地址为127.0.0.1:6700
    struct sockaddr_in netAddr;
    memset(&netAddr, 0, sizeof(struct sockaddr_in));
    netAddr.sin_family = AF_INET;
    netAddr.sin_addr.s_addr = inet_addr(host);
	netAddr.sin_port = htons(port);
	
    if (connect(*ws, (struct sockaddr *)&netAddr, sizeof(struct sockaddr)) != 0)
    {
        printf("Warn: %s:%u连接失败\n", host, port);
        return 0;
    }
    printf("Info: %s:%u连接成功\n", host, port);

    // 发送基于http的握手请求
    char header[] = "GET /ws HTTP/1.1\n\
Host: 127.0.0.1:6700\n\
Connection: Upgrade\n\
Upgrade: websocket\n\
Sec-WebSocket-Version: 13\n\
Sec-WebSocket-Key: Bt4+Nfq12qxyxHslV2iFFg==\n\n";
    send(*ws, header, strlen(header), 0);

    char recvData[2048];
    if (recv(*ws, recvData, strlen(recvData), 0) != 0)
    {
        printf("Warn: 握手失败，WebSocket客户端创建失败\n");
        return 0;
    }
    recv(*ws, recvData, 2048, 0);   // 握手返回数据
    printf("Info: 握手成功，WebSocket客户端创建成功\n");

    return 1;
}

// WebSocket发送数据
int WsSend(int *ws, char sendData[])
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
    return send(*ws, realData, dataLen+8, 0);
    
}

// WebSocket接收数据并返回cJSON指针
cJSON * WsRecv(int *ws)
{
    char buf[1024*5];
    int bufSize = recv(*ws, buf, 1024*5, 0);
    int idx = strchr(buf, '{') - buf;
    char dest[bufSize-idx];
    memcpy(dest, buf+idx, bufSize-idx);
    // printf("%s\n", dest);
    return cJSON_Parse(dest);
}

// 获取json数据中key对应的字符串
char * getJsonStr(cJSON *root, const char *key)
{
    cJSON* item = cJSON_GetObjectItem(root,key);
    if (item==NULL){return "";}
    return item->valuestring;
}

// 获取json数据中key对应的整数
long long getJsonInt(cJSON *root, const char *key)
{
    cJSON* item = cJSON_GetObjectItem(root,key);
    if (item==NULL){return 0;}
    return item->valuedouble;
}

// 获取json数据中key对应的Json数据
cJSON * getJsonJson(cJSON *root, const char *key)
{
    cJSON* item = cJSON_GetObjectItem(root,key);
    if (item==NULL){return NULL;}
    return item;
}