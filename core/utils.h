#include "wsClient\wsClient.h"

// 
cJSON * callApi(char *action, cJSON *params)
{
    cJSON *obj = cJSON_Parse("{\"echo\": 1}");
    cJSON_AddStringToObject(obj, "action", action);
    cJSON_AddItemToObject(obj, "params", params);
    return obj;
}

cJSON * parseJson(cJSON *root, char *path)
{
    char *key;
    char paths[200];
    cJSON *obj = root;

    memcpy(paths, path, strlen(path));
    paths[strlen(path)] = '\0';

    key = strtok(paths, "."); 
    while (key != NULL)
    {
        if (strspn(key, "0123456789") == strlen(key))
        {
            int idx;
            sscanf(key, "%d", &idx);
            obj = cJSON_GetArrayItem(obj, idx);
        }else{
            obj = cJSON_GetObjectItem(obj, key);
        }

        if (obj == NULL)
        {
            printf("\"%s\" is a wrong key\n", key);
            return NULL;
        }
        key = strtok(NULL, ".");
    }
    return obj;
}