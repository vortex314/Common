#include <Log.h>
#include <Semaphore.h>
//________________________________________________________________ LINUX
//
#ifdef __linux__
#include <errno.h> /* errno, ECHILD            */
#include <fcntl.h>
#include <semaphore.h> /* sem_open(), sem_destroy(), sem_wait().. */
#include <semaphore.h>
#include <stdio.h>     /* printf()                 */
#include <stdlib.h>    /* exit(), malloc(), free() */
#include <sys/shm.h>   /* shmat(), IPC_RMID        */
#include <sys/types.h> /* key_t, sem_t, pid_t      */
#include <sys/types.h>

class LinuxSemaphore : public Semaphore {
    sem_t put_mutex;

  public:
    LinuxSemaphore() {
        if (sem_init(&put_mutex, 1, 1) < 0) {
            WARN("connect: Unable to create semaphore  %s errno : %d : %s ",
                 "pSem", errno, strerror(errno));
        }
        sem_unlink("pSem");
    }
    ~LinuxSemaphore() {}
    void wait() {
        if (::sem_wait(&put_mutex) < 0) { /* P operation */
            WARN("connect: Unable to wait semaphore  %s errno : %d : %s ",
                 "pSem", errno, strerror(errno));
        }
    }

    void release() {
        if (::sem_post(&put_mutex) < 0) { /* V operation */
            WARN("connect: Unable to post semaphore  %s errno : %d : %s ",
                 "pSem", errno, strerror(errno));
        }
    }
};

Semaphore& Semaphore::create() { return *new LinuxSemaphore(); }

#endif

#ifdef ARDUINO

class ArduinoSemaphore : public Semaphore {

  public:
    ArduinoSemaphore() {}
    ~ArduinoSemaphore() {}
    void wait() {}

    void release() {}
};

Semaphore& Semaphore::create() {
    ArduinoSemaphore* ls = new ArduinoSemaphore();
    return *ls;
}

#endif
//____________________________________________________________ STM32
#ifdef STM32F1
class STM32Semaphore : public Semaphore {
    //    sem_t* sem;
    //    unsigned int value = 0;
    sem_t put_mutex;

  public:
    STM32Semaphore() {}
    void wait() { cm_disable_interrupts(); }

    void release() { cm_enable_interrupts(); }
};

Semaphore& Semaphore::create() { return *(new STM32Semaphore()); }
#endif

#if defined(ESP32_IDF) || defined(ESP8266_OPEN_RTOS)
#ifdef ESP8266_OPEN_RTOS
#include <FreeRTOS.h>
#include <semphr.h>
#endif
#ifdef ESP32_IDF
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#endif
class Esp32Semaphore : public Semaphore {
    SemaphoreHandle_t xSemaphore = NULL;

  public:
    Esp32Semaphore() {
        xSemaphore = xSemaphoreCreateBinary();
        xSemaphoreGive(xSemaphore);
    }
    ~Esp32Semaphore() {}
    void wait() {
        if (xSemaphoreTake(xSemaphore, (TickType_t)1000) != pdTRUE) {
            WARN(" xSemaphoreTake()  timed out ");
        }
    }

    void release() {
        if (xSemaphoreGive(xSemaphore) != pdTRUE) {
            WARN("xSemaphoreGive() failed");
        }
    }
    static Semaphore& create() { return *(new Esp32Semaphore()); }
};

Semaphore& Semaphore::create() { return *new Esp32Semaphore(); }
#endif
