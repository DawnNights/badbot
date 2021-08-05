#include "core\api.h"

int main()
{
    system("chcp 65001");
    int ws = WsInit("ws://127.0.0.1:6700/ws");
    WsRecv(&ws);    // 忽略心跳事件

    cJSON *msg;
    char *msgText,*nickName;
    long long userId,groupId;

    while (1)
    {
        msg = WsRecv(&ws);
        
        if (strcmp(parseJson(msg, "message_type")->valuestring, "private") == 0)
        {
            userId = parseJson(msg, "sender.user_id")->valuedouble;
            nickName = parseJson(msg, "sender.nickname")->valuestring;
            msgText = parseJson(msg, "message")->valuestring;

            printf("收到用户 %s(%u) 的消息: %s\n", nickName, userId, msgText);
            Api_SendPrivateMsg(&ws, userId, msgText);
        }else if (strcmp(parseJson(msg, "message_type")->valuestring, "group") == 0)
        {
            userId = parseJson(msg, "sender.user_id")->valuedouble;
            nickName = parseJson(msg, "sender.nickname")->valuestring;
            groupId = parseJson(msg, "group_id")->valuedouble;
            msgText = parseJson(msg, "message")->valuestring;

            printf("收到 群(%u)内用户 %s(%u) 的消息: %s\n", groupId, nickName, userId, msgText);
            Api_SendGroupMsg(&ws, groupId, msgText);
        }
        
        
    }
    
    return 0;
}