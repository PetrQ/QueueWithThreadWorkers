#ifndef ITHREADWORKER_H
#define ITHREADWORKER_H

#include <memory>
#include <chrono>
#include <sstream>
#include <iostream>
#include <mutex>

const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
std::mutex cout_lock;

void logg( const std::string& mess, void* const adress = nullptr )
{
     std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
     double msec = std::chrono::duration_cast< std::chrono::milliseconds >( now - start ).count();

     std::stringstream ss;
     ss << "ms " << msec << " " << mess;
     if( adress )
          ss << " from: " << adress;
     ss << std::endl;

     {
          std::lock_guard< std::mutex > lock( cout_lock );
          std::cerr << ss.str() << ::std::flush;
     }
}

namespace pkus {

class IThreadWorker
{
public:
     virtual void start( bool val ) = 0;
     virtual void pause( bool val ) = 0;
};

using WorkerHandler = std::shared_ptr< IThreadWorker >;
using WeakWorkerHandler = std::weak_ptr< IThreadWorker >;

} // namespace pkus

#endif // ITHREADWORKER_H
