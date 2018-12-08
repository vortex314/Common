
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

class Semaphore
{
public:
    virtual void wait();
    virtual void release();
    ~Semaphore() {};
    static Semaphore& create();
    // Semaphore& operator=(Semaphore const&);
};

#endif
