/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DataFile.cpp
 * Author: tenevoi
 * 
 * Created on 9 января 2020 г., 17:19
 */

#include <dirent.h>
#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/statfs.h>

#include "DataFile.hpp"
#define dcout if(DEBUG)cout << "<DataFile>: " 

extern bool exitFlag;

DataFile::DataFile() {
}

bool DataFile::SetChannelList(ChannelList &channels) {
    this->channels = &channels;
}


int DataFile::GetChannelNumberByName(const struct segment_timecontext & segment) {
    int id;
    id = channels->FindChannel(segment.camname); //пробуем найти номер канала
    if (id == -1) //если не найден 
        id = channels->AddChannel(segment.camname); //пробуем добавить
    if (id == -1) // если все равно -1 значит ошибка
        cout << "ERROR: error add channel";
    return id; //ошибка если -1
}

bool DataFile::AddDataToFile(segment_timecontext &segment) {
    int currentChannelNumber;
    map<string,IndexFile>::iterator current_index_data;
    IndexFile * currentIndexFile = NULL;
    
    
    if (file == NULL)
        if (CreateNewDataFile(segment)) {
            return true;
        }

    //найти номер канала
    if ((currentChannelNumber = GetChannelNumberByName(segment)) == -1)
        return true;

    //найти в списке нужный индекс файл по имени канала
    current_index_data = indexFiles.find(segment.camname);
    if (current_index_data == indexFiles.end()) {
        //если в списке нет такого канала то добавить его
        current_index_data = indexFiles.emplace(segment.camname, IndexFile(segment.camname)).first;
        (*current_index_data).second.CreateFile(prefixName);
    }

    //проверить свободное место
    if (!CheckFreeSpace(pathToFileDir.c_str(), MAXDATASIZE)) {
        //todo delete last file;
        cout << "ERROR: Нет достаточного места на диске\r\n";
        exitFlag = true;
        return true;
    }

    char sourcefilepath[PATH_MAX];
    sprintf(sourcefilepath, "%s/%s", segment.source_dir, segment.filename);
    FILE * soursefile = NULL;
    if ((soursefile = fopen(sourcefilepath, "rb")) == NULL) {
        cout << "ERROR: openfile false:" << strerror(errno) << "\r\n";
        return true;
    }
    //перемотать курсор файла в конец
    if (fseek(file, 0L, SEEK_END)) {
        cout << "ERROR: fseek dest file error:" << strerror(errno) << "\r\n";
        fclose(soursefile);
        return true;
    }
    //записать положение курсора в переменную
    long dest_start_pos = ftell(file);

    //проверить не превышает ли размер дата файла допустимый.
    if (dest_start_pos > MAXDATASIZE) {
        CreateNewDataFile(segment);
        //найти в списке нужный индекс файл по имени канала
        current_index_data = indexFiles.find(segment.camname);
        if (current_index_data == indexFiles.end()) {
            //если в списке нет такого канала то добавить его
            current_index_data = indexFiles.emplace(segment.camname, IndexFile(segment.camname)).first;
            (*current_index_data).second.CreateFile(prefixName);
        }
        dest_start_pos = 0;
    }
    //узнать размер копируемого файла
    long sourcefilesize = getfilesize(soursefile);

    //выделить память под этот размер
    char * buffer = (char*) malloc(sourcefilesize);

    //считать весь файл в память
    long readed = fread(buffer, sourcefilesize, 1, soursefile);

    //дописать содержимое буффера в файл назначения
    long writed = fwrite(buffer, sourcefilesize, 1, file);

    //проверить записалось ли все без проблем
    if (writed == 1) {
        if (SAVEWRITE)fflush(file);
    } else {
        cout << "ERROR: ошибка записи в дата файл" << strerror(errno) << "\r\n";
        fclose(soursefile);
        free(buffer);
        return true;
    }

    //записать положения курсора большого файлы в переменную
    long dest_end_pos = ftell(file);

    videodatapart temp;
    temp.startTime = segment.start;
    temp.duration = segment.duration;
    temp.pos = dest_start_pos;
    temp.size = dest_end_pos - dest_start_pos;


    //добавить данные в нужный индекс файл
    if ((*current_index_data).second.AddData(temp, prefixName)) {
        //освободить памать 
        free(buffer);
        //закрыть копируемый файл 
        fclose(soursefile);
        return true;
    }




    //освободить памать 
    free(buffer);
    //закрыть копируемый файл 
    fclose(soursefile);
    //удалить копируемый файл
    if (unlink(sourcefilepath) != 0) {
        cout << "ERROR: delete dest file error:" << strerror(errno) << "\r\n";
        dcout << "Exiting from AddDataToFile\r\n";
        return true;
    } 

    return false;
}

bool DataFile::CheckFreeSpace(const char *diskpath, long mustHave) {
    struct statfs buf;
    statfs(diskpath, &buf);
    //cout << "space free " << buf.f_bsize * buf.f_bavail << "\r\n";
    if (buf.f_bsize * buf.f_bavail > mustHave)
        return true;

    else
        return false;
};

long DataFile::getfilesize(FILE *f) {
    struct stat st;
    fstat(fileno(f), &st);

    return st.st_size;
};

bool DataFile::CreateNewDataFile(segment_timecontext &segment) {
    if (file != NULL) {
        fflush(file);
        fclose(file);
        file = NULL;
    }
    prefixName = to_string(time(NULL));
    fileName = pathToFileDir + prefixName + "-data";
    if ((file = fopen(fileName.c_str(), "ab+")) == NULL) {
        cout << "ERROR: Cannot open data file. " << strerror(errno) << "\r\n";
        return true;
    }
    indexFiles.clear();

    return false;
}

DataFile::~DataFile() {

}

