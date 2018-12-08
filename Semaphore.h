
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

class Semaphore {
  public:
    void wait();
    void release();
    ~Semaphore(){};
    static Semaphore& create();
    // Semaphore& operator=(Semaphore const&);
};

#endif
