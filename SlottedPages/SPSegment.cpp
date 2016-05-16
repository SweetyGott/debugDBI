#include "SPSegment.hpp"

#include "SlottedPage.hpp"

uint64_t SPSegment::createPageId() {
        return pageIdCounter++;
}
    

//Passende Seite finden, wird beim insert benötigt
SlottedPage& SPSegment::getFittingPage(unsigned len) {
    //Suche nach passender Page
    for( auto& sp : spm ) {
        //Page genug Platz?
        Header* hdr = (Header*) &sp.second;
        if( hdr->freeSpace >= len ) {
            return sp.second;
        }
        //Page genug Platz bei rearangement?
        else if ( hdr->freeSpace + hdr->spaceByArrangement >= len ) {
            sp.second.arrangePage();
            return sp.second;
        }
        
    }
    //Keine freie Page gefunden, es wird eine neue geholt;
    uint64_t pI = createPageId();
    spm[pI] = SlottedPage(bm, pI);
    return spm[pI];
}
    
    
    //depreciated?
    /*getSlot(BufferManager& bm){
        BufferFrame& bf = bm.fix(pageId, false);
        char* data = bf.getdata();
    }*/

    SPSegment::SPSegment( BufferManager& bm) : bm(bm) {
        pageIdCounter = 0;
    }

    
    //Insert a Record
    TID SPSegment::insert(const Record& r) {
        SlottedPage& sp = getFittingPage(r.getLen());
        unsigned slotId = sp.insert(r);
        return TID(sp.getPageId(), slotId);
    }
    
    //Stupid Solution
    bool SPSegment::remove(TID tid) {
        return spm[tid.pageId].remove(tid);
    }
    
    Record SPSegment::lookup(TID tid) {
        return spm[tid.pageId].lookup(tid);
    }
    
    //TBD
    bool SPSegment::update(TID tid, const Record& r) {
        //remove(tid);
        //insert()
        return true;
    }