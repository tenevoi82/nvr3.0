/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   indexFile.cpp
 * Author: tenevoi
 * 
 * Created on 9 января 2020 г., 17:13
 */

#include <limits.h>
#include <cstring>

#include "IndexFile.hpp"
#define dcout if(DEBUG)cout << "<IndexFile>: " 

IndexFile::IndexFile(string chName) {
    dcout << "Creating class IndexFile *************************\r\n";
    this->chName = chName;
}

bool IndexFile::AddData(struct videodatapart & data, const string & prefix) {
    if (file == NULL) {
        cout << "Creating file IndexFile         **********************\r\n";
        if (CreateFile(prefix)) {
            cout << "index flile error\r\n";
            return true;
        }
    }
    size_t writed = -1;
    size_t size = sizeof (struct videodatapart);
    writed = fwrite(&data, size, 1, file);
    if (writed == 1) {
        if (SAVEWRITE)fflush(file); //for write imidiatli
        return false;
    } else {
        cout << "ERROR Write index file\r\n";
        return true;
    }
    cout << "writed " << writed << "bytes from " << size << " to indexfile\r\n";
    return false;
}

bool IndexFile::CreateFile(const string & prefix) {

    dcout << "Entering to CreateNewIndexFile\r\n";
    this->prefix = prefix;
    FileName  = PathToFileDir + prefix + "-index-"+chName;
    if ((file = fopen(FileName.c_str(), "ab+")) == NULL) {
        cout << "Cannot open index file. " << strerror(errno) << "\r\n";
        return true;
    }
    return false;
}