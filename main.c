#include "core\badbot.h"

int main()
{
    system("chcp 65001");   // 防止windows控制台输出中文乱码
    int *bot = botCreate();

    struct message msg;
    while (1)
    {
        recvMsg(bot, &msg);
        if (msg.msgType==-1)
        {
            continue;
        }else if (msg.msgType==0)
        {
            send_private_msg(bot, msg.fromQQ, msg.msgText);
        }else if (msg.msgType==1)
        {
            send_group_msg(bot, msg.fromGroup, msg.msgText);
        }  
        
    }  
    return 0;
}
