#include "rpc.h"

int main() {
    key_t key = ftok(server_path, server_projid);
    if(key == -1) handle_error("ftok() ");

    int msqid = msgget(key, 0666 | IPC_CREAT);
    if(msqid == -1) handle_error("msgget");

    while(1) {
        struct mymsg msg;
        msg.type = 2;

        if (msgrcv(msqid, &msg, sizeof(struct mymsg), 2, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        if(strcmp(msg.msg.function_name, "add") == 0) {
            printf("recieved add request packet from server stub\n");
            msg.msg.return_val = htons((msg.msg.arguments.arg1) + (msg.msg.arguments.arg2));
        } else if(strcmp(msg.msg.function_name, "subtract") == 0) {
            printf("recieved subtract request packet from server stub\n");
            msg.msg.return_val = htons((msg.msg.arguments.arg1) - (msg.msg.arguments.arg2));
        } else {
            printf("invalid function name\n");
            msg.msg.return_val = -1;
        }

        printf("result = %d\n", ntohs(msg.msg.return_val));
        msg.msg.opcode = htons(2); //reply
        msg.type = 3;
        if (msgsnd(msqid, &msg, sizeof(struct rpc_hdr), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
        printf("Sent reply packet to server stub\n");
    }
    return 0;
}