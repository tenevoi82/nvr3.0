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

#define dcout if(DEBUG)cout << "<ChannelList>: " 

void ChannelList::SaveChandes() {
    dcout << "Entering to SaveChandes\r\n";
    return ;
    m.lock();
    if (file == NULL)
    {
        if ((file = fopen("/var/www/video/archive/channels", "wb+")) == NULL)
        {
            cout << "Cannot open data file. " << strerror(errno) << "\r\n";
            file = NULL;
            m.unlock();
            dcout << "Exiting from SaveChandes\r\n";            
            return;
        }
    }
    cout << "File is not null\r\n" ;
    fwrite(&version, sizeof (version), 1, file);
    dcout << "fwrite version is complited\r\n";
    for (auto const &it : items)
    {
        cout << "for\r\n";
        fwrite(&it.second, sizeof (it.second), 1, file);
        fflush(file);
    }
    cout << "Saving channel list\r\n";
    fclose(file);
    file = NULL;
    m.unlock();
    dcout << "Exiting from SaveChandes\r\n";
}

void ChannelList::Print() {
    m.lock();
    for (auto const &it : items)
    {
        cout << it.first << "\t" << it.second.id << "\r\n";
    }
    m.unlock();
}

void ChannelList::LoadFromDisk() {
    m.lock();
    if (file == NULL)
        if ((file = fopen("/var/www/video/archive/channels", "r")) == NULL)
        {
            cout << "Cannot open data file. " << strerror(errno) << "\r\n";
            file = NULL;
            m.unlock();
            return;
        }
    int versionOfFile = 0;
    if(fread(&versionOfFile, sizeof (versionOfFile), 1, file)!=1){
        cout << "read version error\r\n";
    }
    if (versionOfFile != version)
    {
        cout << "WRONG FILE VERSION\r\n";
        fclose(file);
        file = NULL;
        m.unlock();
    } else
        cout << "FILE VERSION IS OK (" << versionOfFile << ")\r\n";
    items.clear();
    struct channels_item item;
    while (fread(&item, sizeof (item), 1, file) != 0)
        items.emplace(item.chName, item);
    dcout << "Load channel list complit.\r\n";
    fclose(file);
    file = NULL;
    m.unlock();
    Print();
}

ChannelList::ChannelList() {
    dcout << "Creating class ChannelList\r\n";

}

ChannelList::~ChannelList() {
    SaveChandes();
    dcout << "Distroing class ChannelList\r\n";    
    for (auto elemelem : items)
    {
        cout << elemelem.first << " " << elemelem.second.chName << " " << elemelem.second.id << "\r\n" ;
    }

}

int ChannelList::AddChannel(const string& channel) {
    dcout << "Entering to AddChannel\r\n";
    struct channels_item item;
    memset(item.chName, 0, sizeof (item.chName));
    strncpy(item.chName, channel.c_str(), channel.size());
    item.id = items.size();
    item.firstStart = 999L;

    auto res = items.emplace(channel, item);
    if (res.second)
    {
        cout << "Channel " << item.chName << " added\r\n";
        SaveChandes();
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
    try
    {
        id = items.at(channel).id;
        cout << "FOUND CHANNEL WITH ID " << id << "\r\n";
    } catch (...)
    {
        cout << "CHANNEL NOT FOUND\r\n";
        id = -1;
    }
    m.unlock();
    dcout << "Exiting from FindChannel\r\n";
    return id;

}


