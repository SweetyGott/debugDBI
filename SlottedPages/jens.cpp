#include <functional>
#include <unordered_map>

struct TID{
	uint64_t pageId;
	unsigned slotId;
};

inline bool operator==(const TID& lhs, const TID& rhs){ 
	return lhs.pageId==rhs.pageId && lhs.slotId==rhs.slotId; 
}

namespace std {
	template <>
	struct hash<TID> {
    	std::size_t operator()(const TID& tid) const {
    		size_t h1 = std::hash<uint64_t>()(tid.pageId);
        	size_t h2 = std::hash<unsigned>()(tid.slotId);
			return h1^(h2<<1);
		}
	};
}

int main(int argc, char** argv){
	std::unordered_map<TID, unsigned> tids;

	TID tid = {1, 2};
	tids[tid] = 3;
	return 0;
}