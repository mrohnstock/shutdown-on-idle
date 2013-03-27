/**
 * Shutdown on Idle (SOI)
 * (c) 2013 - Mathias Rohnstock
 **/

#include "process.h"
#include "stat.h"

#define VERSION "0.1-dev"
#define MAXHOLD 6   // hold max loads (60min)
#define SLEEP   600 // sleep sec (10min)

// main program
class SOI : node::ObjectWrap
{
    private:
        vector<Process *> processes;
        vector<Stat *> stats;

        // pthread
        volatile bool m_stoprequested;
        volatile bool m_running;
        pthread_mutex_t m_mutex;
        pthread_t m_thread;

        void addProcess(Process *p)
        {
            SOI::processes.push_back(p);
        }

        static void *start_thread(void *obj)
        {
            reinterpret_cast<SOI *>(obj)->do_work();
            return NULL;
        }

        void do_work()
        {
            while (!m_stoprequested)
            {
                addStat();
                pthread_mutex_lock(&m_mutex);
                pthread_mutex_unlock(&m_mutex);
                sleep(SLEEP);
            }
        }

    public:
        static v8::Persistent<v8::FunctionTemplate> s_ct;
        static void Init(v8::Handle<v8::Object> target)
        {
            v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(New);

            s_ct = v8::Persistent<v8::FunctionTemplate>::New(t);
            s_ct->InstanceTemplate()->SetInternalFieldCount(1);
            s_ct->SetClassName(v8::String::NewSymbol("SOI"));

            NODE_SET_PROTOTYPE_METHOD(s_ct, "version",     version);
            NODE_SET_PROTOTYPE_METHOD(s_ct, "add",         add);
            NODE_SET_PROTOTYPE_METHOD(s_ct, "shutdown",    shutdown);
            NODE_SET_PROTOTYPE_METHOD(s_ct, "hostname",    hostname);
            NODE_SET_PROTOTYPE_METHOD(s_ct, "system",      sys);
            NODE_SET_PROTOTYPE_METHOD(s_ct, "processlist", processlist);

            target->Set(v8::String::NewSymbol("SOI"),
                s_ct->GetFunction());
        }

        SOI()
        {
            // init pthread
            m_stoprequested = false;
            m_running = false;

            pthread_mutex_init(&m_mutex, NULL);
        }

        ~SOI()
        {
            stop();
            for(unsigned int i = 0; i < SOI::getProcessesSize(); i++)
                delete(SOI::getProcess(i));

            for(unsigned int i = 0; i < SOI::getStatsSize(); i++)
                delete(SOI::getStat(i));
        }

        bool addProcess(const v8::Arguments& args)
        {
            v8::String::Utf8Value name(args[0]->ToString());
            v8::String::Utf8Value description(args[1]->ToString());
            v8::String::Utf8Value binary(args[2]->ToString());
            v8::String::Utf8Value logfile(args[3]->ToString());

            if ( ! SOI::processExists(string(*name)))
            {
                Process *p = new Process(string(*name), string(*description), string(*binary), string(*logfile));
                SOI::addProcess(p);
                return true;
            }
            return false;
        }

        void addStat()
        {
            Stat *s = new Stat();
            if (SOI::getStatsSize() > MAXHOLD)
            {
                delete(SOI::stats.front()); // remove first item
                SOI::stats.erase(SOI::stats.begin());
            }
            SOI::stats.push_back(u);
        }

        Process *getProcess(int index)
        {
            return(SOI::processes.at(index));
        }

        Stat *getStat(int index)
        {
            return(SOI::stats.at(index));
        }

        bool processExists(string name)
        {
            for (unsigned int i = 0; i < SOI::getProcessesSize(); i++)
            {
                if (SOI::getProcess(i)->getName().compare(name) == 0)
                    return true;
            }
            return false;
        }

        size_t getProcessesSize()
        {
            return(SOI::processes.size());
        }

        size_t getStatsSize()
        {
            return(SOI::stats.size());
        }

        int shutdown()
        {
            return system("shutdown -h now &");
        }

        void removeProcess(string name)
        {
            for (unsigned int i = 0; i < SOI::getProcessesSize(); i++)
            {
                if(SOI::getProcess(i)->getName().compare(name) == 0)
                {
                    delete(SOI::getProcess(i));
                    SOI::processes.erase(SOI::processes.begin() + i);
                }
            }
        }

        string getVersion()
        {
            return string(VERSION);
        }

        string getHostname()
        {
            char hostname[255];
            if (gethostname(hostname, 255) == 0)
                return string(hostname);
            return string("can't receive hostname!");
        }

        string getOS()
        {
            char version[255];
            struct utsname buf;

            if (uname(&buf) != -1)
              sprintf(version, "%s %s", buf.sysname, buf.release);
            else
              sprintf(version, "unknown OS");

            return string(version);
        }

        long getUptime()
        {
            struct sysinfo info;
            if (sysinfo(&info) != -1)
              return info.uptime;
            return -1;
        }

        long getMem()
        {
            struct sysinfo info;
            if (sysinfo(&info) != -1)
              return info.totalram;
            return -1;
        }

