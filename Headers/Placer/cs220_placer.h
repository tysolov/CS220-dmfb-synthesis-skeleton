#ifndef _CS220_PLACER_H_
#define _CS220_PLACER_H_

#include "../Resources/enums.h"
#include "placer.h"
#include <vector>


class CS220Placer : public Placer
{
protected:
    // Methods

public:
    // Constructors
    CS220Placer();
    virtual ~CS220Placer();

    // Methods
    void place(DmfbArch *arch, DAG *schedDag, vector<ReconfigModule *> *rModules, CoalescingPool* cp = nullptr);
};
#endif /* _CS220_PLACER_ */