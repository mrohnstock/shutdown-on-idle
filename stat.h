/**
 * Shutdown on Idle (SOI)
 * (c) 2013 - Mathias Rohnstock
 **/

#ifndef STAT_H
#define	STAT_H

using namespace std;

// history of used system ressources
class Stat
{
    private:
        time_t timestamp;
        vector<float> loads;    // loadavg between 1min
        long mem;               // free mem
        long swap;              // free swap
        unsigned short process; // running processes

        void setTimestamp(time_t timestamp)
        {
            Stat::timestamp = timestamp;
        }

        void setLoads(long one, long five, long fifteen)
        {
            Stat::loads.push_back(((float)one) / (float)(1 << SI_LOAD_SHIFT));
            Stat::loads.push_back(((float)five) / (float)(1 << SI_LOAD_SHIFT));
            Stat::loads.push_back(((float)fifteen) / (float)(1 << SI_LOAD_SHIFT));
        }

        void setMem(long mem)
        {
            Stat::mem = mem;
        }

        void setSwap(long swap)
        {
            Stat::swap = swap;
        }

        void setProcess(unsigned short process)
        {
            Stat::process = process;
        }

    public:
        Stat()
        {
            struct sysinfo info;
            time_t t = time(NULL);
            struct tm *current = NULL;
            current = localtime(&t);
            Stat::setTimestamp(mktime(current));
            if (sysinfo(&info) != -1)
            {
                Stat::setLoads(info.loads[0], info.loads[1], info.loads[2]);
                Stat::setMem((info.totalram - info.freeram)/1024/1024);
                Stat::setSwap((info.totalswap - info.freeswap)/1024/1024);
                Stat::setProcess(info.procs);
            }
            else
            {
                Stat::setLoads(-1, -1, -1);
                Stat::setMem(-1);
                Stat::setSwap(-1);
                Stat::setProcess(-1);
            }
        }

        time_t getTimestamp()
        {
            return Stat::timestamp;
        }

        float getLoad(int index)
        {
            return Stat::loads.at(index);
        }

        long getMem()
        {
            return Stat::mem;
        }

        long getSwap()
        {
            return Stat::swap;
        }

        unsigned short getProcess()
        {
            return Stat::process;
        }

};

#endif	/* STAT_H */

