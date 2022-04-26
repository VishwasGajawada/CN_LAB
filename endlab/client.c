#include "rpc.h"

int msqid;

void add(int a, int b){
    // wrap inside the rpc header and send it to client_stub using message queue IPC
    struct mymsg msg;
    msg.type = 1;

    /* msg.msg is rpc_hdr */
    strcpy(msg.msg.function_name, "add");
    msg.msg.opcode = htons(1);
    /* msg.msg.arguments is rpc_args */
    msg.msg.arguments.arg1 = htons(a);
    msg.msg.arguments.arg2 = htons(b);

    if (msgsnd(msqid, &msg, sizeof(struct rpc_hdr), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    /* wait for reply */
    int result;
    if (msgrcv(msqid, &msg, sizeof(struct rpc_hdr), 4, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }
    result = (msg.msg.return_val);

    printf("%d + %d = %d\n\n", a, b, result);
}

void subtract(int a, int b) {
    // wrap inside the rpc header and send it to client_stub using message queue IPC
    struct mymsg msg;
    msg.type = 1;

    /* msg.msg is rpc_hdr */
    strcpy(msg.msg.function_name, "subtract");
    msg.msg.opcode = htons(1);
    /* msg.msg.arguments is rpc_args */
    msg.msg.arguments.arg1 = htons(a);
    msg.msg.arguments.arg2 = htons(b);

    if (msgsnd(msqid, &msg, sizeof(struct rpc_hdr), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    /* wait for reply */
    int result;
    if (msgrcv(msqid, &msg, sizeof(struct rpc_hdr), 4, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }
    result = (msg.msg.return_val);

    printf("%d - %d = %d\n\n", a, b, result);
}

int main() {
    key_t key = ftok(client_path, client_projid);
    if(key == -1) handle_error("ftok() ");

    msqid = msgget(key, 0666 | IPC_CREAT);
    if(msqid == -1) handle_error("msgget");

    while(1) {
        printf("choose : 1-Add, 2-Sub\n");
        int opt;
        scanf("%d", &opt);
        int arg1, arg2;
        printf("Enter the two input numbers : ");
        scanf("%d", &arg1);
        scanf("%d", &arg2);

        if(opt == 1) add(arg1, arg2);
        else if(opt == 2) subtract(arg1, arg2);
        else {
            printf("Invalid option\n");
            continue;
        }
    }   
    return 0;
}