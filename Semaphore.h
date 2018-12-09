
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

class Semaphore {
  public:
    virtual void wait() = 0;
    virtual void release() = 0;
    //    ~Semaphore(){};
    //   Semaphore();
    static Semaphore& create();
    // Semaphore& operator=(Semaphore const&);
};

#endif
