//
//  main.cpp
//  Barber
//
//  Created by Nghia Le on 13/9/21.
//

#include <iostream>    // cout
#include <sys/time.h> // gettimeofday
#include <unistd.h>
#include <cstdlib>
#include <stdlib.h>
#include "Shop.hpp"

using namespace std;

void *barber( void * );
void *customer( void * );


class ThreadParam {
public:
  ThreadParam( Shop *shop, int id, int serviceTime ) :
    shop( shop ), id( id ), serviceTime( serviceTime ) { };
  Shop *shop;
  int id;
  int serviceTime;
};

int main( int argc, char *argv[] ) {

  // validate the arguments
  if ( argc != 5 ) {
    cerr << "usage: sleepingBarber nBaerbers nChairs nCustomers serviceTime" << endl;
    return -1;
  }
  int nBarbers = atoi( argv[1] );
  int nChairs = atoi( argv[2] );
  int nCustomers = atoi( argv[3] );
  int serviceTime = atoi( argv[4] );

  cout << "barbers: " << nBarbers << " chairs: " << nChairs << " customer: " << nCustomers << " serviceTime: " << serviceTime << endl;
  pthread_t barber_thread[nBarbers];
  pthread_t customer_threads[nCustomers];
  Shop shop( nBarbers, nChairs );
  
  for ( int i = 0; i < nBarbers; i++ ) {
    ThreadParam *param = new ThreadParam( &shop, i, serviceTime );
    pthread_create( &barber_thread[i], NULL, barber, (void *)param );
  }
  for ( int i = 0; i < nCustomers; i++ ) {
    usleep( rand( ) % 1000 );
    ThreadParam *param = new ThreadParam( &shop, i + 1, 0 );
    pthread_create( &customer_threads[i], NULL, customer, (void *)param );
  }

  for ( int i = 0; i < nCustomers; i++ )
    pthread_join( customer_threads[i], NULL );

  for ( int i = 0; i < nBarbers; i++ )
    pthread_cancel( barber_thread[i] );
  cout << "# customers who didn't receive a service = " << shop.nDropsOff
       << endl;

  return 0;
}
void *barber( void *arg ) {

  
  ThreadParam &param = *(ThreadParam *)arg;
  Shop &shop = *(param.shop);
  int id = param.id;
  int serviceTime = param.serviceTime;
  delete &param;

  // keep working until being terminated by the main
  while( true ) {
    shop.helloCustomer( id );  // pick up a new customer
    usleep( serviceTime );     // spend a service time
    shop.byeCustomer( id );    // release the customer
  }
}


void *customer(void *arg) {
    ThreadParam &param = *(ThreadParam *)arg;
    Shop &shop = *(param.shop);
    int id = param.id;
    delete &param;
    
    int barber = -1;
    
    if (barber == shop.visitShop(id) || barber != -1) {
        shop.leaveShop(id, barber);
    }
    
    return 0;
}
