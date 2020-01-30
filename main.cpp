/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: tenevoi
 *
 * Created on 19 декабря 2019 г., 15:57
 */


#include "Recorder.hpp"
#include "Scheduller.hpp"

#include <cstdlib>
#include <map>
#include <mutex>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <iostream> //close()
#include <signal.h>
#include <dirent.h>
#include <experimental/filesystem>
#define dcout if(DEBUG)cout << "<MAIN>: " 

using namespace std;


bool exitFlag = false;

void term_handler(int i) {
    cout << "Terminating\r\n";
    exitFlag = true;
}

void setExitFunction() {
    struct sigaction sa;
    sigset_t newset;
    sigemptyset(&newset);
    sigaddset(&newset, SIGHUP);
    sigprocmask(SIG_BLOCK, &newset, 0);
    sa.sa_handler = term_handler;
    sigaction(SIGTERM, &sa, 0);
}

int main(int argc, char** argv) {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    setExitFunction();

    system("killall -9 segment_mover");

    DIR* recDir = opendir(strPathToArchive);
    if (recDir)
    {
        /* Directory exists. */
        closedir(recDir);
    } else
    {

        system("mkdir " strPathToArchive);
        cout << "Dir not exist\r\n";
    }

    ChannelList channelList;

    Recorder recorder(channelList);
    recorder.Run();




    Scheduller g;

    g.Init(&recorder, &channelList);

    while (!exitFlag)
    {
        namespace fs = std::experimental::filesystem;
        fs::path p = fs::path(strPathToArchive);
        uintmax_t free = fs::space(p).available;

        if (free < MAXDATASIZE * 2)
            g.DelLast();
        this_thread::sleep_for(chrono::microseconds(100));
    }

    //Ждём пока система остановиться
    if (recorder.running)
        recorder.thr->join();
    dcout << "Waking up\r\n";
    cout << "exit\r\n";
    return 0;
}

