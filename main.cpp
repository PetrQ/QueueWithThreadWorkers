#include <iostream>

#include "ImessageQueueEvents.h"
#include "ThreadWorker.h"

using namespace std;

int main()
{
     Manager manager;
     ThreadWorker< int > wk1( 1000 );
     ThreadWorker< int > wk2( 500 );
     ThreadWorker< int > wk3( 2000 );

     WorkerHandler handle_w_1 = manager.addToManaged( WorkerHandler( &wk1, []( void* ) {} ) );
     WorkerHandler handle_w_2 = manager.addToManaged( WorkerHandler( &wk2, []( void* ) {} ) );
     WorkerHandler handle_w_3 = manager.addToManaged( WorkerHandler( &wk3, []( void* ) {} ) );

     handle_w_2->start( true );
     std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
     manager.on_start();
     std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
     handle_w_1.reset();
     std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
     manager.on_stop();

     logg( " MAIN FINISH " );

     return 0;
}
