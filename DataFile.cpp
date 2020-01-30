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
#include <vector>
#include <experimental/filesystem>

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
        cout << "ERROR: error add channel\r\n";
    return id; //ошибка если -1
}

bool DataFile::AddDataToFile(segment_timecontext &segment) {
    int currentChannelNumber;
    map<string, IndexFile>::iterator current_index_data;
    IndexFile * currentIndexFile = NULL;


    if (file == NULL)
        if (CreateNewDataFile(segment))
        {
            cout << "Create new data file error\r\n";
            return true;
        } else
        {
            cout << "Create new data file ok\r\n";
        }

    //найти номер канала
    if ((currentChannelNumber = GetChannelNumberByName(segment)) == -1)
    {
        cout << "channel number not found\r\n";
        return true;
    } 
    //найти в списке нужный индекс файл по имени канала
    current_index_data = indexFiles.find(segment.camname);
    if (current_index_data == indexFiles.end())
    {
        //если в списке нет такого канала то добавить его
        current_index_data = indexFiles.emplace(segment.camname, IndexFile(segment.camname)).first;
        (*current_index_data).second.CreateFile(prefixName);
    }

    //проверить свободное место
    if (!CheckFreeSpace(pathToFileDir.c_str(), MAXDATASIZE))
    {
        //todo delete last file;
        cout << "ERROR: Нет достаточного места на диске\r\n";
        exitFlag = true;
        return true;
    }

    char sourcefilepath[PATH_MAX];
    sprintf(sourcefilepath, "%s/%s", segment.source_dir, segment.filename);
    FILE * soursefile = NULL;
    if ((soursefile = fopen(sourcefilepath, "rb")) == NULL)
    {
        cout << "ERROR: openfile false:" << strerror(errno) << "\r\n";
        return true;
    }
    //перемотать курсор файла в конец
    if (fseek(file, 0L, SEEK_END))
    {
        cout << "ERROR: fseek dest file error:" << strerror(errno) << "\r\n";
        fclose(soursefile);
        return true;
    }
    //записать положение курсора в переменную
    long dest_start_pos = ftell(file);

    //проверить не превышает ли размер дата файла допустимый.
    if (dest_start_pos > MAXDATASIZE)
    {
        //ConctatIndexFiles();
        CreateNewDataFile(segment);
        //найти в списке нужный индекс файл по имени канала
        current_index_data = indexFiles.find(segment.camname);
        if (current_index_data == indexFiles.end())
        {
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
    if (writed == 1)
    {
        if (SAVEWRITE)fflush(file);
    } else
    {
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
    if ((*current_index_data).second.AddData(temp, prefixName))
    {
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
    if (unlink(sourcefilepath) != 0)
    {
        cout << "ERROR: delete dest file error:" << strerror(errno) << "\r\n";
        dcout << "Exiting from AddDataToFile\r\n";
        return true;
    }

    return false;
}

void DataFile::ConctatIndexFiles() {
    if (indexFiles.empty())
        return;
#pragma pack(push,1)

    struct pos {
        int id;
        long start;
        long stop;
    };
#pragma pack(pop)

    vector<struct pos> ar;
    string dstfname = pathToFileDir + prefixName + ".i";
    FILE * dst = fopen(dstfname.c_str(), "wb+");
    for (auto &it : indexFiles)
    {
        int id = channels->FindChannel(it.first);
        long start = ftell(dst);
        //cout << "o " << it.second.FileName << "";
        FILE * src = it.second.file;
        size_t srcsize = getfilesize(src);
        //cout << "size = " << srcsize << "\r\n";

        char * srcdata = (char*) malloc(srcsize);
        fseek(src,0,0);
        size_t readed = fread(srcdata, srcsize, 1, src);
        //cout << "r -> " << srcsize * readed << "\r\n";
        size_t writed = fwrite(srcdata, srcsize, 1, dst);
        //cout << "wr -> " << srcsize * writed << "\r\n";
        fflush(dst);
        long stop = ftell(dst);


        struct pos s;
        s.id = id;
        s.start = start;
        s.stop = stop;
        ar.push_back(s);


        fclose(src);
        free(srcdata);
        unlink(it.second.FileName.c_str());
    }

    for (auto &it : ar)
    {
        fwrite(&it, sizeof (it), 1, dst);
    }
    auto count = ar.size();
    fwrite(&count, sizeof (count), 1, dst);
    fflush(dst);
    fclose(dst);

}

bool DataFile::CheckFreeSpace(const char *diskpath, uintmax_t mustHave) {

    namespace fs = std::experimental::filesystem;
    fs::path p = fs::path(diskpath);
    std::error_code er;
    uintmax_t free = fs::space(p, er).available;
    if (er)
    {
        cout << er.message() << "\r\n";
        return true;
    }
    if (free > mustHave)
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
    if (file != NULL)
    {
        fflush(file);
        fclose(file);
        file = NULL;
    }
    ConctatIndexFiles();

    prefixName = to_string(time(NULL));
    fileName = pathToFileDir + prefixName + ".data";
    if ((file = fopen(fileName.c_str(), "ab+")) == NULL)
    {
        cout << "ERROR: Cannot open data file. " << strerror(errno) << "\r\n";
        return true;
    }
    indexFiles.clear();

    return false;
}

DataFile::~DataFile() {

}

