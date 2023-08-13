#include <iostream>

#include "ThreadsManager.h"
#include "ImessageQueueEvents.h"
#include "ThreadWorker.h"

using namespace std;

int main()
{
     using namespace pkus;

     //тест старта потоков через колбек
     Manager manager;
     ThreadWorker< int > wk1( 1000 );
     ThreadWorker< int > wk2( 500 );
     ThreadWorker< int > wk3( 2000 );

     //пустой деструктор для временных умных указателей в аргументах
     WorkerHandler handle_w_1 = manager.addToManaged( WorkerHandler( &wk1, []( void* ) {} ) );
     WorkerHandler handle_w_2 = manager.addToManaged( WorkerHandler( &wk2, []( void* ) {} ) );
     WorkerHandler handle_w_3 = manager.addToManaged( WorkerHandler( &wk3, []( void* ) {} ) );

     handle_w_2->start( true ); //пробуем стартовать поток отдельно ( не через менеджер )
     std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
     manager.on_start(); // стартуем все потоки в менеджере
     std::this_thread::sleep_for( std::chrono::seconds( 3 ) );

     manager.on_hwm(); // ставим потоки на паузу

     std::this_thread::sleep_for( std::chrono::seconds( 5 ) );

     manager.on_lwm(); // снова стартуем потоки

     handle_w_1.reset(); // забираем 1 поток из под контроля менеджера, проверяем join в деструкторе
     std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
     manager.on_stop();

     logg( " MAIN FINISH " );

     return 0;
}
