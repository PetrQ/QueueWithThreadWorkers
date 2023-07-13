#ifndef ITHREADWORKER_H
#define ITHREADWORKER_H

class IThreadWorker
{
public:
     virtual start( bool val ) = 0;
     virtual stop( bool val ) = 0;
};

#endif // ITHREADWORKER_H
