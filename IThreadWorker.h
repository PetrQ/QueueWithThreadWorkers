#ifndef ITHREADWORKER_H
#define ITHREADWORKER_H

#include <memory>

class IThreadWorker
{
public:
     virtual void start( bool val ) = 0;
     virtual void pause( bool val ) = 0;
};

using WorkerHandler = std::shared_ptr< IThreadWorker >;
using WeakWorkerHandler = std::weak_ptr< IThreadWorker >;

#endif // ITHREADWORKER_H
