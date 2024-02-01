#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <iostream>
#include <mutex>
#include <condition_variable>

using namespace std; 
        
class Semaphore {
  public:
    Semaphore (int valeur_initiale)
    : val(valeur_initiale) 
    {
            cout << "valeur initiale du sémaphore = " << val << endl;

    }
    
    inline void V ( int tid ) {
      std::unique_lock<std::mutex> lock(mtx);
      val++;
      //cout << "processus " << tid <<  " V" << endl;
      //déblocage d'un processus bloqué (si il en existe)
      cv.notify_one();
    }
    
    inline void P ( int tid ) {
      std::unique_lock<std::mutex> lock(mtx);
      while(val == 0) {
        //cout << "processus " << tid << " P" << endl;
        //attente jusqu'à invocation d'un V() -> blocage du processus appelant
        cv.wait(lock);
        //cout << "processus " << tid << " reprend" << endl;
      }
      val--;
    }
  private:
    std::mutex mtx;
    std::condition_variable cv;
    int val=0;
};

#endif //SEMAPHORE_HPP

