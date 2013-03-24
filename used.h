/**
 * Shutdown on Idle (SOI)
 * (c) 2013 - Mathias Rohnstock
 **/

#ifndef USED_H
#define	USED_H

using namespace std;

// history of used system ressources
class Used
{
    private:
        time_t timestamp;
        vector<float> loads;    // loadavg between 1min
        long mem;               // free mem
        long swap;              // free swap
        unsigned short process; // running processes

        void setTimestamp(time_t timestamp)
        {
            Used::timestamp = timestamp;
        }

        void setLoads(long one, long five, long fifteen)
        {
            Used::loads.push_back(((float)one) / (float)(1 << SI_LOAD_SHIFT));
            Used::loads.push_back(((float)five) / (float)(1 << SI_LOAD_SHIFT));
            Used::loads.push_back(((float)fifteen) / (float)(1 << SI_LOAD_SHIFT));
        }

        void setMem(long mem)
        {
            Used::mem = mem;
        }

        void setSwap(long swap)
        {
            Used::swap = swap;
        }

        void setProcess(unsigned short process)
        {
            Used::process = process;
        }

    public:
        Used()
        {
            struct sysinfo info;
            time_t t = time(NULL);
            struct tm *current = NULL;
            current = localtime(&t);
            Used::setTimestamp(mktime(current));
            if (sysinfo(&info) != -1)
            {
                Used::setLoads(info.loads[0], info.loads[1], info.loads[2]);
                Used::setMem((info.totalram - info.freeram)/1024/1024);
                Used::setSwap((info.totalswap - info.freeswap)/1024/1024);
                Used::setProcess(info.procs);
            }
            else
            {
                Used::setLoads(-1, -1, -1);
                Used::setMem(-1);
                Used::setSwap(-1);
                Used::setProcess(-1);
            }
        }

        time_t getTimestamp()
        {
            return Used::timestamp;
        }

        float getLoad(int index)
        {
            return Used::loads.at(index);
        }

        long getMem()
        {
            return Used::mem;
        }

        long getSwap()
        {
            return Used::swap;
        }

        unsigned short getProcess()
        {
            return Used::process;
        }

};

#endif	/* USED_H */

