#ifndef __PARSE_HTTP_RESPONSE__
#define __PARSE_HTTP_RESPONSE__

#include "Common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
void parseDataFromHttpPostResponse(void *args, void* data, size_t len) {
    if (!args) return;
    TaskContext *taskContext = (TaskContext *)args;

    char * p1, *p2;
    char * ch = (char*)data;
    ch[len] = '\0';
    Logger.Info("Response --> \n%s", ch);
    
    p1 = strstr (ch, "{\"Status\":\"Success\"");   /* check for success response (return address of string occurrence) */
    p2 = strstr (ch, "\"Status\": \"Success\"");
    int position = p1 - ch;
    Logger.Info("Position: %d, p1: %d, p2: %d, len: %d", position, (int)p1, (int)p2, len);
    if (position >= 0) {
        char st[len - position];
        strncpy(st, p1, (len - position));
        st[len - position] = '\0';
        Logger.Info("Response Body --> %s", st);
        if (p1 || p2) {
           Logger.Info("Success...");
           finishTask((void*)taskContext);
           retry_count = 0;
           return;
        }
    }

    abortTask((void*)taskContext);

    Logger.Info("******************************");
    if (retry_count < MAX_RETRY) {
        retry_count++;
        Logger.Info("Status Upload, Retry %d/%d!", retry_count, MAX_RETRY);
        
    } else {
        Logger.Info("Status Upload, Retry Aborted!");
        retry_count = 0;
        gUpdateCloud = false;
        isUpdateAborted = false;
    }
    Logger.Info("******************************");
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
