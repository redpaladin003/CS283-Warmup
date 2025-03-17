#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>
#include "dshlib.h"
#include "rshlib.h"

/*
 * exec_remote_cmd_loop() - no changes needed for the local pipe test,
 * but we keep it in case we do remote connections.
 */
int exec_remote_cmd_loop(char *address, int port) {
    char *cmd_buff = malloc(RDSH_COMM_BUFF_SZ);
    char *rsp_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (!cmd_buff || !rsp_buff) {
        perror("malloc");
        return client_cleanup(-1, cmd_buff, rsp_buff, ERR_MEMORY);
    }

    int cli_socket = start_client(address, port);
    if (cli_socket < 0) {
        perror("start client");
        return client_cleanup(cli_socket, cmd_buff, rsp_buff, ERR_RDSH_CLIENT);
    }

    while (1) {
        // Same prompt as local
        printf("%s", SH_PROMPT);

        if (!fgets(cmd_buff, RDSH_COMM_BUFF_SZ, stdin)) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        ssize_t send_cmd = send(cli_socket, cmd_buff, strlen(cmd_buff) + 1, 0);
        if (send_cmd < 0) {
            perror("send");
            return client_cleanup(cli_socket, cmd_buff, rsp_buff, ERR_RDSH_COMMUNICATION);
        }

        // Receive until EOF char
        while (1) {
            ssize_t rbytes = recv(cli_socket, rsp_buff, RDSH_COMM_BUFF_SZ, 0);
            if (rbytes < 0) {
                perror("recv");
                return client_cleanup(cli_socket, cmd_buff, rsp_buff, ERR_RDSH_COMMUNICATION);
            }
            if (rbytes == 0) {
                // Server closed
                break;
            }
            printf("%.*s", (int)rbytes, rsp_buff);
            if (rsp_buff[rbytes - 1] == RDSH_EOF_CHAR) {
                break;
            }
        }

        if (strcmp(cmd_buff, "exit") == 0 || strcmp(cmd_buff, "stop-server") == 0) {
            break;
        }
    }

    return client_cleanup(cli_socket, cmd_buff, rsp_buff, OK);
}

int start_client(char *server_ip, int port) {
    int cli_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_socket < 0) {
        perror("socket");
        return ERR_RDSH_CLIENT;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int ret = inet_pton(AF_INET, server_ip, &addr.sin_addr);
    if (ret <= 0) {
        perror("inet_pton");
        close(cli_socket);
        return ERR_RDSH_CLIENT;
    }

    ret = connect(cli_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("connect");
        close(cli_socket);
        return ERR_RDSH_CLIENT;
    }
    return cli_socket;
}

int client_cleanup(int cli_socket, char *cmd_buff, char *rsp_buff, int rc) {
    if (cli_socket > 0) {
        close(cli_socket);
    }
    free(cmd_buff);
    free(rsp_buff);
    return rc;
}

