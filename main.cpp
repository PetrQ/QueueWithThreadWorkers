#include <iostream>

#include "ThreadsManager.h"
#include "ImessageQueueEvents.h"
#include "ThreadWorker.h"
#include "Writer.h"
#include "Reader.h"

int main()
{
     using namespace pkus;

     {
          typedef int MessType;
          //Создаем очередь и потоки писателей и читателей
          MessageQueuePtr< MessType > liveQueue = std::make_shared< MessageQueue< int > >( 20, 4, 17 );
          Writer< MessType > wr1( liveQueue, 200 );
          Writer< MessType > wr2( liveQueue, 300 );
          ReaderWorker< MessType > rw1( liveQueue, 250 );
          ReaderWorker< MessType > rw2( liveQueue, 250 );

          //Собираем упрвление писателями/читателями
          ManagerPtr manager = std::make_shared< Manager >();
          WorkerHandler handle_w_1 = manager->addToManaged( WorkerHandler( &wr1, []( void* ) {} ) );
          WorkerHandler handle_w_2 = manager->addToManaged( WorkerHandler( &wr2, []( void* ) {} ) );

          liveQueue->setEvents( manager );

          //Запускаем обработку
          liveQueue->run();
          std::this_thread::sleep_for( std::chrono::seconds( 5 ) );

          //Завершаем обработку
          liveQueue->stop();
     } //автоматическое завершение
     std::cout << "MAIN FINISH" << std::endl;

     return 0;
}
