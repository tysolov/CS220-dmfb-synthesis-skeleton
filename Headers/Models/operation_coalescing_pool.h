//
// Created by Tyson Loveless on 8/20/18.
//

#ifndef MFSIMSSA_COALESCINGPOO_H
#define MFSIMSSA_COALESCINGPOO_H

#include <unordered_set>
#include "assay_node.h"

struct Area {
    int lx, rx, ty, by;
    bool operator==(const Area& rhs) const {
        return (lx == rhs.lx &&
                ty == rhs.ty &&
                rx == rhs.rx &&
                by == rhs.by);
    }
};

struct Pool {
    std::unordered_set<AssayNode*> pool;
    AssayNodeStatus status;
    Area area;                     // for free-placers
    FixedModule* module;           // for fixed-placers
    ResourceType resourceType;
    OperationType operationType;
    bool free;
    std::unordered_set<int> live;
    std::unordered_set<DAG *> dags;
};

class CoalescingPool {
public:
    CoalescingPool();
    CoalescingPool(std::vector<std::vector<AssayNode*> >);
    ~CoalescingPool() = default;
    std::unordered_set<AssayNode*> getNodes(AssayNode*);
    Pool* getPool(AssayNode*);
    AssayNodeStatus getStatus(AssayNode*);
    FixedModule* getModule(AssayNode*);
    void setStatus(AssayNode*);
    void setModule(AssayNode*, FixedModule*);
    void setType(AssayNode*, ResourceType);
    void addPool(std::vector<AssayNode*>, OperationType, ResourceType);
    std::vector<Pool*> * getPoolsByType(ResourceType);
    int getNumUsedByType(ResourceType);
    std::unordered_set<Pool*> * getAllPools() { return this->pools; }

private:
    std::unordered_set<Pool*> *pools;
};

#endif //MFSIMSSA_COALESCINGPOOL_H
