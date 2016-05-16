#include "SlottedPage.hpp"

#include "Tid.hpp"
#include "BufferManager.hpp"

#include <cstdlib>
#include <iostream>

SlottedPage::SlottedPage( BufferManager& bm, uint64_t pageId) : bm(bm), pageId(pageId) {        
    }
    
SlottedPage::SlottedPage() {
    bm = BufferManager(100);
    std::cout << "SlottedPage Constrcutor called ohne Argumente" << std::endl;
}
    
    
uint64_t SlottedPage::getPageId() {
    return pageId;
}
//Removing all the Spaces from a Page
void SlottedPage::arrangePage() {
    BufferFrame bf = bm.fixPage(pageId, false); //vll. true?
    void* data = bf.getData();
    
    int moveBack = 0;
    for( int i = 0; i < freeSpaces.size(); i++ ) {
        memcpy(data + freeSpaces[i].offset + freeSpaces[i].length - moveBack, data + freeSpaces[i].offset - moveBack, 8*1024-freeSpaces[i].offset);
        moveBack += freeSpaces[i].length;
    }
    
    bm.unfixPage(bf, true);
}
    
    
unsigned SlottedPage::insert( const Record& rec ) {
    BufferFrame bf = bm.fixPage(pageId, false);//vll true?
    void* data = bf.getData();
    
    Header* hdr = (Header*)data;
    
    //Leere Page, initialisierung der Werte
    if( !hdr->slotCount ) {
        hdr->firstFreeSlot = sizeof(Header);
        hdr->dataStart = 8*1024;
        hdr->freeSpace = 8*1024-sizeof(Header);
        hdr->spaceByArrangement = 0;
    }
    
    
    //Slot management
    Slot* slot = (Slot*)(data + hdr->firstFreeSlot);
    hdr->slotCount++;
    hdr->firstFreeSlot += sizeof(Slot);

    //Header Management
    hdr->dataStart -= rec.getLen();
    hdr->freeSpace -= rec.getLen();
    
    //Set Slot
    slot->offset = hdr->dataStart;
    slot->length = rec.getLen();
    
    //CopyData
    memcpy(data+slot->offset, rec.getData(), rec.getLen());
    
    bm.unfixPage(bf, true);
    
    return hdr->firstFreeSlot;
}

//Needs to be improved
bool SlottedPage::remove(TID tid) {
    BufferFrame bf = bm.fixPage(tid.pageId, false); //vll. true?
    void* data = bf.getData();
    
    Slot* s = (Slot*)(data + sizeof(Header)) + tid.slotId;
    freeSpaces.push_back(*s);//Save free part so rearrange page later on
    s->offset = 0;
    s->length = 0;
    bm.unfixPage(bf, true);
    
    return true;
}

Record SlottedPage::lookup( TID tid ) {
    BufferFrame bf = bm.fixPage(tid.pageId, false);
    void* data = bf.getData();
    
    Slot* s = (Slot*)(data + sizeof(Header)) + tid.slotId;
    
    
    Record* ret = (Record*)(data+s->offset);
    
    Record* rec = static_cast<Record*>(malloc(ret->getLen()));
    memcpy(rec, ret, ret->getLen());
    
    
    
    
    bm.unfixPage(bf, false);
    return Record(rec->getLen(), rec->getData());
}