#ifndef READER_H
#define READER_H

#include "MessageQueue.h"
#include "IThreadWorker.h"

namespace pkus {

template< typename T >
class ReaderWorker
{
public:
     explicit ReaderWorker( MessageQueuePtr< T > queue, int msDelay = 0 );
     ~ReaderWorker();

     static int instanceId();

private:
     void thread_work();

private:
     int m_msDelay;
     std::thread m_pThread;
     std::atomic_bool m_work { false };
     MessageQueuePtr< T > m_queue;

     int m_id = 0;
};

template< typename T >
ReaderWorker< T >::ReaderWorker( MessageQueuePtr< T > queue, int msDelay )
     : m_msDelay( msDelay )
     , m_queue( queue )
     , m_id( ReaderWorker< T >::instanceId() )
{
     m_work = true;
     m_pThread = std::thread( &ReaderWorker::thread_work, std::ref( *this ) );

     std::stringstream ss;
     ss << " Strat worker" << m_msDelay << ' ' << m_id << ' ' << m_pThread.get_id() << " run ";
     logg( ss.str(), this );
}

template< typename T >
ReaderWorker< T >::~ReaderWorker()
{
     if( m_pThread.joinable() )
     {
          std::stringstream ss;
          ss << " Stop worker" << m_msDelay << ' ' << m_id << ' ' << m_pThread.get_id();
          logg( ss.str(), this );

          m_work = false;
          m_pThread.join();
     }
}

template< typename T >
void ReaderWorker< T >::thread_work()
{
     while( m_work )
     {
          T message {}; //очищаем предыдущее состояние
          RetCode ret = m_queue->threadGet( message );

          if( ret == RetCode::UNAVAILABLE )
               continue;

          std::stringstream ss;
          ss << " Reader " << m_msDelay << ' ' << m_id << " get message " << message;
          ss << " ReturnCode " << static_cast< int >( ret );
          logg( ss.str() );

          if( ret == RetCode::STOPPED )
          {
               m_work = false;
          }

          std::this_thread::sleep_for( std::chrono::milliseconds( m_msDelay ) );
     }
}

template< typename T >
int ReaderWorker< T >::instanceId()
{
     static int id_counter = 0;
     return id_counter++;
}

} // namespace pkus

#endif // READER_H
