#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include "ImessageQueueEvents.h"
#include "RingBuffer.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace pkus {

enum class RetCode
{
     OK = 0,
     HWM = -1,
     NO_SPACE = -2,
     STOPPED = -3,
     UNAVAILABLE = -4
};

template< typename T >
class MessageQueue
{
public:
     explicit MessageQueue( int queue_size, int lwm, int hwm )
          : m_container( RingBuffer< T >( queue_size ) )
          , m_hwm( hwm )
          , m_lwm( lwm )
          , m_isStrted( false )
          , m_not_empty( false ) {};

     ~MessageQueue()
     {
          std::cout << "FIN QUEUE size " << m_container.size() << std::endl;
          //даем сообщение об остановке читателем
     }

     void setEvents( IMessageQueueEventsPtr events )
     {
          m_events = events;
     };

     void run()
     {
          if( !m_events )
               return;
          m_isStrted = true;
          m_events->on_start();
     }
     void stop()
     {
          if( !m_events )
               return;
          m_isStrted = false;
          m_events->on_stop();

          //даем сообщение об остановке читателем, они проснутся сами по таймауту
          m_not_empty = true;
     };
     RetCode put( const T& message )
     {
          if( !m_isStrted )
               return RetCode::STOPPED;

          std::lock_guard< std::mutex > lk( m_mutex );
          try
          {
               m_container.pushBack( message );
          }
          catch( const std::range_error& ex )
          {
               return RetCode::NO_SPACE;
          }
          m_not_empty = m_container.size();
          m_cv.notify_one();

          std::cout << "PUT size " << m_container.size() << std::endl;
          if( m_container.size() >= m_hwm )
          {
               m_events->on_hwm();
               return RetCode::HWM;
          }
          return RetCode::OK;
     }
     RetCode get( T& message )
     {
          //         std::lock_guard< std::mutex > lk( m_mutex );
          if( !m_isStrted )
          {
               return RetCode::STOPPED;
          }

          message = m_container.popFront();
          std::cout << "GET size " << m_container.size() << std::endl;
          if( m_container.size() <= m_lwm )
          {
               m_events->on_lwm();
          }
          m_not_empty = m_container.size();
          return RetCode::OK;
     }

     RetCode threadGet( T& message )
     {
          std::unique_lock< std::mutex > lock { m_mutex };

          m_cv.wait_for(
               lock, std::chrono::milliseconds( 100 ), [ this ]() { return static_cast< bool >( m_not_empty ); } );
          if( m_not_empty )
          {
               return get( message );
          }

          return RetCode::UNAVAILABLE;
     }

private:
     IMessageQueueEventsPtr m_events;
     RingBuffer< T > m_container;

     std::atomic_bool m_isStrted;
     std::atomic_bool m_not_empty;
     std::uint32_t m_hwm, m_lwm; // HighWaterMark, LowWaterMark

     std::mutex m_mutex;
     std::condition_variable m_cv;
};

template< typename T >
using MessageQueuePtr = ::std::shared_ptr< MessageQueue< T > >;

} // namespace pkus

#endif // MESSAGEQUEUE_H
