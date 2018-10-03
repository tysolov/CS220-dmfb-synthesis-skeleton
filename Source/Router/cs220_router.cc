/*
 * Implement your routing algorithm here!
 */

#include "../../Headers/Router/cs220_router.h"

///////////////////////////////////////////////////////////////////////////////////
// Constructors
///////////////////////////////////////////////////////////////////////////////////
CS220Router::CS220Router()
{
    claim(false, "Invalid constructor used for Router variant.  Must use form that accepts DmfbArch.\n");
}

CS220Router::CS220Router(DmfbArch *dmfbArch)
{
    arch = dmfbArch;
}

CS220Router::~CS220Router()
{
}

///////////////////////////////////////////////////////////////////////////////////
// Any initializations required for your routing algorithm should be done here
///////////////////////////////////////////////////////////////////////////////////
void CS220Router::routerSpecificInits()
{

}

///////////////////////////////////////////////////////////////////////////////////
// This is the method that actually performs the scheduling.  See roy_maze_router.cc
//  to get a basic idea on how this is accomplished.
//
// Must compute the individual subroutes for a sub-problem. A new sub-route is
// created for each sub-route and added to subRoutes; also, the corresponding
// droplet is added to subDrops (corresponding routes and droplets must share
// the same index in subRoutes and subDrops).
//
// See PostSubproblemCompactRouter::computeIndivSupProbRoutes() for more details.
///////////////////////////////////////////////////////////////////////////////////
void CS220Router::computeIndivSupProbRoutes(vector<vector<RoutePoint *> *> *subRoutes, vector<Droplet *> *subDrops, map<Droplet *, vector<RoutePoint *> *> *routes)
{

}