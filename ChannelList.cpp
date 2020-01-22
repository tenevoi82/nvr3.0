/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ChannelList.cpp
 * Author: tenevoi
 * 
 * Created on 31 декабря 2019 г., 11:35
 */

#include "ChannelList.hpp"
#include "structures.hpp"
#include <string.h>
#include <functional>
#include <unistd.h>

#define cout  cout << "<ChannelList>: " 
#define dcout  cout << "<ChannelList>: " 

bool ChannelList::SaveChandes() {
    dcout << "Entering to SaveChandes\r\n";
    m.lock();
    fseek(file, sizeof (versionOfFile), SEEK_SET);
    for (auto const &it : items) {
        fwrite(&it.second, sizeof (it.second), 1, file);
        cout << "WRWRWR\r\n";
    }
    fflush(file);
    cout << "Saving channel list\r\n";
    m.unlock();
    dcout << "Exiting from SaveChandes\r\n";
    return false;
}

void ChannelList::Print() {
    m.lock();
    cout << "\r\n===============Channel list:===============\r\n";
    cout << "Name" << "\t" << "id" << "\r\n";
    for (auto const &it : items) {
        cout << it.first << "\t" << it.second.id << "\r\n";
    }
    cout << "===========================================\r\n";
    m.unlock();
}

void ChannelList::LoadFromDisk() {
    dcout << "Entering to LoadChandes\r\n";
    m.lock();
    if (file == NULL) {
        if ((file = fopen(fileName.c_str(), "rb+")) == NULL) {
            m.unlock();
            if (CreateFile()) {
                cout << "ERROR OPEN CHANNELS FILE;\r\n";
                return;
            } else {
                m.lock();
            }
        } else {
            if (fread(&versionOfFile, sizeof (versionOfFile), 1, file) != 1) {
                cout << "read version error\r\n";
                fclose(file);
                file = NULL;
                m.unlock();
                return;
            } else {
                if (versionOfFile != 4) {
                    cout << "WRONG FILE VERSION\r\n";
                    fclose(file);
                    file = NULL;
                    m.unlock();
                    return;
                } else
                    cout << "FILE VERSION IS OK (" << versionOfFile << ")\r\n";
                items.clear();
            }
        }
    }


    struct channels_item item;
    while (fread(&item, sizeof (item), 1, file) != 0)
        items.emplace(item.chName, item);
    cout << "readed " << items.size() << "\r\n";
    dcout << "Load channel list complit.\r\n";
    m.unlock();
    Print();
    // sleep(100);
}

ChannelList::ChannelList() {
    dcout << "Creating class ChannelList\r\n";
    fileName = pathToFileDir + "channels";
    LoadFromDisk();

}

ChannelList::~ChannelList() {
    SaveChandes();
    Print();
    if (file != NULL) {
        fclose(file);
        file = NULL;
    }
    dcout << "Distroing class ChannelList\r\n";
}

int ChannelList::AddChannel(const string& channel) {
    dcout << "Entering to AddChannel\r\n";
    struct channels_item item;
    memset(item.chName, 0, sizeof (item.chName));
    strncpy(item.chName, channel.c_str(), channel.size());
    item.id = items.size();
    item.firstStart = 999L;

    auto res = items.emplace(channel, item);
    if (res.second) {
        cout << "Channel " << item.chName << " added\r\n";
        if (fwrite(&item, sizeof (item), 1, file) != 1) {

        } else {
            cout << "WRWRWR\r\n";
            dcout << "Exiting from AddChannel\r\n";
            return -1;
        }
        return res.first->second.id;
    }
    dcout << "Exiting from AddChannel\r\n";
    return 0;
}

int ChannelList::FindChannel(const string& channel) {
    dcout << "Entering to FindChannel\r\n";
    /*этого не нужно*/
    int id = -1;
    m.lock();
    try {
        id = items.at(channel).id;
        cout << "FOUND CHANNEL WITH ID " << id << "\r\n";
    } catch (...) {
        cout << "CHANNEL NOT FOUND\r\n";
        id = -1;
    }
    m.unlock();
    dcout << "Exiting from FindChannel\r\n";
    return id;

}

bool ChannelList::CreateFile() {
    m.lock();
    if (file != NULL) {
        fclose(file);
        file = NULL;
    }
    if ((file = fopen(fileName.c_str(), "ab+")) == NULL) {
        cout << "Cannot open data file. " << strerror(errno) << "\r\n";
        file = NULL;
        m.unlock();
        return true;
    }
    fwrite(&versionOfFile, sizeof (versionOfFile), 1, file);
    dcout << "fwrite version is complited\r\n";
    cout << "Create channels file is comlited\r\n";
    m.unlock();
}