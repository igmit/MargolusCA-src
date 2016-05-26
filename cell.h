#ifndef CELL_H
#define	CELL_H

#include <memory>
#include <vector>
#include "substance.h"
#include "types.h"
#include <cereal/types/vector.hpp>


using namespace std;

class Cell {
public:
    Cell();
    Cell(pSub sub);
    ~Cell();
    
    bool AddSub(pSub newSub);
    uint GetSubCount() const;
    void RemoveSub(int i);
    void Clear();
    
    bool HaveActive();
    bool HaveLiquid();
    bool HaveModifier();
    bool HaveSolid();
    bool IsEmpty() const;
    
    vector<pSub> GetSubs() const;
    pSub GetSub(uint i) const;

    template<class Archive>
    void serialize(Archive & archive) {
        
        vector<string> substances;

        for (int i = 0; i < subs.size(); i++) {
            substances.push_back(this->GetSub(i)->GetName());
        }
        
        archive(cereal::make_nvp("s", substances));
    }
private:
    vector<pSub> subs;
};

#endif	/* CELL_H */
