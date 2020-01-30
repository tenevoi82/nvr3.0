/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Scheduller.hpp
 * Author: tenevoi
 *
 * Created on 29 января 2020 г., 11:39
 */

#ifndef SCHEDULLER_HPP
#define SCHEDULLER_HPP

#include "Recorder.hpp"
#include "ChannelList.hpp"
#include "structures.hpp"
#include "settings.hpp"

#include <iostream>

class Scheduller {
public:
    Scheduller();
    Scheduller(const Scheduller& orig);
    void Init(Recorder*, ChannelList *);
    virtual ~Scheduller();
    void DelLast();
private:

    Recorder* recorder = NULL;
    ChannelList* channelList = NULL;
    
    
    
};

#endif /* SCHEDULLER_HPP */

