/*
 * BipBuffer.cpp
 *
 *  Created on: 12-sep.-2014
 *      Author: lieven2
 */

#include <BipBuffer.h>
#include "errno.h"
#include "platform.h"
#include "Sys.h"

#include "malloc.h"
#if defined(__CYGWIN__)
#include "stdio.h"
#endif

/*
 Copyright (c) 2003 Simon Cooke, All Rights Reserved


 Licensed royalty-free for commercial and non-commercial
 use. All that I ask is that you send me an email
 telling me that you're using my code. It'll make me
 feel warm and fuzzy inside. simoncooke@earthlink.net

 */
/*
 void *memcpy(void *dest, const void *src, size_t n) {
 uint8_t* dp = (uint8_t*) dest;
 const uint8_t* sp = (uint8_t*) src;
 while (n--)
 *dp++ = *sp++;
 return dest;
 }
 */

IROM BipBuffer::BipBuffer() :
		pBuffer(NULL), ixa(0), sza(0), ixb(0), szb(0), buflen(0), ixResrv(0), szResrv(
				0) {
}

BipBuffer::~BipBuffer() {
	// We don't call FreeBuffer, because we don't need to reset our variables - our object is dying
	if (pBuffer != NULL) {
		//		::VirtualFree(pBuffer, buflen, MEM_DECOMMIT);
		::free(pBuffer); // LMR
	}
}

// Allocate Buffer
//
// Allocates a buffer in virtual memory, to the nearest page size (rounded up)
//
// Parameters:
//   int buffersize                size of buffer to allocate, in bytes (default: 4096)
//
// Returns:
//   bool                        true if successful, false if buffer cannot be allocated

IROM bool BipBuffer::allocateBuffer(int buffersize = 4096) {
	/*    if (buffersize <= 0) return false;

	 if (pBuffer != NULL) FreeBuffer();

	 SYSTEM_INFO si;
	 ::GetSystemInfo(&si);

	 // Calculate nearest page size
	 buffersize = ((buffersize + si.dwPageSize - 1) / si.dwPageSize) * si.dwPageSize;

	 pBuffer = (uint8_t*)::VirtualAlloc(NULL, buffersize, MEM_COMMIT, PAGE_READWRITE);
	 if (pBuffer == NULL) return false;

	 buflen = buffersize;
	 return true;*/
	pBuffer = (uint8_t*) malloc(buffersize);
	if (pBuffer == NULL)
		return false;

	buflen = buffersize;
	return true;
}

///
/// \brief Clears the buffer of any allocations.
///
/// Clears the buffer of any allocations or reservations. Note; it
/// does not wipe the buffer memory; it merely resets all pointers,
/// returning the buffer to a completely empty state ready for new
/// allocations.
///

IROM void BipBuffer::clear() {
	ixa = sza = ixb = szb = ixResrv = szResrv = 0;
}

// Free Buffer
//
// Frees a previously allocated buffer, resetting all internal pointers to 0.
//
// Parameters:
//   none
//
// Returns:
//   void

IROM void BipBuffer::freeBuffer() {
	if (pBuffer == NULL)
		return;

	ixa = sza = ixb = szb = buflen = 0;
	free(pBuffer); // LMR

//		::VirtualFree(pBuffer, buflen, MEM_DECOMMIT);

	pBuffer = NULL;
}

// Reserve
//
// Reserves space in the buffer for a memory write operation
//
// Parameters:
//   int size                amount of space to reserve
//   OUT int& reserved        size of space actually reserved
//
// Returns:
//   uint8_t*                    pointer to the reserved block
//
// Notes:
//   Will return NULL for the pointer if no space can be allocated.
//   Can return any value from 1 to size in reserved.
//   Will return NULL if a previous reservation has not been committed.

