/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Recorder.cpp
 * Author: tenevoi
 * 
 * Created on 10 января 2020 г., 11:49
 */
#include "Recorder.hpp"
#include <netinet/ip.h>
#include <string.h>
#define dcout if(DEBUG) cout << "<Recorder>: " 

Recorder::Recorder() {
    dcout << "Creating class Recorder\r\n";
    dataFile.SetChannelList(channelList);

}

Recorder::~Recorder() {
    dcout << "Distroing class Recorder\r\n";
}

extern bool exitFlag;

void Recorder::WorkWithFFMPEG() {
    running = true;
    cout << "Starting new thread for main work of recorder and my id is " << getpid() << "\r\n";
    int newsock;
    char buf[BUF_SIZE];

    while (!exitFlag) {
        newsock = accept(MainSocket, NULL, NULL);
        if (newsock < 0) {
            cout << "accept() failed:" << strerror(errno) << "\r\n";
            running = false;
            return;
        }
        int count = 0;
        count = read(newsock, buf, BUF_SIZE);
        buf[count] = 0;
        cout << buf << "\r\n";
        struct segment_timecontext segment;
        sscanf(buf,
                "c:%s\ti:%d\tf:%s\ts:%Lf\td:%f\te:%Lf\tdir:%s",
                segment.camname, &segment.index, segment.filename, &segment.start, &segment.duration, &segment.end, segment.source_dir);
        dataFile.AddDataToFile(segment);

        shutdown(newsock, SHUT_RDWR);
        close(newsock);
    }
    cout << "Closing socket.\r\n";
    shutdown(MainSocket, SHUT_RDWR);
    fcloseall();
    cout << "Stoping thread of main work of recorder and my id is " << this_thread::get_id() << "\r\n";
    running = false;
}

bool Recorder::NetInit() {
    struct sockaddr_in serv_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cout << "socket create failed: " << strerror(errno) << "\r\n";
        return false;
    }
    memset((char *) &serv_addr, 0, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(INPUTPORT);
    for (int count = 0; bind(sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0; count++) {
        cout << "bind socket failed: " << strerror(errno) << " retry after " << RETRY_NETWORCK_REINIT_SECOND << "second\r\n";
        if (count > RETRY_NETWORCK_REINIT_COUNT) {
            cout << "The maximum number of network setup attempts has been reached.\r\n";
            return false;
        }
        std::this_thread::sleep_for(std::chrono::seconds(RETRY_NETWORCK_REINIT_SECOND));
    }
    if (sock <= 0) {
        cout << "Critical network error! abort\r\n";
        return false;
    }
    cout << "Network Initialization Successful\r\n";

    if (listen(sock, SOMAXCONN) == -1) {
        cout << "socket listen failed: " << strerror(errno) << "\r\n";
        return false;
    }
    MainSocket = sock;
    return true;
}

void Recorder::Run() {
    cout << "Startting recorder\r\n";
    if (!NetInit()) {
        cout << "NET INIT ERROR\r\n";
        return;
    }

    thr = new thread([&]() {
        WorkWithFFMPEG(); });
    sleep(1);
}