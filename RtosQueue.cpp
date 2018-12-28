#include "RtosQueue.h"

RtosQueue::~RtosQueue() {

}

#if defined(ESP32_IDF) || defined(ESP_OPEN_RTOS)

#ifdef ESP32_IDF
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#endif // ESP32_IDF

#ifdef ESP_OPEN_RTOS
#include <FreeRTOS.h>
#include <queue.h>
#endif // ESP_OPEN_RTOS

class FreeRtosQueue : public RtosQueue {

	private:
		QueueHandle_t _queue;
	public:
		FreeRtosQueue(uint32_t size) ;
		virtual ~FreeRtosQueue() ;
		int enqueue(Xdr& );
		int dequeue(Xdr&,uint32_t time);
};


FreeRtosQueue::FreeRtosQueue(uint32_t size) {
	INFO(" FreeRtosQueue created ");
	_queue = xQueueCreate(size,sizeof(Xdr*));

}

FreeRtosQueue::~FreeRtosQueue() {
}


int FreeRtosQueue::enqueue(Xdr& xdr) {
	Xdr* nx = new Xdr(xdr.size());
	*nx=xdr;
	if ( xQueueSend(_queue,&nx,1)!=pdTRUE ) {
		WARN("enqueue failed");
		return ENOENT;
	}
	return 0;
}
int FreeRtosQueue::dequeue(Xdr& xdr,uint32_t time) {
	Xdr* px;
	if ( xQueueReceive(_queue,&px,time/portTICK_PERIOD_MS)!=pdTRUE) {
		return ENOENT;
	}
	xdr = *px; // copy data
	delete px;
	return 0;
}



RtosQueue& RtosQueue::create(uint32_t size ) {
	return *new FreeRtosQueue(size);
}


#endif // defined(ESP32_IDF) || defined(ESP_OPEN_RTOS)

#ifdef __linux__
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

class LinuxRtosQueue : public RtosQueue {

	private:
		mqd_t  _queue;
	public:
		LinuxRtosQueue(uint32_t size) ;
		~LinuxRtosQueue() ;
		int enqueue(Xdr& );
		int dequeue(Xdr&,uint32_t);
};


LinuxRtosQueue::LinuxRtosQueue(uint32_t size) {
	struct mq_attr attr;
	/* initialize the queue attributes */
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(Xdr*);
	attr.mq_curmsgs = 0;

	char queueName[30];
	snprintf(queueName,sizeof(queueName),"/LinuxQueue%lX",(uint64_t)&_queue);

	/* create the message queue */
	if ((_queue = mq_open(queueName, O_CREAT | O_RDWR | O_CLOEXEC, 0644, &attr))<0) {
		WARN(" queue creation failed [%d] '%s' ",errno,strerror(errno));
	}
	assert(_queue>=0);
	INFO(" LinuxRtosQueue created ");

}

LinuxRtosQueue::~LinuxRtosQueue() {
}


int LinuxRtosQueue::enqueue(Xdr& xdr) {
	Xdr* nx = new Xdr(xdr.size());
	*nx=xdr;
	if (  mq_send(_queue, ( char*)&nx, sizeof(nx), 0)) {
		WARN(" enqueue failed [%d] '%s' ",errno,strerror(errno));
		assert(false);
		return ENOENT;
	}
	return 0;
}

int LinuxRtosQueue::dequeue(Xdr& xdr,uint32_t waitTime) {
	Xdr* px;
	struct   timespec tm;

	clock_gettime(CLOCK_REALTIME, &tm);
	tm.tv_sec += waitTime/1000;
	tm.tv_nsec += 1000000 * (waitTime%1000);
	int nbrBytes;
	if ( (nbrBytes = mq_timedreceive(_queue, ( char*)&px, sizeof(px), NULL,&tm)) != sizeof(px)) {
		if ( errno != ETIMEDOUT ) {
			WARN(" dequeue failed [%d] '%s', #bytes : %d ",errno,strerror(errno),nbrBytes);
			assert(false);
		}
		return ENOENT;
	}
	xdr = *px; // copy data
	delete px;
	return 0;
}

RtosQueue& RtosQueue::create(uint32_t size ) {
	return *new LinuxRtosQueue(size);
}



#endif // __linux__
