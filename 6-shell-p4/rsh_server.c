#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>
#include "dshlib.h"
#include "rshlib.h"

int start_server(char *ifaces, int port, int is_threaded) {
    int svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0) {
        return svr_socket;
    }
    int rc = process_cli_requests(svr_socket);
    stop_server(svr_socket);
    return rc;
}

int stop_server(int svr_socket) {
    return close(svr_socket);
}

int boot_server(char *ifaces, int port) {
    int svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket < 0) {
        perror("socket");
        return ERR_RDSH_COMMUNICATION;
    }

    int enable = 1;
    if (setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ifaces, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    if (bind(svr_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    if (listen(svr_socket, 20) < 0) {
        perror("listen");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }
    return svr_socket;
}

int process_cli_requests(int svr_socket) {
    int cli_socket;
    int rc = OK;
    while (1) {
        cli_socket = accept(svr_socket, NULL, NULL);
        if (cli_socket < 0) {
            perror("accept");
            return ERR_RDSH_COMMUNICATION;
        }
        rc = exec_client_requests(cli_socket);
        close(cli_socket);
        if (rc == OK_EXIT || rc < 0) {
            break;
        }
    }
    stop_server(cli_socket);
    return rc;
}

int exec_client_requests(int cli_socket) {
    char *io_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (!io_buff) {
        return ERR_RDSH_SERVER;
    }
    while (1) {
        ssize_t io_size = recv(cli_socket, io_buff, RDSH_COMM_BUFF_SZ - 1, 0);
        if (io_size <= 0) {
            free(io_buff);
            return ERR_RDSH_COMMUNICATION;
        }
        io_buff[io_size] = '\0';

        command_list_t cmd_list;
        int rc = build_cmd_list(io_buff, &cmd_list);
        if (rc != OK) {
            send_message_string(cli_socket, "Invalid command");
            continue;
        }
        int cmd_rc = rsh_execute_pipeline(cli_socket, &cmd_list);
        if (cmd_rc != 0) {
            send_message_string(cli_socket, "Command execution error");
        } else {
            send_message_string(cli_socket, "Command executed successfully");
        }

        if (strcmp(io_buff, "exit") == 0) {
            break;
        }
        if (strcmp(io_buff, "stop-server") == 0) {
            free(io_buff);
            return OK_EXIT;
        }
        send_message_eof(cli_socket);
    }
    free(io_buff);
    return OK;
}

int send_message_eof(int cli_socket) {
    int sent_len = send(cli_socket, &RDSH_EOF_CHAR, 1, 0);
    if (sent_len != 1) {
        return ERR_RDSH_COMMUNICATION;
    }
    return OK;
}

int send_message_string(int cli_socket, char *buff) {
   int len = strlen(buff) + 1;
    int sent = send(cli_socket, buff, len, 0);
    if (sent != len) {
        return ERR_RDSH_COMMUNICATION;
    }
    return send_message_eof(cli_socket);
}

int rsh_execute_pipeline(int cli_sock, command_list_t *clist) {
    int pipes[clist->num - 1][2];
    pid_t pids[clist->num];
    int pids_st[clist->num];
    int exit_code;

    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pids[i] == 0) {
            if (i == 0) {
                dup2(cli_sock, STDIN_FILENO);
            } else {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i == clist->num - 1) {
                dup2(cli_sock, STDOUT_FILENO);
                dup2(cli_sock, STDERR_FILENO);
            } else {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(ERR_RDSH_CMD_EXEC);
        }
    }

    // parent closes all pipes
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // wait for all children
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], &pids_st[i], 0);
    }
    exit_code = WEXITSTATUS(pids_st[clist->num - 1]);
    for (int i = 0; i < clist->num; i++) {
        if (WEXITSTATUS(pids_st[i]) == EXIT_SC) {
            exit_code = EXIT_SC;
        }
    }
    return exit_code;
}