IROM uint8_t* BipBuffer::reserve(int size, int& reserved) {
	if (szResrv) {
		reserved = 0;
		ERROR("BipBuffer multithread issue  ");
		return 0;
	}
// We always allocate on B if B exists; this means we have two blocks and our buffer is filling.
	if (szb) {
		int freespace = getBFreeSpace();

		if (size < freespace)
			freespace = size;

		if (freespace == 0) {
			ERROR("BipBuffer overflow %d:%d %d:%d ", ixa, sza, ixb, szb);
			return NULL;
		}

		szResrv = freespace;
		reserved = freespace;
		ixResrv = ixb + szb;
		return pBuffer + ixResrv;
	} else {
		// Block b does not exist, so we can check if the space AFTER a is bigger than the space
		// before A, and allocate the bigger one.

		int freespace = getSpaceAfterA();
		if (freespace >= ixa) {
			if (freespace == 0)
				return NULL;
			if (size < freespace)
				freespace = size;

			szResrv = freespace;
			reserved = freespace;
			ixResrv = ixa + sza;
			return pBuffer + ixResrv;
		} else {
			if (ixa == 0)
				return NULL;
			if (ixa < size)
				size = ixa;
			szResrv = size;
			reserved = size;
			ixResrv = 0;
			return pBuffer;
		}
	}
}

// Commit
//
// Commits space that has been written to in the buffer
//
// Parameters:
//   int size                number of bytes to commit
//
// Notes:
//   Committing a size > than the reserved size will cause an assert in a debug build;
//   in a release build, the actual reserved size will be used.
//   Committing a size < than the reserved size will commit that amount of data, and release
//   the rest of the space.
//   Committing a size of 0 will release the reservation.
//

IROM void BipBuffer::commit(int size) {
	if (size == 0) {
		// decommit any reservation
		szResrv = ixResrv = 0;
		return;
	}

// If we try to commit more space than we asked for, clip to the size we asked for.

	if (size > szResrv) {
		size = szResrv;
	}

// If we have no blocks being used currently, we create one in A.

	if (sza == 0 && szb == 0) {
		ixa = ixResrv;
		sza = size;

		ixResrv = 0;
		szResrv = 0;
		return;
	}

	if (ixResrv == sza + ixa) {
		sza += size;
	} else {
		szb += size;
	}

	ixResrv = 0;
	szResrv = 0;
}

// GetContiguousBlock
//
// Gets a pointer to the first contiguous block in the buffer, and returns the size of that block.
//
// Parameters:
//   OUT int & size            returns the size of the first contiguous block
//
// Returns:
//   uint8_t*                    pointer to the first contiguous block, or NULL if empty.

IROM uint8_t* BipBuffer::getContiguousBlock(uint32_t& size) {
	if (sza == 0) {
		size = 0;
		return NULL;
	}

	size = sza;
	return pBuffer + ixa;

}

// DecommitBlock
//
// Decommits space from the first contiguous block
//
// Parameters:
//   int size                amount of memory to decommit
//
// Returns:
//   nothing

IROM void BipBuffer::decommitBlock(int size) {
	if (size >= sza) {
		ixa = ixb;
		sza = szb;
		ixb = 0;
		szb = 0;
	} else {
		sza -= size;
		ixa += size;
	}
}

// GetCommittedSize
//
// Queries how much data (in total) has been committed in the buffer
//
// Parameters:
//   none
//
// Returns:
//   int                    total amount of committed data in the buffer

IROM int BipBuffer::getCommittedSize() const {
	return sza + szb;
}

// GetReservationSize
//
// Queries how much space has been reserved in the buffer.
//
// Parameters:
//   none
//
// Returns:
//   int                    number of bytes that have been reserved
//
// Notes:
//   A return value of 0 indicates that no space has been reserved

IROM int BipBuffer::getReservationSize() const {
	return szResrv;
}

// GetBufferSize
//
// Queries the maximum total size of the buffer
//
// Parameters:
//   none
//
// Returns:
//   int                    total size of buffer

IROM int BipBuffer::getBufferSize() const {
	return buflen;
}

// IsInitialized
//
// Queries whether or not the buffer has been allocated
//
// Parameters:
//   none
//
// Returns:
//   bool                    true if the buffer has been allocated

IROM bool BipBuffer::isInitialized() const {
	return pBuffer != NULL;
}

IROM int BipBuffer::getSpaceAfterA() const {
	return buflen - ixa - sza;
}

IROM int BipBuffer::getBFreeSpace() const {
	return ixa - ixb - szb;
}

