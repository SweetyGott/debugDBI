#ifndef H_SlottedPage_HPP
#define H_SlottedPage_HPP

#include "Tid.hpp"
#include "BufferManager.hpp"
#include "Record.hpp"

#include <vector>


class Header {
    public:
        unsigned slotCount;
        unsigned firstFreeSlot;
        unsigned dataStart;
        unsigned freeSpace;
        unsigned spaceByArrangement;
};

//Slots
class Slot {
    public:
        unsigned offset;
        unsigned length;
};


class SlottedPage {
    
private:
    
    
    uint64_t pageId;
    BufferManager bm;
    
    std::vector<Slot> freeSpaces;
    
    //Header
public:    
    
    //Records
    
    
    SlottedPage();
    SlottedPage( BufferManager& bm, uint64_t pageId);
    
    void arrangePage();
    
    uint64_t getPageId();
    
    unsigned insert( const Record& rec );
    
    //Needs to be improved
    bool remove(TID tid);
    
    Record lookup( TID tid );
    
    
    
    
};



#endif