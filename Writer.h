#ifndef WRITER_H
#define WRITER_H

#include "ThreadWorker.h"
#include "MessageQueue.h"

namespace pkus {

template< typename T >
class Writer : public ThreadWorker< T >
{
public:
     explicit Writer( MessageQueuePtr< T > queue, int msDelay )
          : ThreadWorker< T >( msDelay )
          , m_queue( queue )
     {}

private:
     virtual void work() override final
     {
          T message;
          Writer::create_message( message, ++m_counter );
          RetCode ret = m_queue->put( message );

          std::stringstream ss;
          ss << " Writer" << ThreadWorker< T >::m_msDelay << " put message " << message;
          ss << " ReturnCode " << static_cast< int >( ret );
          logg( ss.str() );
     }; //подумать о доступе к очереди в параллельном потоке

     void create_message( T& message, std::uint32_t count );

private:
     MessageQueuePtr< T > m_queue;
     std::uint32_t m_counter = 0;
};

template< typename T >
void Writer< T >::create_message( T& message, std::uint32_t count )
{
     logg( "unknown type, can't create message" );
}

template<>
void Writer< int >::create_message( int& message, std::uint32_t count ) //спецификация шаблона для int
{
     message = m_msDelay * 100 + static_cast< int >( count );
}

template<>
void Writer< std::string >::create_message( std::string& message,
                                            std::uint32_t count ) //спецификация шаблона для std::string
{
     std::stringstream ss;
     ss << " Messge number " << count << " from writer " << std::this_thread::get_id();
     message = ss.str();
}

} // namespace pkus

#endif // WRITER_H
