#include "IThreadWorker.h"

#include <chrono>
#include <iostream>
#include <mutex>

const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
static std::mutex cout_lock;

void logg( const std::string& mess, void* const adress )
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
