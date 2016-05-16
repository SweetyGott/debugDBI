#ifndef H_SPSegment_HPP
#define H_SPSegment_HPP

#include <map>

#include "BufferManager.hpp"

#include "Tid.hpp"
#include "SlottedPage.hpp"



class SPSegment {
    
private:
    BufferManager bm;
    
    std::map<uint64_t, SlottedPage> spm;
    
    uint64_t pageIdCounter;
    uint64_t createPageId();
    

    //Passende Seite finden, wird beim insert benötigt
    SlottedPage& getFittingPage(unsigned len);
    
    
    //depreciated?
    /*getSlot(BufferManager& bm){
        BufferFrame& bf = bm.fix(pageId, false);
        char* data = bf.getdata();
    }*/

public:
    SPSegment( BufferManager& bm);

    
    //Insert a Record
    TID insert(const Record& r);
    
    //Stupid Solution
    bool remove(TID tid);
    
    Record lookup(TID tid);
    
    //TBD
    bool update(TID tid, const Record& r);
    
    
};



#endif