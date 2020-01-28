/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DataFile.hpp
 * Author: tenevoi
 *
 * Created on 9 января 2020 г., 17:19
 */

#ifndef DATAFILE_HPP
#define DATAFILE_HPP

#include <string>

#include "ChannelList.hpp"
#include "structures.hpp"
#include "settings.hpp"


using namespace std;

class DataFile {
public:
    DataFile();
    bool SetChannelList(ChannelList &channels);
    bool AddDataToFile(segment_timecontext&);
    virtual ~DataFile();
private:
    bool CreateNewDataFile(segment_timecontext&);
    long getfilesize(FILE *);
    bool CheckFreeSpace(const char *diskpath, long mustHave); //возвращает false если места не достаточно

    //найти номер канала
    int GetChannelNumberByName(const struct segment_timecontext &);
    void ConctatIndexFiles();
    FILE * file = NULL;
    string fileName;
    string prefixName;
    string pathToFileDir = strPathToArchive;
    ChannelList *channels;
    map<string, IndexFile> indexFiles;
};

#endif /* DATAFILE_HPP */

