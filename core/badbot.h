#include "wsClient.h"

struct message
{
    long long fromQQ;
    long long fromGroup;
    int msgType;
    char *msgText;
};


int * botCreate()
{
    static int ws;
    WsInit(&ws, "127.0.0.1", 6700);
    return &ws;
}

void recvMsg(int *bot,struct message *msg)
{

    cJSON *root = WsRecv(bot);
    char *msgType = getJsonStr(root, "message_type");
    if (msgType=="")
    {
        msg->msgType = -1;
        return;
    }

    if (strcmp(msgType,"private") == 0)
    {
        msg->msgType = 0;
        msg->fromQQ = getJsonInt(root, "user_id");
        msg->fromGroup = 0;
        msg->msgText = getJsonStr(root, "message");

        char * subType = getJsonStr(root, "sub_type");
        char * fromName = getJsonStr(getJsonJson(root, "sender"), "nickname");
        
        if (strcmp(subType,"friend") == 0)
        {
            printf("Info: 收到好友%s(%u)消息: %s\n", fromName, msg->fromQQ, msg->msgText);
        }else
        {
            printf("Info: 收到%s(%u)的临时消息: %s\n", fromName, msg->fromQQ, msg->msgText);
        }
    }else if (strcmp(msgType,"group") == 0)
    {
        msg->msgType = 1;
        msg->fromQQ = getJsonInt(root, "user_id");
        msg->fromGroup = getJsonInt(root, "group_id");
        msg->msgText = getJsonStr(root, "message");

        char * fromName = getJsonStr(getJsonJson(root, "sender"), "nickname");
        printf("Info: 收到群(%u)的%s(%u)消息: %s\n", msg->fromGroup, fromName, msg->fromQQ, msg->msgText);
    }else{
        msg->msgType = -1;
    }
    cJSON_Delete(root);    
}

void send_private_msg(int *bot, long long userID, char *msgText)
{
    const char mould[] = "{\"action\": \"send_private_msg\", \"params\": {\"user_id\": %d, \"message\": \"%s\"}, \"echo\": 1}";
    char data[1024];
    sprintf(data, mould, userID, msgText);
    puts(data);
    WsSend(bot, data);
}

void send_group_msg(int *bot, long long groupID, char *msgText)
{
    const char mould[] = "{\"action\": \"send_group_msg\", \"params\": {\"group_id\": %d, \"message\": \"%s\"}, \"echo\": 1}";
    char data[1024];
    sprintf(data, mould, groupID, msgText);
    puts(data);
    WsSend(bot, data);
}

