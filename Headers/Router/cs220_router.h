#ifndef _CS220_ROUTER_H_
#define _CS220_ROUTER_H_

#include "elapsed_timer.h"
#include "post_subprob_compact_router.h"
#include "enums.h"

class CS220Router : public PostSubproblemCompactionRouter
{
public:
    // Constructors
    CS220Router();
    CS220Router(DmfbArch *dmfbArch);
    virtual ~CS220Router();

protected:
    void computeIndivSupProbRoutes(vector<vector<RoutePoint *> *> *subRoutes, vector<Droplet *> *subDrops, map<Droplet *, vector<RoutePoint *> *> *routes);
    void routerSpecificInits();

private:
    // Methods

    // Members
};


#endif /* _CS220_ROUTER_H_ */