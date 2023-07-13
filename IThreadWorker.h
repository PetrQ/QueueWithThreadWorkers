#ifndef ITHREADWORKER_H
#define ITHREADWORKER_H

class IThreadWorker
{
public:
     virtual void start( bool val ) = 0;
     virtual void stop( bool val ) = 0;
};

#endif // ITHREADWORKER_H
