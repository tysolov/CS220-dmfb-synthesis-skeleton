#ifndef _CS220_SCHEDULER_H
#define _CS220_SCHEDULER_H

#include "scheduler.h"
#include "priority.h"
#include "../Util/util.h"
#include <math.h>

class AssayNode;

class CS220Scheduler : public Scheduler
{
public:
    // Constructors
    CS220Scheduler();
    virtual ~CS220Scheduler();

    // Methods
    unsigned long long schedule(DmfbArch *arch, DAG *dag);

private:
    // Variables

    // Methods

};


#endif // _CS220_SCHEDULER_H