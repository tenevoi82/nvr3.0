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

#include <cstdlib>
#include <map>
#include <mutex>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <iostream> //close()
#include <signal.h>
#include <dirent.h>
#define cout cout << "<MAIN>: " 

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

    
    DIR* recDir = opendir(strPathToArchive);
    if (recDir)
    {
        /* Directory exists. */
        closedir(recDir);
    }
    else{
        
        system("mkdir "strPathToArchive);
        cout << "Dir not exist\r\n";
    }
    
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    setExitFunction();
    cout << "Creating recorder\r\n";
    Recorder recorder;
    cout << "Recorder was created\r\n";
    cout << "Runing recorder\r\n";
    recorder.Run();
    cout << "Recorder was run\r\n";
    cout << "Going to sleep for 100 sec\r\n";
    //    while(!exitFlag){
    //        usleep(5000);
    //    }
    recorder.thr->join();
    //recorder.thr->join();
    //this_thread::sleep_for(chrono::seconds(30));
    cout << "Waking up\r\n";


    cout << "exit\r\n";
    return 0;
}

