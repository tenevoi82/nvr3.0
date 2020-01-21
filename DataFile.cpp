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

DataFile::DataFile() {
    dcout << "Creating class DataFile\r\n";
}

bool DataFile::SetChannelList(ChannelList &channels) {
    this->channels = &channels;
    File = NULL;
}

bool DataFile::AddDataToFile(segment_timecontext &segment) {
    dcout << "Enering to AddDataToFile\r\n";
    int id;
    if (File == NULL)
        if (CreateNewDataFile(segment))
        {
            return true;
        }
    //найти номер канала
    id = channels->FindChannel(segment.camname);
    if (id == -1)
        id = channels->AddChannel(segment.camname);
    //проверить размер файла, и если слишком много то создать новый
    if (!CheckFreeSpace(PathToFileDir.c_str(), MAXDATASIZE))
    {
        //todo delete last file;
        cout << "Нет достаточного места на диске\r\n";
        cout << "**************\r\n";
        cout << "***   NO   ***\r\n";
        cout << "**   SPACE  **\r\n";
        cout << "******  ******\r\n";
        cout << "**************\r\n";
    }

    char sourcefilepath[PATH_MAX];
    sprintf(sourcefilepath, "%s/%s", segment.source_dir, segment.filename);
    cout << "Sourcefile = " << sourcefilepath << "\r\n";
    FILE * soursefile = fopen(sourcefilepath, "rb");
    if (soursefile != NULL)
        cout << "openfile OK\r\n";
    else
    {
        cout << "openfile false:" << strerror(errno) << "\r\n";
        dcout << "Exiting from AddDataToFile\r\n";
        return true;
    }
    //перемотать курсор файла в конец
    if (fseek(File, 0L, SEEK_END) != 0)
    {
        cout << "fseek dest file error:" << strerror(errno) << "\r\n";
        fclose(soursefile);
        dcout << "Exiting from AddDataToFile\r\n";
        return true;
    } else
    {
        cout << "fseek dest file OK\r\n";
    }
    //записать положение курсора в переменную
    long dest_start_pos = ftell(File);
    //узнать размер копируемого файла
    if (dest_start_pos > 50 * 1024 * 1024)
    {
        fclose(File);
        File = NULL;
        AddDataToFile(segment);
    }
    long sourcefilesize = getfilesize(soursefile);
    cout << "Source file size is " << sourcefilesize << "\r\n";
    //выделить память под этот размер
    char * buffer = (char*) malloc(sourcefilesize);
    //считать весь файл в память
    long readed = fread(buffer, sourcefilesize, 1, soursefile);
    cout << "считанно " << readed * sourcefilesize << "byte\r\n";
    //дописать весь файл в файл назначения
    long writed = fwrite(buffer, sourcefilesize, 1, File);
    if (writed == 1)
    {
        cout << "записанно " << writed * sourcefilesize << "byte\r\n";
        if (SAVEWRITE)fflush(File);
    } else
    {
        cout << "Ошибка записи:" << strerror(errno) << "\r\n";
        fclose(soursefile);
        free(buffer);
        dcout << "Exiting from AddDataToFile\r\n";
        return true;
    }
    //записать положения курсора большого файлы в переменную
    long dest_end_pos = ftell(File);
    cout << "записанно от " << dest_start_pos << " до " << dest_end_pos << "\r\n";

    videodatapart temp;
    temp.startTime = segment.start;
    temp.duration = segment.duration;
    temp.pos = dest_start_pos;
    temp.size = dest_end_pos - dest_start_pos;

    //printf("start:%lf duration:%f start_pos:%d size:%d\r\n",temp.startTime,temp.duration,temp.pos,temp.size);


    //найти нужный индекс файл
    auto it = indexFiles.find(segment.camname);
    if (it == indexFiles.end())
    {
        it = indexFiles.emplace(segment.camname, IndexFile(segment.camname)).first;
    }

    //добавить данные в нужный индекс файл
    if ((*it).second.AddData(temp))
    {
        //освободить памать 
        free(buffer);
        //закрыть копируемый файл 
        fclose(soursefile);
        dcout << "Exiting from AddDataToFile\r\n";        
        return true;
    }




    //освободить памать 
    free(buffer);
    //закрыть копируемый файл 
    fclose(soursefile);
    //удалить копируемый файл
    if (unlink(sourcefilepath) != 0)
    {
        cout << "delete dest file error:" << strerror(errno) << "\r\n";
        dcout << "Exiting from AddDataToFile\r\n";
        return true;
    } else
    {
        cout << "delete dest file OK.\r\n";
    }
    dcout << "Exiting from AddDataToFile\r\n";
    return false;
}

bool DataFile::CheckFreeSpace(const char *diskpath, long mustHave) {
    struct statfs buf;
    statfs(diskpath, &buf);
    cout << "space free " << buf.f_bsize * buf.f_bavail << "\r\n";
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
    dcout << "Entering to CreateNewDataFile\r\n";
    auto t = time(NULL);
    char tmp[PATH_MAX];
    sprintf(tmp, "%s%011ld-data", PathToFileDir.c_str(), t);
    if ((File = fopen(tmp, "ab+")) == NULL)
    {
        cout << "Cannot open data file. " << strerror(errno) << "\r\n";
        return true;
    }
    cout << "creating new file" << tmp << "\r\n";
    dcout << "Exiting from CreateNewDataFile\r\n";
    return false;
}

DataFile::~DataFile() {
    dcout << "Distroing class DataFile\r\n";
}

