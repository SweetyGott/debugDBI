//
//  BufferManager.cpp
//  
//
//  Created by Julia Kindelsberger on 25/04/16.
//  Copyright © 2016 Julia Kindelsberger. All rights reserved.
//
#include <tuple>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

#include "BufferManager.hpp"

// Create a new instance that keeps up to pageCount frames in main memory.
// size_t instead of unsigned because of the following error: Undefined symbols for architecture x86_64
BufferManager::BufferManager(size_t pageCount) {
    
    pC = pageCount;
    mapFrames.reserve(pageCount);

    leastRecentlyUsed = NULL;
    mostRecentlyUsed = NULL;
    
    // initialize locks
    pthread_mutex_init(&mutex, NULL);
    pthread_rwlock_init(&lock, NULL);
    
    //std::cout << "BufferManager created" << std::endl;
}

BufferManager::BufferManager() {
    BufferManager(100);
}

// Destructor. Write all dirty frames to disk and free all resources.
BufferManager::~BufferManager() {
    
    // destroy the locks
    pthread_rwlock_destroy(&lock);
    pthread_mutex_destroy(&mutex);

    // write dirty frames to disk
    for (auto& keyValue: mapFrames) {
        keyValue.second.writeDataToDisk();
    }

    // close file descriptos
    for (auto& keyValue: mapFileDescriptors) {
        close(keyValue.second);
    }
    
    //std::cout << "BufferManager destroyed" << std::endl;
}

// A method to retrieve frames given a page ID and indicating whether the page will be held exclusively by this thread or not. The method can fail (by throwing an exception) if no free frame is available and no used frame can be freed. The pageID variable is split into a segment ID and the actual page ID. Each page is stored on disk in a file with the same name as its segment ID (e.g., "1").
BufferFrame& BufferManager::fixPage(uint64_t pageId, bool exclusive) {
    
    BufferFrame* bufferFrame;
    
    // lock the map
    pthread_rwlock_wrlock(&lock);
    
    //std::cout << "BufferManager fixPage" << std::endl;
    
    auto frame = mapFrames.find(pageId);
    
    // frame is buffered
    if(frame != mapFrames.end()) {
        bufferFrame = &frame->second;
        deleteLeastRecentlyusedFrame(bufferFrame);
        
    // frame is not buffered
    } else {
        
        // recheck condition, maybe it was buffered in the meantime
        frame = mapFrames.find(pageId);
        if(frame != mapFrames.end()) {
            bufferFrame = &frame->second;
            deleteLeastRecentlyusedFrame(bufferFrame);

        } else {
            // buffer full?
            if (pC <= mapFrames.size()) {
                // delete a frame
                BufferFrame* bufferFrame = getLeastRecentlyUsedFrame();
                mapFrames.erase((*bufferFrame).pageIdentification);
            }
            
            // get the file descriptor
            int id = pageId >> 48;
            
            // fd exists
            auto frame = mapFileDescriptors.find(id);
            int fileDescriptor;
            if(frame != mapFileDescriptors.end()) {
                fileDescriptor = (*frame).second;
                
                // fd does not exist
            } else {
                // The pageID variable is split into a segment ID and the actual page ID. Each page is stored on disk in a file with the same name as its segment ID (e.g., "1").-> 16 bit
                char filename[15];
                sprintf(filename, "%d", id);
                
                fileDescriptor = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                
                mapFileDescriptors[id] = fileDescriptor;
            }
            
            // add frame
            auto frame2 = mapFrames.emplace(std::piecewise_construct,
                  std::forward_as_tuple(pageId),
                  std::forward_as_tuple(fileDescriptor, pageId)
            );
            bufferFrame = &frame2.first->second;

            (*bufferFrame).users++;
        }
    }

    // unlock map
    pthread_rwlock_unlock(&lock);

    // lock frame
    (*bufferFrame).lockFrame(exclusive);

    return *bufferFrame;
}

/* Return a frame to the buffer manager indicating whether it is dirty or not. If dirty, 
 the page manager must write it back to disk. It does not have to write it back immediately, 
 but must not write it back before unfixPage is called. */
void BufferManager::unfixPage(BufferFrame& frame, bool isDirty) {
    
    //std::cout << "BufferManager unfixPage" << std::endl;
    
    if(isDirty) {
        frame.setDirty();
    }

    // unlock frame
    frame.unlockFrame();

    addLeastRecentlyUsedFrame(&frame);
}

void BufferManager::deleteLeastRecentlyusedFrame(BufferFrame* frame) {
    
    pthread_mutex_lock(&mutex);
    
    int temp = (*frame).users++;
    
    // no one is using it
    if(temp == 0) {
        // delete frame in linked list
        if ((*frame).nextFrame != NULL) {
            frame->nextFrame->previousFrame = (*frame).previousFrame;
        }
        
        if ((*frame).previousFrame != NULL) {
            frame->previousFrame->nextFrame = (*frame).nextFrame;
        }
        
        if (leastRecentlyUsed == frame) {
            leastRecentlyUsed = (*frame).nextFrame;
        }
        
        if (mostRecentlyUsed == frame) {
            mostRecentlyUsed = (*frame).previousFrame;
        }
        // delete pointer to next and previous frame
        (*frame).nextFrame = NULL;
        (*frame).previousFrame = NULL;
    }
    
    pthread_mutex_unlock(&mutex);
}

BufferFrame* BufferManager::getLeastRecentlyUsedFrame() {
    
   pthread_mutex_lock(&mutex);
    
    BufferFrame* bufferFrame = leastRecentlyUsed;
    
    if(leastRecentlyUsed != NULL) {
        leastRecentlyUsed = (*bufferFrame).nextFrame;
        (*bufferFrame).nextFrame = NULL;
        
        if(leastRecentlyUsed != NULL) {
            (*leastRecentlyUsed).previousFrame = NULL;
        }
        
        else {
            mostRecentlyUsed = NULL;
        }
    }
    
    pthread_mutex_unlock(&mutex);
    
    return bufferFrame;
}


void BufferManager::addLeastRecentlyUsedFrame(BufferFrame* frame) {
    pthread_mutex_lock(&mutex);

    int temp = --((*frame).users);
    
    // no one is using it
    if(temp == 0) {
        if(mostRecentlyUsed == NULL) {
            leastRecentlyUsed = frame;
        } else {
            (*frame).previousFrame  = mostRecentlyUsed;
            (*mostRecentlyUsed).nextFrame = frame;
        }
        mostRecentlyUsed = frame;
    }

    pthread_mutex_unlock(&mutex);
}
