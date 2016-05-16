//
//  BufferFrame.cpp
//
//
//  Created by Julia Kindelsberger on 25/04/16.
//  Copyright © 2016 Julia Kindelsberger. All rights reserved.
//

#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "BufferFrame.hpp"

//#define DEBUG

const size_t blocksize = 1024 * 8;

BufferFrame::BufferFrame(int segmentFd, uint64_t pageId) {
    isNewlyCreated = true;
    isDirty = false;
    isClean = false;
    pageIdentification = pageId;
    data = NULL;
    offset = blocksize * (pageId& 0x0000ffffffffffff);
    fileDescriptor = segmentFd;
    previousFrame = NULL;
    nextFrame = NULL;
    users = 0;
    // init the lock
    pthread_rwlock_init(&lock, NULL);
    
    //std::cout << "BufferFrame created" << std::endl;
}

BufferFrame::~BufferFrame() {
     // destroy lock
    pthread_rwlock_destroy(&lock);

    writeDataToDisk();

    free(data);
    
    //std::cout << "BufferFrame destroyed" << std::endl;

}

// A buffer frame should offer a method giving access to the buffered page. Except for the buffered page, BufferFrame objects can also store control information (page ID, dirtyness, ...).
void* BufferFrame::getData() {
    //std::cout << "BufferFrame getData" << std::endl;
    // load data, if not already loaded
    if (isNewlyCreated) {
        
        data = malloc(blocksize);
        
        // read the data
        pread(fileDescriptor, data, blocksize, offset);
        
        isNewlyCreated = false;
        isClean = true;
        isDirty = false;
    }
    
    return data;
}

void BufferFrame::writeDataToDisk() {
    // write the data to the disk if dirty
    if (isDirty) {
        pwrite(fileDescriptor, data, blocksize, offset);
        
        isClean = true;
        isDirty = false;
        isNewlyCreated = false;
        
    }
}

void BufferFrame::lockFrame(bool exclusive) {
    if (exclusive) {
        pthread_rwlock_wrlock(&lock);
    }
    else {
        pthread_rwlock_rdlock(&lock);
    }
}

void BufferFrame::unlockFrame() {
    pthread_rwlock_unlock(&lock);
}

void BufferFrame::setDirty() {
    isDirty = true;
    isNewlyCreated = false;
    isClean = false;
}

