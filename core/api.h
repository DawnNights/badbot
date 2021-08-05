#include "utils.h"

int Api_SendPrivateMsg(int *ws, long long userID, char *msgText)
{
    cJSON *obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "user_id", userID);
    cJSON_AddStringToObject(obj, "message", msgText);
    obj = callApi("send_private_msg", obj);
    WsSend(ws, cJSON_Print(obj));

    obj = parseJson(WsRecv(ws), "data.message_id");
    return obj->valueint;
}

int Api_SendGroupMsg(int *ws, long long GroupID, char *msgText)
{
    cJSON *obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "group_id", GroupID);
    cJSON_AddStringToObject(obj, "message", msgText);
    obj = callApi("send_group_msg", obj);
    WsSend(ws, cJSON_Print(obj));

    obj = parseJson(WsRecv(ws), "data.message_id");
    return obj->valueint;
}

