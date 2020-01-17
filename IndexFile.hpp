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

using namespace std;

class IndexFile {
public:
    IndexFile();
    IndexFile(const IndexFile& orig);
    virtual ~IndexFile();
private:
    FILE * currentFile;
    string FileName;

};

#endif /* INDEXFILE_HPP */

