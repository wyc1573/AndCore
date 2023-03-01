//
// Created by wyc on 2023/2/28.
//

#include <sys/socket.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <thread>
#include <fcntl.h>
#include <unistd.h>

#include "android-base/logging.h"
#include "android-base/file.h"

#define SOCK_PATH "/data/user/0/com.wyc.andcore/files/server.socket"
#define SEND_FILE_PATH "/data/user/0/com.wyc.andcore/files/test"

void* handle_request(void* client) {
    int client_sock = reinterpret_cast<uint64_t>(client);
    int rc = -1;
    while (true) {
        char recv_buff[256];
        /**
         * 几种方式都可以接收
         * */
//        rc = recv(client_sock, recv_buff, 256, 0);
//        rc = read(client_sock, recv_buff, 256);
        rc = recvfrom(client_sock, recv_buff, 256, 0, NULL, NULL);
        if (rc == -1) {
            PLOG(INFO) << "RECV ERROR";
            return nullptr;
        }
        LOG(INFO) << "RECV: " << recv_buff << "\n";
        sleep(1);
    }
}

void handle_connect(int server) {
    if (server < 0) {
        LOG(INFO) << "handle_connect: illegal client, return.";
        return;
    }
    struct sockaddr_un client_addr;
    socklen_t addr_len = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    while(true) {
        int client = accept(server, (struct sockaddr*) &client_addr, &addr_len);
        if (client == -1) {
            LOG(INFO) << "handle_connect accept error!";
            return;
        }
        pthread_t pthread;
        pthread_create(&pthread, NULL, handle_request, (void *)client);
    }
}

void handle_client(int client) {
    int rc = -1;
    if (client < 0) {
        LOG(INFO) << "handle_client: illegal client, return.";
        return;
    }
    while (true) {
        const char* send_buff = "This is msg from client.";
        LOG(INFO) << "\nSEND: " << send_buff;
        /**
         * 几种方式都可以发送
         * */
//        rc = send(client, send_buff, strlen(send_buff) + 1, 0);
//        rc = write(client, send_buff, strlen(send_buff) + 1);
        rc = sendto(client, send_buff, strlen(send_buff) + 1, 0, NULL, NULL);
        if (rc == -1) {
            PLOG(INFO) << "SEND ERROR";
            return;
        }
        sleep(2);
    }
}

void test_normal_socket() {
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        int rc = -1, server = -1;
        socklen_t sock_len = -1;
        struct sockaddr_un server_addr;
        struct sockaddr_un client_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        memset(&client_addr, 0, sizeof(client_addr));

        server = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server == -1) {
            PLOG(INFO) << "SOCKET ERROR!";
            return;
        }
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SOCK_PATH);
        sock_len = offsetof(struct sockaddr_un, sun_path) + strlen(server_addr.sun_path) + 1;
        unlink(SOCK_PATH);
        rc = bind(server, (struct sockaddr*)& server_addr, sock_len);
        if (rc == -1) {
            PLOG(INFO) << "BIND ERROR!";
            close(server);
            return;
        }
        chmod(SOCK_PATH, 0666);
        rc = listen(server, 16);
        if (rc == -1) {
            PLOG(INFO) << "LISTEN ERROR!";
            close(server);
            exit(1);
        }
        handle_connect(server);
    } else {
        sleep(1);
        int client = -1, rc = -1;
        socklen_t addr_len = -1;
        struct sockaddr_un server_addr;
        client = socket(AF_UNIX, SOCK_STREAM, 0);
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SOCK_PATH);
        addr_len = offsetof(struct sockaddr_un, sun_path) + strlen(server_addr.sun_path) + 1;
        rc = connect(client, (struct sockaddr*) &server_addr, addr_len);
        if (rc == -1) {
            PLOG(INFO) << "CONNECT ERROR!";
            close(client);
            return;
        }
        handle_client(client);
    }
}

