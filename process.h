/**
 * Shutdown on Idle (SOI)
 * (c) 2013 - Mathias Rohnstock
 **/

#ifndef PROCESS_H
#define	PROCESS_H

#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <v8.h>
#include <node.h>
#include <pthread.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>

using namespace std;

// Each observed process, get their own object
class Process
{
    private:
        string name;            // name for the process, i.e. bash
        string description;     // description for the process, i.e. The GNU Bourne Again shell
        string binary;          // executed binary, i.e. /bin/bash
        string logfile;         // logfile of the process, i.e. /var/log/Xorg.0.log
        vector<int> pids;       // pids of the process, i.e. 0
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

        void addPid(int pid)
        {
            Process::pids.push_back(pid);
        }

        void removePid(int pid)
        {
            for (unsigned int i = 0; i < Process::getPidsSize(); i++)
            {
                if (Process::getPid(i) == pid)
                    Process::pids.erase(pids.begin() + i);
            }
        }

        void setStatus(int status)
        {
            Process::status = status;
        }

    public:
        Process(string name, string description, string binary, string logfile)
        {
            setName(name);
            setDescription(description);
            setBinary(binary);
            setLogfile(logfile);
            setStatus(-1);
            setPids();
        }

        void update()
        {
            Process::setPids();

            // perform any other update
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

        void setPids()
        {
            Process::pids.clear();
            stringstream cmd;
            cmd << "/bin/pidof " << Process::getBinary();
            FILE* pipe = popen(cmd.str().c_str(), "r");
            if (pipe)
            {
                char buffer[5]; // see /proc/sys/kernel/pid_max (default max 32768)
                char c;
                int i = 0;
                do
                {
                    c = fgetc(pipe);
                    if ((c == ' ' || c == EOF) && i != 0)
                    {
                        buffer[i] = '\0';
                        i = 0;
                        addPid(atoi(buffer));
                        continue;
                    }
                    else if (c != EOF)
                    {
                        buffer[i] = c;
                        i++;
                        continue;
                    }
                } while(c != EOF);
            }

            if (Process::getStatus() == -1)
            {
                if (Process::getPidsSize() > 0)
                    Process::setStatus(1);
                else
                    Process::setStatus(0);
            }
            else if (Process::getPidsSize() == 0)
                Process::setStatus(0);
            else if (Process::getPidsSize() != 0 && Process::getStatus() == 0)
                Process::setStatus(1);

            pclose(pipe);
        }

        int getPid(int index)
        {
            return Process::pids.at(index);
        }

        size_t getPidsSize()
        {
            return Process::pids.size();
        }

        int getStatus()
        {
            return Process::status;
        }

        string toString()
        {
            stringstream out;
            out << "Name: " << Process::getName() << ", Description: " << Process::getDescription() << ", Binary: " << Process::getBinary() << ", Logfile: " << Process::getLogfile();
            return out.str();
        }
};

#endif	/* PROCESS_H */

