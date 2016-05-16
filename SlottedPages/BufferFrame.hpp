//
//  BufferFrame.hpp
//
//
//  Created by Julia Kindelsberger on 25/04/16.
//  Copyright © 2016 Julia Kindelsberger. All rights reserved.
//
#ifndef BufferFrame_hpp
#define BufferFrame_hpp

class BufferFrame {
    
  public:
    BufferFrame(int fileDescriptor, uint64_t pageId);
    ~BufferFrame();
    
    uint64_t pageIdentification; // 64 bit, 16 bit segment id
    void* data;
    off_t offset;
    int fileDescriptor;
    
    bool isDirty;
    void setDirty();
    bool isClean; // loaded but not changed
    bool isNewlyCreated;
    
    // pointer linked list frames
    BufferFrame* previousFrame;
    BufferFrame* nextFrame;
    
    unsigned users;
    
    void* getData();
    void writeDataToDisk();
    
    void lockFrame(bool exclusive);
    void unlockFrame();
    pthread_rwlock_t lock;

};

#endif /* BufferFrame_hpp */
