#ifndef READER_H
#define READER_H

#include "ThreadWorker.h"
#include "MessageQueue.h"

namespace pkus {

template< typename T >
class Reader : public ThreadWorker< T >
{
public:
     explicit Reader( MessageQueuePtr queue )
          : m_queue( queue )
     {}

private:
     virtual void work() override final {}; //подумать о доступе к очереди в параллельном потоке

private:
     MessageQueuePtr m_queue;
};

} // namespace pkus

#endif // READER_H
