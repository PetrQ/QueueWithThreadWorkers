#ifndef ITHREADWORKER_H
#define ITHREADWORKER_H

#include <memory>
#include <chrono>
#include <sstream>
#include <iostream>
#include <mutex>

void logg( const std::string& mess, void* const adress = nullptr );

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
