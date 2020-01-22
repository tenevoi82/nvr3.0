/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   indexFile.hpp
 * Author: tenevoi
 *
 * Created on 9 января 2020 г., 17:13
 */

#ifndef INDEXFILE_HPP
#define INDEXFILE_HPP

#include <cstdio>
#include <string>
#include <iostream>

#include "structures.hpp"
#include "settings.hpp"

using namespace std;

class IndexFile {
public:
    IndexFile(string chName);
    bool AddData(struct videodatapart & data, const string & prefix);
    bool CreateFile(const string & prefix);
private:
    string chName;
    string FileName;
    string prefix;
    FILE * file = NULL;
    string PathToFileDir = strPathToArchive;    

};

#endif /* INDEXFILE_HPP */

