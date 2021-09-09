#include "UniqueThreadPool.h"
#include <iostream>
#include <memory>


enum class Order
{
    idle,
    newOrder,
    ready
};

std::condition_variable cv;
std::mutex mut;
Order order_status = Order::idle;

void cooker( )
{
    while ( true )
    {
        std::unique_lock< std::mutex > locker( mut );
        cv.wait( locker, [ = ]( ){ return order_status == Order::newOrder; } );

        std::cout << "Cooker cooks" << std::endl;
        std::this_thread::sleep_for( 10000ms );
        std::cout << "Order is ready!" << std::endl;

        order_status = Order::ready;
        cv.notify_one( );
    }
    
}


void waiter( )
{
    while ( true )
    {
        std::unique_lock< std::mutex > locker( mut );

        cv.wait( locker, [ = ] ( ) { return order_status == Order::idle; } );
        std::cout << "Waiter wait for new order" << std::endl;
        std::this_thread::sleep_for( 10000ms );
        std::cout << "Waiter has new order!" << std::endl;
        order_status = Order::newOrder;
        cv.notify_one( );

        cv.wait( locker, [ = ] ( ) { return order_status == Order::ready; } );
        std::this_thread::sleep_for( 5000ms );
        std::cout << "Waiter give order" << std::endl;
        
        order_status = Order::idle;
        cv.notify_all( );
    }
    
}


int main( )
{
    std::unique_ptr<UniqueThreadPool> th_pool( UniqueThreadPool::getpool( ) );
    th_pool->start( );

    th_pool->add_task( cooker );
    th_pool->add_task( waiter );
    th_pool->add_task( waiter );
    th_pool->add_task( waiter );
    th_pool->add_task( waiter );
    

    return 0;
}
