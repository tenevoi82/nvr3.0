/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Scheduller.cpp
 * Author: tenevoi
 * 
 * Created on 29 января 2020 г., 11:39
 */

#include "Scheduller.hpp"
#include "settings.hpp"
#include <iostream>
#include <cstring>
#include <experimental/filesystem>
#include <climits>

using namespace std;
namespace fs = std::experimental::filesystem;

extern bool exitFlag;

Scheduller::Scheduller() {
}

Scheduller::Scheduller(const Scheduller& orig) {
}

Scheduller::~Scheduller() {
}

void Scheduller::Init(Recorder * const r, ChannelList * const c) {
    if (r != NULL && c != NULL)
    {
        channelList = c;
        recorder = r;

    }
}

void Scheduller::DelLast() {       
    fs::path p = fs::path(strPathToArchive);
    std::error_code er;
    fs::directory_iterator dir(p);
    long mintime = LONG_MAX;
    fs::path toDeleteIndex;
    fs::path toDeleteData;
    for (auto &it : dir)
    {
        if (!it.path().filename().extension().compare(".i"))
        {
            //cout << it.path().filename().stem() << "\r\n";
            if (strtol(it.path().filename().stem().native().c_str(),NULL,10) < mintime){
                mintime = strtol(it.path().filename().stem().native().c_str(),NULL,10);
                toDeleteIndex = fs::path(it);
                toDeleteData = fs::path(it);
                toDeleteData.replace_extension(".data");
            }
        }
    }
    if(toDeleteIndex.empty())
        return;
    cout << "removing files: ";
    cout << toDeleteIndex << "\t";
    cout << toDeleteData << "\r\n";
    remove(toDeleteIndex);
    remove(toDeleteData);
    return;
    //Прочесть список каналов.    
    //Получить список индекс файлов
    //Получить данные из индекс файлов
    //Найти самую старую запись из всех индекс файлов
    //удалить найденный индекс файл и файл данных.
}