#ifndef READER_H
#define READER_H

#include "ThreadWorker.h"
#include "MessageQueue.h"

namespace pkus {

template< typename T >
class Reader : public ThreadWorker< T >
{
public:
     explicit Reader( MessageQueuePtr< T > queue, int msDelay )
          : ThreadWorker< T >( msDelay )
          , m_queue( queue )
     {}

private:
     virtual void work() override final
     {
          T message;
          RetCode ret = m_queue->get( message );
          Reader::handle_message( message, ret );
     }; //подумать о доступе к очереди в параллельном потоке

     static void handle_message( T& message, RetCode ret )
     {
          std::stringstream ss;
          ss << " Reader " << std::this_thread::get_id() << " get message " << message;
          ss << " ReturnCode " << static_cast< int >( ret );
          logg( ss.str() );
     }

private:
     MessageQueuePtr< T > m_queue;
};

} // namespace pkus

#endif // READER_H
