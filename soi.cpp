/**
 * Shutdown on Idle (SOI)
 * (c) 2013 - Mathias Rohnstock
 **/

#include "process.h"

#define VERSION     "0.1-dev"

// main program
class SOI : node::ObjectWrap
{
    private:
        vector<Process *> processes;

        // pthread
        volatile bool m_stoprequested;
        volatile bool m_running;
        pthread_mutex_t m_mutex;
        pthread_t m_thread;

        void setProcesses(Process *p)
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
                // do something...
                pthread_mutex_lock(&m_mutex);
                pthread_mutex_unlock(&m_mutex);
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
        }

        void addProcess(string name)
        {
            Process *p = new Process(name);
            SOI::setProcesses(p);
        }

        Process *getProcess(int index)
        {
            return(SOI::processes.at(index));
        }

        int getProcessesSize()
        {
            return(SOI::processes.size());
        }

        int shutdown()
        {
            return system("shutdown -h now &");
        }

        void removeProcess(string name)
        {
            // TODO
        }

        string getVersion()
        {
            return string(VERSION);
        }

        string getProcesses()
        {
            stringstream out;
            for (int i = 0; i < SOI::getProcessesSize(); i++)
            {
              out << SOI::getProcess(i)->getName() << "\n";
            }
            return out.str();
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
            //soi->go();
            soi->Wrap(args.This());
            return args.This();
        }

        static v8::Handle<v8::Value> add(const v8::Arguments& args)
        {
            SOI* soi = node::ObjectWrap::Unwrap<SOI>(args.This());
            v8::String::Utf8Value param(args[0]->ToString());
            std::string input = std::string(*param);

            if (input.compare("undefined") == 0)
                return v8::String::New("at least on param is missing");

            soi->addProcess(input);

            return v8::String::New("process added");
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
            for (int i = 0; i < soi->getProcessesSize(); i++)
            {
                v8::Handle<v8::Array> process = v8::Array::New(5); // 6 values
                process->Set(v8::String::New("name"),        v8::String::New(soi->getProcess(i)->getName().c_str()));
                process->Set(v8::String::New("description"), v8::String::New(soi->getProcess(i)->getDescription().c_str()));
                process->Set(v8::String::New("binary"),      v8::String::New(soi->getProcess(i)->getBinary().c_str()));
                process->Set(v8::String::New("logfile"),     v8::String::New(soi->getProcess(i)->getLogfile().c_str()));
                process->Set(v8::String::New("pid"),         v8::Integer::New(soi->getProcess(i)->getPid()));
                process->Set(v8::String::New("status"),      v8::Integer::New(soi->getProcess(i)->getStatus()));
                value->Set(i, process);
            }
            return value;
        }

        static v8::Handle<v8::Value> shutdown(const v8::Arguments& args)
        {
            SOI* soi = node::ObjectWrap::Unwrap<SOI>(args.This());
            soi->shutdown();
            return v8::String::New("good bye ...");
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
