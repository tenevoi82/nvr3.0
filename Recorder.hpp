/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Recorder.hpp
 * Author: tenevoi
 *
 * Created on 10 января 2020 г., 11:49
 */

#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <thread>

#include <unistd.h>
#include <map>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <chrono>

#include "settings.hpp"
#include "DataFile.hpp"
#include "IndexFile.hpp"
#include "ChannelList.hpp"

class Recorder {
public:
    thread * thr;
    Recorder(ChannelList&);
    void Run();
    virtual ~Recorder();
    bool running = false;
    ChannelList* channelList;
private:
    void WorkWithFFMPEG();
    bool NetInit();

    int MainSocket = -1;

    DataFile dataFile;

    //IndexFile indexFile;
};

#endif /* RECORDER_HPP */

