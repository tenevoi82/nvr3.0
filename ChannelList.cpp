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
    fclose(file);
    CreateFile();
    m.lock();
    //fseek(file, sizeof (versionOfFile), SEEK_SET);
    for (auto const &it : items) {
        if ((fwrite(&it.second, sizeof (it.second), 1, file)) != 1) {
            cout << "ERROR: write data to channels file\r\n";
            return true;
        }
    }
    fflush(file);
    m.unlock();
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
    m.lock();
    if (file == NULL) {
        if ((file = fopen(fileName.c_str(), "rb+")) == NULL) {
            m.unlock();
            if (CreateFile()) {
                cout << "ERROR: open channels file;\r\n";
                return;
            } else {
                m.lock();
            }
        } else {
            if (fread(&versionOfFile, sizeof (versionOfFile), 1, file) != 1) {
                cout << "ERROR: read version error\r\n";
                fclose(file);
                file = NULL;
                m.unlock();
                return;
            } else {
                if (versionOfFile != 4) {
                    cout << "ERROR: Wron file version\r\n";
                    fclose(file);
                    file = NULL;
                    m.unlock();
                    return;
                }
                items.clear();
            }
        }
    }


    struct channels_item item;
    while (fread(&item, sizeof (item), 1, file) != 0)
        items.emplace(item.chName, item);
    m.unlock();
    Print();
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
    struct channels_item item;
    memset(item.chName, 0, sizeof (item.chName));
    strncpy(item.chName, channel.c_str(), channel.size());
    item.id = items.size();
    item.firstStart = 999L;

    auto res = items.emplace(channel, item);
    if (res.second) {
        cout << "Channel " << item.chName << " added\r\n";
        if (fwrite(&item, sizeof (item), 1, file) != 1) {
            cout << "ERROR: write add channel to file\r\n";
        } else {
            fflush(file);
            return -1;
        }
        return res.first->second.id;
    }
    return 0;
}

int ChannelList::FindChannel(const string& channel) {
    int id;
    m.lock();
    auto it = items.find(channel);
    if (it != items.end())
        id = -1;
    else
        id = (*it).second.id;
    m.unlock();
    return id;
}

bool ChannelList::CreateFile() {
    m.lock();
    if (file != NULL) {
        fclose(file);
        file = NULL;
    }
    if ((file = fopen(fileName.c_str(), "wb+")) == NULL) {
        cout << "ERROR: Cannot open data file. " << strerror(errno) << "\r\n";
        file = NULL;
        m.unlock();
        return true;
    }
    fwrite(&versionOfFile, sizeof (versionOfFile), 1, file);
    m.unlock();
}