#include <iostream>

#include "ImessageQueueEvents.h"
#include "ThreadWorker.h"

using namespace std;

int main()
{
     MessageQueueEvents manager;
     ThreadWorker< int > wk1( 1000 );
     ThreadWorker< int > wk2( 500 );
     ThreadWorker< int > wk3( 2000 );

     auto handle_w_1 = manager.subscribeToWork( [ &wk1 ]( bool val ) { wk1.start( val ); } );
     auto handle_w_2 = manager.subscribeToWork( [ &wk2 ]( bool val ) { wk2.start( val ); } );
     auto handle_w_3 = manager.subscribeToWork( [ &wk3 ]( bool val ) { wk3.start( val ); } );

     auto handle_p_1 = manager.subscribeToPause( [ &wk1 ]( bool val ) { wk1.pause( val ); } );
     auto handle_p_2 = manager.subscribeToPause( [ &wk2 ]( bool val ) { wk2.pause( val ); } );
     auto handle_p_3 = manager.subscribeToPause( [ &wk3 ]( bool val ) { wk3.pause( val ); } );

     manager.on_start();
     std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
     handle_w_1.reset();
     std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
     manager.on_stop();

     std::cout << "MAIN FINISH " << std::endl;

     return 0;
}
