/**
 * Shutdown on Idle (SOI)
 * (c) 2013 - Mathias Rohnstock
 **/

#include <string>
#include <string.h>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <v8.h>
#include <node.h>
#include <pthread.h>

#define VERSION     "0.1-dev"

using namespace std;

class SOI : node::ObjectWrap
{
    private:
        // pthread
        volatile bool m_stoprequested;
        volatile bool m_running;
        pthread_mutex_t m_mutex;
        pthread_t m_thread;
        
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
            v8::HandleScope scope;
            
            v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New(New);
            
            s_ct = v8::Persistent<v8::FunctionTemplate>::New(t);
            s_ct->InstanceTemplate()->SetInternalFieldCount(1);
            s_ct->SetClassName(v8::String::NewSymbol("SOI"));

            NODE_SET_PROTOTYPE_METHOD(s_ct, "version", version);
            
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
        
        string getVersion()
        {
            stringstream version;
            version << "Shutdown on Idle - version " << VERSION;
            return version.str();
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
            v8::HandleScope scope;
            SOI* soi = new SOI();
            soi->go();
            soi->Wrap(args.This());
            return args.This();
        }

        static v8::Handle<v8::Value> version(const v8::Arguments& args)
        {
            SOI* soi = node::ObjectWrap::Unwrap<SOI>(args.This());

            return v8::String::New(soi->getVersion().c_str());
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