        long getSwap()
        {
            struct sysinfo info;
            if (sysinfo(&info) != -1)
              return info.totalswap;
            return -1;
        }

        void go()
        {
            assert(m_running == false);
            m_running = true;
            m_stoprequested = false;
            pthread_create(&m_thread, 0, &SOI::start_thread, this);
        }

        void stop()
        {
            assert(m_running == true);
            m_running = false;
            m_stoprequested = true;
            pthread_join(m_thread, 0);
        }

        static v8::Handle<v8::Value> New(const v8::Arguments& args)
        {
            SOI* soi = new SOI();
            soi->go();
            soi->Wrap(args.This());
            return args.This();
        }

        static v8::Handle<v8::Value> add(const v8::Arguments& args)
        {
            SOI* soi = node::ObjectWrap::Unwrap<SOI>(args.This());
            return v8::Boolean::New(soi->addProcess(args));
        }

        static v8::Handle<v8::Value> version(const v8::Arguments& args)
        {
            SOI* soi = node::ObjectWrap::Unwrap<SOI>(args.This());

            return v8::String::New(soi->getVersion().c_str());
        }

        static v8::Handle<v8::Value> processlist(const v8::Arguments& args)
        {
            SOI* soi = node::ObjectWrap::Unwrap<SOI>(args.This());

            v8::Handle<v8::Array> value = v8::Array::New(soi->getProcessesSize());
            for (unsigned int i = 0; i < soi->getProcessesSize(); i++)
            {
                v8::Handle<v8::Array> process = v8::Array::New(); // 6 values
                process->Set(v8::String::New("name"),        v8::String::New(soi->getProcess(i)->getName().c_str()));
                process->Set(v8::String::New("description"), v8::String::New(soi->getProcess(i)->getDescription().c_str()));
                process->Set(v8::String::New("binary"),      v8::String::New(soi->getProcess(i)->getBinary().c_str()));
                process->Set(v8::String::New("logfile"),     v8::String::New(soi->getProcess(i)->getLogfile().c_str()));
                v8::Handle<v8::Array> pids = v8::Array::New(soi->getProcess(i)->getPidsSize());
                for (unsigned int j = 0; j < soi->getProcess(i)->getPidsSize(); j++)
                    process->Set(j, v8::Integer::New(soi->getProcess(i)->getPid(j)));
                process->Set(v8::String::New("pids"),        pids);
                process->Set(v8::String::New("status"),      v8::Integer::New(soi->getProcess(i)->getStatus()));
                value->Set(i, process);
            }
            return value;
        }

        static v8::Handle<v8::Value> shutdown(const v8::Arguments& args)
        {
            SOI* soi = node::ObjectWrap::Unwrap<SOI>(args.This());
            return v8::Integer::New(soi->shutdown());
        }

        static v8::Handle<v8::Value> hostname(const v8::Arguments& args)
        {
            SOI* soi = node::ObjectWrap::Unwrap<SOI>(args.This());
            return v8::String::New(soi->getHostname().c_str());
        }

        static v8::Handle<v8::Value> sys(const v8::Arguments& args)
        {
            SOI* soi = node::ObjectWrap::Unwrap<SOI>(args.This());

            v8::Handle<v8::Array> system = v8::Array::New();
            system->Set(v8::String::New("hostname"), v8::String::New(soi->getHostname().c_str()));
            system->Set(v8::String::New("os"),       v8::String::New(soi->getOS().c_str()));
            system->Set(v8::String::New("mem"),      v8::Number::New(soi->getMem()));
            system->Set(v8::String::New("swap"),     v8::Number::New(soi->getSwap()));
            system->Set(v8::String::New("uptime"),   v8::Number::New(soi->getUptime()));
            v8::Handle<v8::Array> stats = v8::Array::New(soi->getStatsSize());
            for (unsigned int i = 0; i < soi->getStatsSize(); i++)
            {
                v8::Handle<v8::Array> loads = v8::Array::New(); // 1min, 5min, 15min
                loads->Set(v8::String::New("onemin"),     v8::Number::New(soi->getStat(i)->getLoad(0)));
                loads->Set(v8::String::New("fivemin"),    v8::Number::New(soi->getStat(i)->getLoad(1)));
                loads->Set(v8::String::New("fifteenmin"), v8::Number::New(soi->getStat(i)->getLoad(2)));
                v8::Handle<v8::Array> stat = v8::Array::New();
                stat->Set(v8::String::New("timestamp"),   v8::Integer::New(soi->getStat(i)->getTimestamp()));
                stat->Set(v8::String::New("loads"),       loads);
                stat->Set(v8::String::New("mem"),         v8::Number::New(soi->getStat(i)->getMem()));
                stat->Set(v8::String::New("swap"),        v8::Number::New(soi->getStat(i)->getSwap()));
                stat->Set(v8::String::New("process"),     v8::Number::New(soi->getStat(i)->getProcess()));
                stats->Set(i, stat);
            }
            system->Set(v8::String::New("stats"), stats);
            return system;
        }
};

v8::Persistent<v8::FunctionTemplate> SOI::s_ct;

extern "C"
{
    static void init (v8::Handle<v8::Object> target)
    {
        SOI::Init(target);
    }

    NODE_MODULE(soi, init);
}
