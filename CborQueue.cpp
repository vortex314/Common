/*
 * CborQueue.cpp
 *
 *  Created on: Nov 24, 2015
 *      Author: lieven
 */

#include <CborQueue.h>
#include <Log.h>

#ifdef __linux__
#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>
#include <sys/types.h>
#include <semaphore.h>


sem_t* sem;
unsigned int value = 0;
sem_t put_mutex;
#endif

#ifdef OPENCM3
#include <libopencm3/cm3/cortex.h>
#endif

void sema_create() {
#ifdef __linux__
//	sem = sem_open("pSem", O_CREAT | O_EXCL); //, 0644, value);
//	if (sem < 0) {
	if (sem_init(&put_mutex, 1, 1) < 0) {
		WARN("connect: Unable to create semaphore  %s errno : %d : %s ",
				"pSem", errno, strerror(errno));
	}
	sem_unlink("pSem");
#endif
#ifdef STM32F1
#endif
}

void sema_wait() {
#ifdef __linux__
	if (::sem_wait(&put_mutex) < 0) { /* P operation */
		WARN("connect: Unable to wait semaphore  %s errno : %d : %s ",
				"pSem", errno, strerror(errno));
	}
#endif
#ifdef STM32F1
	cm_disable_interrupts();
#endif
}

void sema_release() {
#ifdef __linux__
	if (::sem_post(&put_mutex) < 0) { /* V operation */
		WARN("connect: Unable to post semaphore  %s errno : %d : %s ",
				"pSem", errno, strerror(errno));
	}
#endif
#ifdef STM32F1
	cm_enable_interrupts();
#endif
}

CborQueue::CborQueue(uint32_t size) {
	_read_size = 0;
	_write_size = 0;
	_start = 0;
	_buffer.allocateBuffer(size);
	sema_create();
//	WARN(" CborQueue %X:%d ",this,_size);
}

CborQueue::~CborQueue() {
	_buffer.freeBuffer();
}

uint32_t CborQueue::getCapacity() {
	return _buffer.getBufferSize();
}

uint32_t CborQueue::getUsed() {
	return _buffer.getCommittedSize();
}

bool CborQueue::hasData() {
	return _buffer.hasData();
}

bool CborQueue::hasSpace(uint32_t size) {
	return _buffer.hasSpace(size + 2);
}

Erc CborQueue::put(Cbor& cbor) {
	int reserved = 0;
	Erc erc;
	uint32_t size = cbor.length();
	if (size == 0)
		return EINVAL;

	sema_wait();
	uint8_t* start = _buffer.reserve((int) size + 2, reserved);
	if ((start == 0) || (uint32_t) reserved < (size + 2)) {
		_buffer.commit(0);
		erc = ENOMEM;
	} else {
		*start = size >> 8;
		*(start + 1) = size & 0xFF;
		memcpy(start + 2, cbor.data(), size);
		_buffer.commit(size + 2);
		erc = E_OK;
	}
	sema_release();
	return erc;
}

Erc CborQueue::get(Cbor& cbor) {
	Erc erc;
	uint32_t length;
	cbor.clear();
	sema_wait();
	if (!hasData()) {
		erc = ENOENT;
	} else {
		uint8_t* start = _buffer.getContiguousBlock(length);
		uint32_t size;
		if (length == 0 || start == 0) {
			erc = ENOENT;
		} else {
			size = *start; // --------------- Big endian write of 16 bit size
			size <<= 8;
			size += *(start + 1);

			if (size > (uint32_t) cbor.capacity()) { // --------------- lost message, not enough space
				erc = ENOMEM;
			} else {
				cbor.write(start + 2, 0, size); //-------------- copy to cbor
				cbor.offset(0); //---------------- rewind
				erc = E_OK;
			}
			_buffer.decommitBlock(size + 2);
		}
	}
	sema_release();
	return erc;
}

Erc CborQueue::putf(const char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	Erc erc = vputf(fmt, args);
	va_end(args);
	return erc;
}

Erc CborQueue::vputf(const char* fmt, va_list args) {
//		va_list args;
	Erc erc;
	Cbor cbor(0);
	erc = putMap(cbor);
	if (erc) {
		WARN(" putMap failed : %d ", erc)
		return erc;
	}

	bool b = cbor.vaddf(fmt, args);
	b = b && cbor.hasSpace(1); // full suspect

	if (b == false)
		cbor.clear();
	return putRelease(cbor);
}
/* NOT THREAD SAFE
Erc CborQueue::getf(const char * format, ...) {
	va_list args;
	Erc erc;
	Cbor cbor(0);
	va_start(args, format);
	if ((erc = getMap(cbor)) == E_OK) {
		if (cbor.vscanf(format, args)) {
			getRelease(cbor);
			va_end(args);
			return E_OK;
		} else {
			cbor.clear();
			getRelease(cbor);
			va_end(args);
			return EINVAL;
		}
	} else {
		va_end(args);
		return erc;
	}
}
*/

#define MAX_SIZE 300
Erc CborQueue::putMap(Cbor& cbor) {
	Erc erc;
	int reserved = 0;
	int size = MAX_SIZE;
	sema_wait();						//**************************** MUTEX SET
	if (_write_size) {
		WARN(" CborQueue %X:%d ", this, _write_size);
		erc = EBUSY;
	} else {
		_start = _buffer.reserve((int) size + 2, reserved);
		if (_start == 0 || reserved == 0) {
			erc = ENOMEM;
		} else {
			cbor.map(_start + 2, reserved);
			_write_size = reserved;
			erc = E_OK;
		}
	}
	if (erc)
		sema_release();
	return erc;
}

Erc CborQueue::putRelease(Cbor& cbor) {
	Erc erc;
	if (_write_size == 0) {
		erc = ENOMEM;
	} else {
		uint32_t size = cbor.length();
		if (size == 0)  {
			_buffer.commit(0);
			cbor.map(0, 0);
			_write_size = 0;
			erc= ENOMEM;
		} else {
			*_start = size >> 8;
			*(_start + 1) = size & 0xFF;
			memcpy(_start + 2, cbor.data(), size);
			_buffer.commit(size + 2);
			cbor.map(0, 0);
			_write_size = 0;
			erc=E_OK;
		}
	}
	sema_release();				//******************************* MUTEX RELEASE
	return erc;
}
/*
Erc CborQueue::getMap(Cbor& cbor) {
//	sema_wait();
	if (_read_size) {
//		sema_release();
		return EBUSY;
	}
	if (!hasData())
		return ENOENT;
	uint32_t length = 0;
	cbor.clear();
	_start = _buffer.getContiguousBlock(length);
	if (length == 0) {
//		sema_release();
		return ENOMEM;
	}
	_read_size = *_start; // --------------- Big endian write of 16 bit size
	_read_size <<= 8;
	_read_size += *(_start + 1);
//	INFO(" map to : %X , %d", _start, _size);
	cbor.map(_start + 2, _read_size);

	return E_OK;
}

Erc CborQueue::getRelease(Cbor& cbor) {
	if (_read_size) {
		cbor.map(0, 0);
		_buffer.decommitBlock(_read_size + 2); // --------------- lost message
		_read_size = 0;
//		sema_release();
		return E_OK;
	} else {
//		sema_release();
		return ENOMEM;
	}
}
*/
