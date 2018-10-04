//
// Created by Tyson Loveless on 8/20/18.
//

#include "operation_coalescing_pool.h"

CoalescingPool::CoalescingPool()
{
    pools = new std::unordered_set<Pool*>;
}

CoalescingPool::CoalescingPool(std::vector<std::vector<AssayNode*> > coalescings)
{
    pools = new std::unordered_set<Pool*>;
    for (std::vector<AssayNode*> c : coalescings)
    {
        addPool(c, GENERAL, BASIC_RES);
    }
}

int CoalescingPool::getNumUsedByType(ResourceType rt)
{
    int _ret = 0;
    for (Pool *p : *pools) {
        if (p->resourceType == rt)
            _ret++;
    }
    return _ret;
}

void CoalescingPool::addPool(std::vector<AssayNode *> c, OperationType ot, ResourceType rt)
{
    Pool *p = new Pool;
    auto *pool = new std::unordered_set<AssayNode*>;

    std::unordered_set<DAG*> dags;
    for (AssayNode* i : c)
    {
        for (int j = i->startTimeStep; j < i->endTimeStep; ++j)
            p->live.insert(j);
        pool->insert(i);
        dags.insert(i->GetDAG());
    }
    p->pool = *pool;
    p->status = SCHEDULED;
    p->operationType = ot;
    p->resourceType = rt;
    p->free = false;
    p->dags = dags;
    pools->insert(p);
}

std::unordered_set<AssayNode*> CoalescingPool::getNodes(AssayNode* node)
{
    return getPool(node)->pool;
}

AssayNodeStatus CoalescingPool::getStatus(AssayNode* node)
{
    return getPool(node)->status;
}

FixedModule* CoalescingPool::getModule(AssayNode* node)
{
    return getPool(node)->module;
}

Pool* CoalescingPool::getPool(AssayNode* node)
{
    for (Pool *p : *pools)
    {
        if (p->pool.count(node))
            return p;
    }
    Pool *p = new Pool;
    auto *pool = new std::unordered_set<AssayNode* >;
    pool->insert(node);
    p->pool = *pool;
    p->resourceType = node->boundedResType;
    p->operationType = node->GetType();
    p->status = SCHEDULED;
    p->free = true; // no interferences with other pools, else it would have been coalesced into it's own pool
    std::unordered_set<DAG*> d;
    d.insert(node->GetDAG());
    p->dags = d;
    pools->insert(p);
    clog << LOGGER "\n *** log - " << node->GetSummary() << " (ID: " << node->getId() << ") not found in a pool of coalesced nodes. Returning a new pool containing this node.\n" << endl << flush;
    return p;
}

void CoalescingPool::setStatus(AssayNode* node)
{
    getPool(node)->status = BOUND;
}

void CoalescingPool::setModule(AssayNode* node, FixedModule* fm)
{
    getPool(node)->module = fm;
}

void CoalescingPool::setType(AssayNode* node, ResourceType rt)
{
    Pool *p = getPool(node);
    for (AssayNode* n : p->pool)
    {
        n->boundedResType = rt;
    }
    p->resourceType = rt;
}

std::vector<Pool*> * CoalescingPool::getPoolsByType(ResourceType rt)
{
    auto *_ret = new std::vector<Pool*>;
    for (Pool *p : *pools)
    {
        if (p->resourceType == rt)
            _ret->push_back(p);
    }
    return _ret;
}