void handle_diag_server(int server) {
    char recv_buf[256] = "";
    struct sockaddr_un client_addr;
    socklen_t sock_len = sizeof(struct sockaddr_un);
    while (true) {
        recvfrom(server, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*) &client_addr, &sock_len);
        LOG(INFO) << "RECV: " << recv_buf;
        sleep(1);
    }
}

void handle_diag_client() {
    int client = 0;
    struct sockaddr_un addr;
    bzero(&addr,sizeof(addr));

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path,SOCK_PATH);

    client = socket(AF_UNIX,SOCK_DGRAM,0);
    if(client < 0)
    {
        PLOG(INFO) << "handle_diag_client: socket error.";
        return;
    }

    while(true)
    {
        const char send_buf[256] = "This is msg from client.";
        int len = offsetof(struct sockaddr_un, sun_path) + strlen(addr.sun_path) + 1;
        sendto(client, send_buf,strlen(send_buf),0,(struct sockaddr*)&addr,len);
        LOG(INFO) << "\nSEND: " << send_buf;
        sleep(1);
    }
}

void test_diagram_socket() {
    pid_t pid = fork();
    if (pid == 0) {
        int server_sock = -1, rc = -1;
        socklen_t sock_len;
        struct sockaddr_un server_addr;
        server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (server_sock < 0) {
            PLOG(INFO) << "test_diagram_socket socket error.";
            return;
        }
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SOCK_PATH);
        sock_len = offsetof(struct sockaddr_un, sun_path) + strlen(server_addr.sun_path) + 1;
        unlink(SOCK_PATH);
        rc = bind(server_sock, (struct sockaddr*)&server_addr, sock_len);
        if (rc == -1) {
            PLOG(INFO) << "test_diagram_socket: bind error.";
            return;
        }
        handle_diag_server(server_sock);
    } else {
        sleep(1);
        handle_diag_client();
    }
}

void handle_send_msg(int client) {
    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCK_PATH);

    struct iovec vec = {NULL, 0};
    char msg_control[CMSG_SPACE(sizeof(int))];
    struct msghdr msg = {(void*)&server_addr, sizeof(server_addr),
            &vec, 1, msg_control, sizeof(msg_control), 0};
    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    android::base::WriteStringToFile("Hello, World", SEND_FILE_PATH);
    int fd = open(SEND_FILE_PATH, O_RDWR, S_IRWXO | S_IRUSR | S_IRGRP);
    *(int*)CMSG_DATA(cmsg) = fd;
    LOG(INFO) << "SEND FD: " << fd <<", SEND MSG: " << "Hello, World";
    sendmsg(client, &msg, 0);
    sleep(10);
}

void handle_recv_msg(int server) {
    char buf[512];
    struct iovec vec = {NULL, 0};
    char msg_control[CMSG_SPACE(sizeof(int))];
    struct msghdr msg = {NULL, 0, &vec, 0, msg_control, sizeof(msg_control), 0};
    int rc = recvmsg(server, &msg, 0);
    if (rc == -1) {
        PLOG(INFO) << "handle_recv_msg recvmsg error.";
        return;
    }
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    int fd = *(int*) CMSG_DATA(cmsg);
    std::string content;
    android::base::ReadFdToString(fd, &content);
    LOG(INFO) << "RECV FD: " << fd << ", RECV MSG: " << content;
}

void test_msg_socket() {
    pid_t pid = fork();
    if (pid == 0) {
        int server_sock = -1, rc = -1;
        socklen_t sock_len;
        struct sockaddr_un server_addr;
        server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (server_sock < 0) {
            PLOG(INFO) << "test_msg_socket socket error.";
            return;
        }
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SOCK_PATH);
        sock_len = offsetof(struct sockaddr_un, sun_path) + strlen(server_addr.sun_path) + 1;
        unlink(SOCK_PATH);
        rc = bind(server_sock, (struct sockaddr*)&server_addr, sock_len);
        if (rc == -1) {
            PLOG(INFO) << "test_msg_socket: bind error.";
            return;
        }
        sleep(2);
        handle_recv_msg(server_sock);
    } else {
        sleep(1);
        int client = -1;
        client = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (client == -1) {
            PLOG(INFO) << "test_msg_socket socket error.";
            return;
        }
        handle_send_msg(client);
    }
}

