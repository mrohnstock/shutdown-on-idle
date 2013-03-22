/**
 * Shutdown on Idle (SOI)
 * (c) 2013 - Mathias Rohnstock
 **/

#ifndef PROCESS_H
#define	PROCESS_H

#include <string>
#include <string.h>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <v8.h>
#include <node.h>
#include <pthread.h>

using namespace std;

// Each observed process, get their own object
class Process
{
    private:
        string name;            // name for the process, i.e. bash
        string description;     // description for the process, i.e. The GNU Bourne Again shell
        string binary;          // executed binary, i.e. /bin/bash
        string logfile;         // logfile of the process, i.e. /var/log/Xorg.0.log
        int pid;                // pid of the process, i.e. 0
        int status;             // status of the process, i.e. -1: unknown, 0: stopped, 1: running, 2: idle

        void setName(string name)
        {
            Process::name = name;
        }

        void setDescription(string description)
        {
            Process::description = description;
        }

        void setBinary(string binary)
        {
            Process::binary = binary;
        }

        void setLogfile(string logfile)
        {
            Process::logfile = logfile;
        }

        void setPid(int pid)
        {
            Process::pid = pid;
        }

        void setStatus(int status)
        {
            Process::status = status;
        }

    public:
        Process(string name)
        {
            setName(name);
            setDescription("");
            setBinary("");
            setLogfile("");
            setPid(-1);
            setStatus(-1);
        }

        string getName()
        {
            return Process::name;
        }

        string getDescription()
        {
            return Process::description;
        }

        string getBinary()
        {
            return Process::binary;
        }

        string getLogfile()
        {
            return Process::logfile;
        }

        int getPid()
        {
            return Process::pid;
        }

        int getStatus()
        {
            return Process::status;
        }
};

#endif	/* PROCESS_H */

