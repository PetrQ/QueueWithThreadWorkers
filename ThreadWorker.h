#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <thread>
#include <atomic>
#include <condition_variable>

#include <chrono>
#include <sstream>
#include <iostream>

#include "IThreadWorker.h"

const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
std::mutex cout_lock;

void logg( const std::string& mess, void* const adress = nullptr )
{
     std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
     double msec = std::chrono::duration_cast< std::chrono::milliseconds >( now - start ).count();

     std::stringstream ss;
     ss << "ms " << msec << " " << mess;
     if( adress )
          ss << " from: " << adress;
     ss << std::endl;

     {
          std::lock_guard< std::mutex > lock( cout_lock );
          std::cerr << ss.str() << ::std::flush;
     }
}

//https://dev.to/glpuga/multithreading-by-example-the-stuff-they-didn-t-tell-you-4ed8

template< typename T >
class ThreadWorker : public IThreadWorker
{
protected:
     int m_msDelay;

     std::thread m_pThread;
     std::atomic_bool m_pause { false };
     std::atomic_bool m_work { false };
     std::mutex m_mutex;
     std::condition_variable m_cv;
     const std::chrono::milliseconds m_max_sleep_interval { 100 };

public:
     explicit ThreadWorker( int msDelay );
     virtual ~ThreadWorker();

     void start( bool val ) override final;
     void pause( bool val ) override final;

private:
     virtual std::thread::id run(); //запускает run_safe() в потоке
     virtual void work( bool pause )
     {
          std::stringstream ss;
          ss << " Worker" << m_msDelay << ' ' << std::this_thread::get_id() << " DO " << pause;
          logg( ss.str(), this );
     };
     void thread_work();
};

template< typename T >
ThreadWorker< T >::ThreadWorker( int msDelay )
     : m_msDelay( msDelay )
{}

template< typename T >
ThreadWorker< T >::~ThreadWorker()
{
     if( m_pThread.joinable() )
     {
          std::stringstream ss;
          ss << " Stop worker" << m_msDelay << ' ' << m_pThread.get_id();
          logg( ss.str(), this );

          m_work = false;
          m_cv.notify_all();
          m_pThread.join();
     }
}

template< typename T >
void ThreadWorker< T >::start( bool val )
{
     if( m_work == val )
          return;

     m_work = val;
     if( val )
     {
          run(); //старт потока
     }
     else
     {
          std::stringstream ss;
          ss << " Stop worker" << m_msDelay << ' ' << m_pThread.get_id();
          logg( ss.str(), this );

          m_cv.notify_all();
          m_pThread.join();
     }
}

template< typename T >
void ThreadWorker< T >::pause( bool val )
{
     std::stringstream ss;
     ss << " Pause worker" << m_msDelay << ' ' << m_pThread.get_id();
     logg( ss.str(), this );

     m_pause = val;
     m_cv.notify_all();
}

template< typename T >
std::thread::id ThreadWorker< T >::run()
{
     //создаем новый поток
     m_pThread = std::thread( &ThreadWorker::thread_work, std::ref( *this ) );

     std::stringstream ss;
     ss << " Strat worker" << m_msDelay << ' ' << m_pThread.get_id() << " run ";
     logg( ss.str(), this );

     return m_pThread.get_id();
}

template< typename T >
void ThreadWorker< T >::thread_work()
{
     std::unique_lock< std::mutex > lock { m_mutex };
     while( m_work )
     {
          m_cv.wait_for( lock, m_max_sleep_interval, [ this ]() { return static_cast< bool >( !m_pause ); } );
          if( !m_pause )
          {
               work( m_pause );
               std::this_thread::sleep_for( std::chrono::milliseconds( this->m_msDelay ) );
          }
     }
}

#endif // THREADWORKER_H
