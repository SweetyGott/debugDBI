#ifndef H_TID_HPP
#define H_TID_HPP

#include <functional>




/*class TID {
    uint64_t pageId;
    unsigned slotId;
    
    public:
        TID(uint64_t pageId, unsigned slotId);
        
        uint64_t getPageId();
        unsigned getSlotId();
        
        operator uint64_t() {
            return pageId;
        }
        
        inline bool operator==(const TID& lhs, const TID& rhs){ 
            return lhs.pageId==rhs.pageId && lhs.slotId==rhs.slotId; 
        }
};*/

struct TID {
    uint64_t pageId;
    unsigned slotId;
    
    TID(uint64_t pi, unsigned si) {
        pageId = pi;
        slotId = si;
    }
};

inline bool operator==(const TID& lhs, const TID& rhs){ 
            return lhs.pageId==rhs.pageId && lhs.slotId==rhs.slotId; 
        }

namespace std {
    template <> 
    struct hash<TID> {
        std::size_t operator()(const TID&  tid) const {
            size_t h1 = std::hash<uint64_t>()(tid.pageId);
            size_t h2 = std::hash<unsigned>()(tid.slotId);
            return h1^(h2<<1);
        }
    };
};

#endif