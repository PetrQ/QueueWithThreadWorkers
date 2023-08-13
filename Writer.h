#ifndef WRITER_H
#define WRITER_H

#include "ThreadWorker.h"
#include "MessageQueue.h"

namespace pkus {

template< typename T >
class Writer : public ThreadWorker< T >
{
public:
     explicit Writer( MessageQueuePtr queue )
          : m_queue( queue )
     {}

     void pause( bool val ) override {};

private:
     virtual void work() override final {}; //подумать о доступе к очереди в параллельном потоке

private:
     MessageQueuePtr m_queue;
};

} // namespace pkus

#endif // WRITER_H