void test_socket_pair() {
    int fds[2];
    int rc = socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    if (rc == -1) {
        PLOG(INFO) << "socketpair error";
        return;
    }

    if (fork()) {
        int val = 0;
        close(fds[1]);
        while (true) {
            val++;
            write(fds[0], &val, sizeof(val));
            LOG(INFO) << "parent write " << val;
            sleep(1);
            read(fds[0], &val, sizeof(val));
            LOG(INFO) << "parent read " << val;
            sleep(1);
        }
    } else {
        close(fds[0]);
        int val;
        while (true) {
            read(fds[1], &val, sizeof(val));
            LOG(INFO) << "child read " << val;
            val++;
            write(fds[1], &val, sizeof(val));
            LOG(INFO) << "child write " << val;
        }
    }
}

void test_socket_pipe() {
    int fds[2];
    int rc = pipe(fds);
    if (rc == -1) {
        PLOG(INFO) << "pipe error";
        return;
    }

    if (fork()) {
        int val = 0;
        close(fds[0]);
        while (true) {
            val++;
            write(fds[1], &val, sizeof(val));
            LOG(INFO) << "parent write " << val;
            sleep(1);
        }
    } else {
        close(fds[1]);
        int val;
        while (true) {
            read(fds[0], &val, sizeof(val));
            LOG(INFO) << "child read " << val;
            sleep(1);
        }
    }
}

void test_eventfd() {
    int fd = eventfd(0, EFD_CLOEXEC /*| EFD_NONBLOCK*/ | EFD_SEMAPHORE);
    if (fork()) {
        // parent
        uint64_t counter = 2;
        sleep(5);
//        while (true) {
//            counter++;
            int rc = write(fd, &counter, sizeof(counter));
            if (rc == -1) {
                PLOG(INFO) << "parent write error.";
            } else {
                LOG(INFO) << "parent write " << counter;
            }
            sleep(1);
//        }
    } else {
        uint64_t counter;
        while (true) {
            int rc = read(fd, &counter, sizeof(counter));
            if (rc == -1) {
                PLOG(INFO) << "child read error";
            } else {
                LOG(INFO) << "child read " << counter;
            }
            rc = read(fd, &counter, sizeof(counter));
            if (rc == -1) {
                PLOG(INFO) << "child read2 error";
            } else {
                LOG(INFO) << "child read2 " << counter;
            }
        }
    }
}

void test_epoll() {
    int efd = eventfd(1, 0);
    if (efd == -1) {
        PLOG(INFO) << "eventfd error";
        return;
    }

    if (fork()) {
        int fd = epoll_create(1);
        if (fd == -1) {
            PLOG(INFO) << "epoll_create error";
            return;
        }
        struct epoll_event ev, events[10];
        ev.events = EPOLLIN;
        ev.data.fd = efd;
        int rc = epoll_ctl(fd, EPOLL_CTL_ADD, efd, &ev);
        if (rc == -1) {
            PLOG(INFO) << "EPOLL_CTL_ADD error.";
            return;
        }
        int nfds;
        for (;;) {
            nfds = epoll_wait(fd, events, 10, -1);
            if (nfds == -1) {
                PLOG(INFO) << "epoll_wait error.";
                return;
            }
            for (int i = 0; i < nfds; i++) {
                if (events[i].data.fd == efd) {
                    LOG(INFO) << "parent receive event fd event.";
                    uint64_t val;
                    read(efd, &val, sizeof(val));
                }
            }
        }
    } else {
        while (true) {
            sleep(3);
            uint64_t val = 1;
            write(efd, &val, sizeof(val));
        }
    }
}
