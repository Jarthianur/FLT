/*
 * VFRB.cpp
 *
 *  Created on: 23.04.2016
 *      Author: lula
 */

#include "VFRB.h"
#include "ParserADSB.h"
#include "ParserOGN.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <cstring>
#include <mutex>

std::mutex mutex;

std::condition_variable con_adsb_cond;
std::condition_variable con_ogn_cond;

VFRB::VFRB()
{
}

VFRB::~VFRB()
{
}

void VFRB::run(long double latitude, long double longitude,
        int altitude, int out_port,  int ogn_port,  int adsb_port,
        const char* ogn_host, const char* adsb_host, const char* user, const char* pass)
{
    ConnectOutNMEA out_con(out_port);
    ConnectInADSB adsb_con(adsb_host, adsb_port);
    ConnectInOGN ogn_con(ogn_host, ogn_port, user, pass);
    AircraftContainer ac_cont;

    try{
        std::thread adsb_in_thread(handle_adsb_in, latitude, longitude, altitude, std::ref(adsb_con), std::ref(ac_cont));
        std::thread ogn_in_thread(handle_ogn_in, latitude, longitude, altitude, std::ref(ogn_con), std::ref(ac_cont));

        std::thread con_out_thread(handle_con_out, std::ref(out_con));
        std::thread con_adsb_thread(handle_con_adsb, std::ref(adsb_con));
        std::thread con_ogn_thread(handle_con_ogn, std::ref(ogn_con));

        ParserADSB adsb_parser(latitude, longitude, altitude);
        ParserOGN ogn_parser(latitude, longitude, altitude);
        std::string str;

        while (1) {
            ac_cont.invalidateAircrafts();

            for (Aircraft* ac : ac_cont.getContainer()) {
                if (ac->aircraft_type == -1) {
                    adsb_parser.process(ac, str);
                } else {
                    ogn_parser.process(ac, str);
                }
                if (out_con.sendMsgOut(str) <= 0) {
                    //std::cout << "client disconnected " << std::endl;
                    out_con.closeClient();
                }
            }
            adsb_parser.gprmc(str);
            if (out_con.sendMsgOut(str) <= 0) {
                //std::cout << "client disconnected " << std::endl;
                out_con.closeClient();
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        adsb_in_thread.join();
        ogn_in_thread.join();
        con_out_thread.join();
        con_adsb_thread.join();
        con_ogn_thread.join();

    } catch (std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return;
    }
    return;
}

void VFRB::handle_con_out(ConnectOutNMEA& out_con)
{
    if (out_con.listenOut() == -1) return;
    while (1) {
        //std::cout << "con out thread runs " << std::endl;
        out_con.connectClient();
    }
    return;
}

void VFRB::handle_con_adsb(ConnectInADSB& adsb_con)
{
    if (adsb_con.setupConnectIn() == -1) return;

    while (adsb_con.connectIn() == -1) {
        //std::cout << "waiting for adsb-server"<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    //std::cout << "con adsb thread called notify " << std::endl;
    con_adsb_cond.notify_one();

    while (1) {
        std::unique_lock<std::mutex> lock(mutex);
        con_adsb_cond.wait(lock);
        lock.unlock();
        while (adsb_con.connectIn() == -1) {
            adsb_con.close();
            adsb_con.setupConnectIn();
            //std::cout << "waiting for adsb-server"<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        //std::cout << "con adsb thread called notify " << std::endl;
        con_adsb_cond.notify_one();
    }
    return;
}

void VFRB::handle_con_ogn(ConnectInOGN& ogn_con)
{
    if (ogn_con.setupConnectIn() == -1) return;

    while (ogn_con.connectIn() == -1) {
        //std::cout << "waiting for ogn-server"<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    //std::cout << "con ogn thread called notify " << std::endl;
    con_ogn_cond.notify_one();

    while (1) {
        std::unique_lock<std::mutex> lock(mutex);
        con_ogn_cond.wait(lock);
        lock.unlock();
        while (ogn_con.connectIn() == -1) {
            ogn_con.close();
            ogn_con.setupConnectIn();
            //std::cout << "waiting for adsb-server"<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        //std::cout << "con ogn thread called notify " << std::endl;
        con_ogn_cond.notify_one();
    }
    return;
}

void VFRB::handle_adsb_in(long double latitude, long double longitude, int altitude, ConnectInADSB& adsb_con, AircraftContainer& ac_cont)
{
    ParserADSB parser(latitude, longitude, altitude);
    std::unique_lock<std::mutex> lock(mutex);
    con_adsb_cond.wait(lock);
    lock.unlock();

    //std::cout << "Scan for incoming adsb-msgs..." << std::endl;

    while (1) {
        if (adsb_con.readLineIn(adsb_con.getAdsbInSock()) <= 0) {
            con_adsb_cond.notify_one();
            std::unique_lock<std::mutex> lock(mutex);
            con_adsb_cond.wait(lock);
            //std::cout << "Scan for incoming adsb-msgs..." << std::endl;
            lock.unlock();
        }
        //need msg3 only
        if (adsb_con.getResponse().at(4) == '3') {
            parser.unpack(adsb_con.getResponse(), ac_cont);
        }
    }
    return;
}

void VFRB::handle_ogn_in(long double latitude, long double longitude, int altitude, ConnectInOGN& ogn_con, AircraftContainer& ac_cont)
{
    ParserOGN parser(latitude, longitude, altitude);
    std::unique_lock<std::mutex> lock(mutex);
    con_ogn_cond.wait(lock);
    lock.unlock();

    //std::cout << "Scan for incoming ogn-msgs..." << std::endl;

    while (1) {
        if (ogn_con.readLineIn(ogn_con.getOgnInSock()) <= 0) {
            con_ogn_cond.notify_one();
            std::unique_lock<std::mutex> lock(mutex);
            con_ogn_cond.wait(lock);
            //std::cout << "Scan for incoming ogn-msgs..." << std::endl;
            lock.unlock();
        }
        parser.unpack(ogn_con.getResponse(), ac_cont);
    }
    return;
}
