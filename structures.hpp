/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   structures.hpp
 * Author: tenevoi
 *
 * Created on 26 декабря 2019 г., 12:19
 */

#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <cmath>


struct segment_timecontext {
    int index;
    char filename[128];
    long double start;
    float duration;
    long double end;
    char source_dir[512];
    char camname[32];
};

#pragma pack(push,1)
struct videodatapart{
    double startTime;
    float duration;
    long pos;
    long size;
};
#pragma pack(pop)

#pragma pack(push,1)
struct channels_item{
    char chName[128];
    int id;
    double firstStart;
};
#pragma pack(pop)


#endif /* STRUCTURES_HPP */

