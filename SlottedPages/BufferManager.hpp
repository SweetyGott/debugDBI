//
//  BufferManager.hpp
//  
//
//  Created by Julia Kindelsberger on 25/04/16.
//  Copyright © 2016 Julia Kindelsberger. All rights reserved.
//

#ifndef BufferManager_hpp
#define BufferManager_hpp

#include <pthread.h>
#include <unordered_map>

#include "BufferFrame.hpp"

class BufferManager {
    
  public:

    BufferManager(size_t pageCount);
    BufferManager();
    ~BufferManager();
    
    size_t pC;

    // hash map with the frames
    std::unordered_map<uint64_t, BufferFrame> mapFrames;
    
    // hash map with the file descriptors
    std::unordered_map<unsigned, int> mapFileDescriptors;
    
    // locks
    pthread_rwlock_t lock;
    pthread_mutex_t mutex;
    
    // least recently used logic
    BufferFrame* leastRecentlyUsed;
    BufferFrame* mostRecentlyUsed;
    
    BufferFrame& fixPage(uint64_t pageID, bool exclusive);
    void unfixPage(BufferFrame& frame, bool isDirty);

    void addLeastRecentlyUsedFrame(BufferFrame* frame);
    void deleteLeastRecentlyusedFrame(BufferFrame* frame);
    BufferFrame* getLeastRecentlyUsedFrame();

};

#endif /* BufferManager_hpp */

