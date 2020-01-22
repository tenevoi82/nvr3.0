/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ChannelList.hpp
 * Author: tenevoi
 *
 * Created on 31 декабря 2019 г., 11:35
 */

#ifndef CHANNELLIST_HPP
#define CHANNELLIST_HPP

#include <map>
#include <string>
#include <mutex>
#include <iostream>
#include <list>

#include "IndexFile.hpp"

using namespace std;

class ChannelList {
public:
    ChannelList();
    int AddChannel(const string &channel);
    virtual ~ChannelList();
    void Print();
    int FindChannel(const string& channel);     
private:
    void LoadFromDisk();
    bool SaveChandes();
    bool CreateFile();
    mutex m;
    map<string, struct channels_item> items;
    FILE *file =NULL;
    string fileName;
    string pathToFileDir = strPathToArchive;
    int versionOfFile = 4;
    array<IndexFile,0>  indexFiles;
};


#endif /* CHANNELLIST_HPP */

