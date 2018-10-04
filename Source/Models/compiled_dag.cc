/*------------------------------------------------------------------------------*
 *                       (c)2016, All Rights Reserved.     						*
 *       ___           ___           ___     									*
 *      /__/\         /  /\         /  /\    									*
 *      \  \:\       /  /:/        /  /::\   									*
 *       \  \:\     /  /:/        /  /:/\:\  									*
 *   ___  \  \:\   /  /:/  ___   /  /:/~/:/        								*
 *  /__/\  \__\:\ /__/:/  /  /\ /__/:/ /:/___     UCR DMFB Synthesis Framework  *
 *  \  \:\ /  /:/ \  \:\ /  /:/ \  \:\/:::::/     www.microfluidics.cs.ucr.edu	*
 *   \  \:\  /:/   \  \:\  /:/   \  \::/~~~~ 									*
 *    \  \:\/:/     \  \:\/:/     \  \:\     									*
 *     \  \::/       \  \::/       \  \:\    									*
 *      \__\/         \__\/         \__\/    									*
 *-----------------------------------------------------------------------------*/
/*---------------------------Implementation Details-----------------------------*
 * Source: compiled_dag.cc														*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: August 15, 2015							*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "compiled_dag.h"
#include "scheduler.h"
#include "analyze.h"
#include "file_out.h"

class Analyze;

/////////////////////////////////////////////////////////////////////////////////
// CompiledDAG Constructor/Destructor
/////////////////////////////////////////////////////////////////////////////////
CompiledDAG::CompiledDAG(Synthesis *synthEngine, DAG * dagToCompile)
{
	synthesisEngine = synthEngine;
	uncompiledDAG = dagToCompile;
	printableName = uncompiledDAG->GetPrintableName();
	rModules = new vector<ReconfigModule *>();
	routes = new map<Droplet *, vector<RoutePoint *> *>();
	dirtyCells = new vector<vector<RoutePoint*> *>();
	pinActivations = new vector<vector<int> *>();
	tsBeginningCycle = new vector<unsigned long long>();
	ownTranInToParentTranOut = new map<AssayNode *, AssayNode *>();
	ownTranOutToChildTranIn = new map<AssayNode *, AssayNode *>();
}

CompiledDAG::~CompiledDAG()
{
	//uncompiledDAG = NULL; // Deleted elsewhere

	delete tsBeginningCycle;
	for (unsigned i = 0; i < rModules->size(); i++)
	{
		ReconfigModule *rm = rModules->back();
		rModules->pop_back();
		delete rm;
	}
	delete rModules;
	while (!routes->empty())
	{
		Droplet *d = routes->begin()->first;
		vector<RoutePoint *> *route = routes->begin()->second;
		routes->erase(d);
		delete d;
		while (!route->empty())
		{
			RoutePoint *rp = route->back();
			route->pop_back();
			delete rp;
		}
		delete route;
	}
	delete routes;

	while (!dirtyCells->empty())
	{
		vector<RoutePoint *> *v = dirtyCells->back();
		dirtyCells->pop_back();
		v->clear();
		delete v;
	}
	delete dirtyCells;

	while (!pinActivations->empty())
	{
		vector<int> *pins = pinActivations->back();
		pinActivations->pop_back();
		pins->clear();
		delete pins;
	}
	delete pinActivations;

	delete ownTranInToParentTranOut;
	delete ownTranOutToChildTranIn;
}

/////////////////////////////////////////////////////////////////////////////////
// This method compiles the DAG given the specified scheduler/placer/router/etc.
/////////////////////////////////////////////////////////////////////////////////
void CompiledDAG::compileDAG()
{
	cout << "Compiling DAG: " << uncompiledDAG->GetPrintableName() << endl;

	scheduleDAG();
	placeDAG();
	routeDAG();
}



/////////////////////////////////////////////////////////////////////////////////
// This method schedules the DAG given the specified scheduler
/////////////////////////////////////////////////////////////////////////////////
void CompiledDAG::scheduleDAG()
{
	cout << "Scheduling DAG: " << uncompiledDAG->GetPrintableName() << endl;
	///////////////////////////////////////////////////////
	// Implemented So Far: List Scheduler
	///////////////////////////////////////////////////////
	Scheduler *scheduler = synthesisEngine->getScheduler();
	DmfbArch *arch = synthesisEngine->getArch();
	uncompiledDAG->setFreq(arch->getFreqInHz()); // Sets the number of cycles of the operations; must be done before scheduling

	CompatChk::PreScheduleChk(scheduler, arch, uncompiledDAG, true);
	unsigned long long lastTS = scheduler->schedule(arch, uncompiledDAG);
	cout << "Scheduled " << getPrintableName() << " w/ schedule length of " << lastTS << " time-steps." << endl;
	FileOut::WriteScheduledDagAndArchToFile(uncompiledDAG, arch, scheduler, "Output/1_SCHED_to_PLACE_" + getPrintableName() + ".txt");
	Analyze::AnalyzeSchedule(uncompiledDAG, arch, scheduler);
	cout << "Outputting scheduled graph..." << endl << endl; uncompiledDAG->OutputGraphFile("SCHED_COMPILED_" + getPrintableName(), true, true);
}

/////////////////////////////////////////////////////////////////////////////////
// This method places/binds the DAG given the specified placer/binder
/////////////////////////////////////////////////////////////////////////////////
void CompiledDAG::placeDAG(CoalescingPool *coalescedNodes)
{
	cout << "Placing DAG: " << uncompiledDAG->GetPrintableName() << endl;
	///////////////////////////////////////////////////////
	// Implemented So Far: left edge binder, NSGAII placer, KAMER placer
	///////////////////////////////////////////////////////
	Placer *placer = synthesisEngine->getPlacer();
	DmfbArch *arch = synthesisEngine->getArch();
	CompatChk::PrePlaceChk(placer, arch, true);
	placer->place(arch, uncompiledDAG, rModules, coalescedNodes);
	FileOut::WritePlacedDagAndArchToFile(uncompiledDAG, arch, placer, rModules, "Output/2_PLACE_to_ROUTE_" + getPrintableName() + ".txt");
	Analyze::AnalyzePlacement(arch, rModules);
	cout << "Outputting placed graph..." << endl << endl; uncompiledDAG->OutputGraphFile("PLACE_COMPILED_" + getPrintableName(), true, true);
}

/////////////////////////////////////////////////////////////////////////////////
// This method routes the DAG given the specified routes
/////////////////////////////////////////////////////////////////////////////////
void CompiledDAG::routeDAG()
{
	cout << "Routing DAG: " << uncompiledDAG->GetPrintableName() << endl;
	///////////////////////////////////////////////////////
	// Implemented So Far: Roy Maze Router & Beginning Compaction
	///////////////////////////////////////////////////////
	Router *router = synthesisEngine->getRouter();
	DmfbArch *arch = synthesisEngine->getArch();
	CompatChk::PreRouteChk(router, arch, true);
	router->route(uncompiledDAG, arch, rModules, routes, pinActivations, tsBeginningCycle);
	if (router->getDropLocationsFromSim())
		router->simulateDropletMotion(routes, pinActivations);
	else
		router->setPinActivationsFromDropletMotion(routes, pinActivations);
	if (CompatChk::CanPerformRouteAnalysis(router))
	{
		//Analyze::AnalyzeDropletConcentrationAndIO(uncompiledDAG, arch, routes);
		Analyze::AnalyzeRoutes(arch, routes);
	}

	///////////////////////////////////////////////////////
	// Pin-mapping & Wire-routing /////////////////////////
	///////////////////////////////////////////////////////
	// Pin-mapping will always be Individually Addressable
	// (or some other fixed mapping like the FPPC DMFB)
	// b/c of the dynamic nature of CFGs. Thus we skip it in
	// this area. Likewise, wire routing will be done later
	// as it only needs to be done once.
	///////////////////////////////////////////////////////
	//arch->getPinMapper()->setMapPostRoute(pinActivations, routes);
	//CompatChk::PreWireRouteChk(arch, true);
	//arch->getWireRouter()->computeWireRoutes(pinActivations, false);
	//Synthesis::printWireRoutingStats(arch);

	FileOut::WriteRoutedDagAndArchToFile(uncompiledDAG, arch, router, rModules, routes, dirtyCells, pinActivations, tsBeginningCycle, "Output/3_ROUTE_to_SIM_" + getPrintableName() + ".txt");
	//FileOut::WriteHardwareFileWithWireRoutes(arch, "Output/4_HARDWARE_DESCRIPTION.txt", true);
}

/////////////////////////////////////////////////////////////////////////////////
// This method takes in the conditional groups (for the uncompiled CFG) and a
// map between uncompiled/compiled DAGs and generates routing DAGs based on the
// control flow dictated by the conditional groups.
/////////////////////////////////////////////////////////////////////////////////
void CompiledDAG::generateRoutingDAGs(vector<ConditionalGroup *> conditionalGroups, map<DAG *, CompiledDAG *> *uncompToCompDAG, map<Condition *, vector<CompiledDAG *> *> *conditionToRoutingDAGs)
{
	CompiledDAG *source = this;
	for (ConditionalGroup *cg : conditionalGroups)
	{
		for (Condition *c : *(cg->getConditions()))
		{
			bool unsupportedDAG = false;
			string unsupportedDAGname = "";
			bool generatedRoutingDAG = false;
			for (DAG * dependentDAG : c->dependents)
			{
				// Sanity check for behavior that is not currently supported
				if (dependentDAG == uncompiledDAG)
				{
					if (c->branchIfTrue)
					{
						// Generate Routing DAG for this condition
						CompiledDAG *target = uncompToCompDAG->at(c->branchIfTrue);
						cout << "Generating routing DAG for transition: " << source->getPrintableName() << " ===> " << target->getPrintableName() << endl;

						// Create newRoutingDAG and generate CompiledDAG version (from newRoutingDAG) and map to condition
						DAG * newRoutingDAG = new DAG();
						newRoutingDAG->setName(source->getPrintableName() + "__TO__" + target->getPrintableName());
						CompiledDAG *newRoutingCompiledDAG = new CompiledDAG(synthesisEngine, newRoutingDAG);


						unsigned long long lastStartTS = uncompiledDAG->getMaxOpStartingTS();
						for (AssayNode *n : uncompiledDAG->tails)
						{
							// Found a tail which will need to be moved to the routingDAG
							if (n->GetStartTS() == lastStartTS)
							{
								if (n->GetType() == OUTPUT)
								{
									cout << "\tMove OUTPUT (" << n->GetName() << ") to routing DAG." << endl;
								}
								else if (n->GetType() == TRANSFER_OUT)
								{
									// Already terminated in a TRANSFER_OUT, will create a pass-through in routing DAG
									// by creating new TRANSFER_IN/OUT
									cout << "\tMove TRANSFER_OUT (" << n->GetName() << ") to routing DAG." << endl;
									AssayNode * newTI = newRoutingDAG->AddTransInNode();
									AssayNode * newTO = newRoutingDAG->AddTransOutNode();
									newRoutingDAG->ParentChild(newTI, newTO);

									// Now, link the new TRANSFER_IN with the current TRANSFER_OUT (n) in the newRoutingCompiledDAG
									// IF we find a transfer edge that has a TRANSFER_IN in the target DAG and has a TRANSFER_OUT that
									// is the same as the current TRANSFER_OUT (n)
									bool foundMatch = false;
									for (TransferEdge *edge : c->transfers)
									{
										if (edge->transIn->GetDAG() == target->uncompiledDAG && edge->transOut == n)
										{
											(*newRoutingCompiledDAG->ownTranInToParentTranOut)[newTI] = edge->transOut;
											(*newRoutingCompiledDAG->ownTranOutToChildTranIn)[newTO] = edge->transIn;
											foundMatch = true;
										}
									}
									claim(foundMatch, "Could not find connected TransferIn node in target assay " + target->getPrintableName());
								}
								else
									claim(false, "Candidate node for moving to routing DAG must be an OUTPUT or TRANSFER_OUT node.");
							}
						}

						// If the DAG actually has nodes in it, then save it; otherwise, delete it
						if (newRoutingDAG->allNodes.empty())
						{
							delete newRoutingDAG;
							delete newRoutingCompiledDAG;
							cout << "\tNo operations; routing DAG discarded." << endl << endl;
							return;
						}
						else
						{
							(*conditionToRoutingDAGs)[c]->push_back(newRoutingCompiledDAG);
						}
					}
					else
					{
						cout << "Generating routing DAG for transition: " << dependentDAG->GetPrintableName() << " ===> NONE" << endl;
						claim(false, "NOT YET HANDLED.");
					}
					generatedRoutingDAG = true;

				}
				else
				{
					unsupportedDAG = true;
					unsupportedDAGname = dependentDAG->GetPrintableName();
				}
			}
			claim(!(generatedRoutingDAG && unsupportedDAG), getPrintableName() + " is dependent upon a DAG which is not itself(" + unsupportedDAGname + "). This behavior is not currently supported.");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
// This method should be executed AFTER all of the normal (non-routing) DAGs have
// been routed. It will find the RoutePoint locations of parent transfer-ins and
// child transfer-outs for routing purposes.
/////////////////////////////////////////////////////////////////////////////////
void CompiledDAG::initTransferLocations(map<DAG *, CompiledDAG *> *uncompToCompDAG)
{
	map<AssayNode *, AssayNode *>::iterator tiIt = ownTranInToParentTranOut->begin();
	for (; tiIt != ownTranInToParentTranOut->end(); tiIt++)
	{
		// Copy parent TRANSFER_OUT node's last droplet location to own TRANSFER_IN's location
		AssayNode *ownTi = tiIt->first;
		AssayNode *parTo = tiIt->second;
		claim(parTo->GetTransferRef(), "The transfer out edge from " + parTo->GetDAG()->GetPrintableName() + " must have a routing reference point.");
		ownTi->SetTransferRef(parTo->GetTransferRef());
	}

	map<AssayNode *, AssayNode *>::iterator toIt = ownTranOutToChildTranIn->begin();
	for (; toIt != ownTranOutToChildTranIn->end(); toIt++)
	{
		// Copy child TRANSFER_IN node's first droplet location to own TRANSFER_OUT's location
		AssayNode *ownTo = toIt->first;
		AssayNode *childTi = toIt->second;
		claim(childTi->GetTransferRef(), "The transfer in edge from " + childTi->GetDAG()->GetPrintableName() + " must have a routing reference point.");
		ownTo->SetTransferRef(childTi->GetTransferRef());
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Examines the routes data-structure and returns the biggest cycle number
/////////////////////////////////////////////////////////////////////////////////
unsigned long long CompiledDAG::getNumExecutionCycles()
{
	unsigned long long maxCycle = 0;
	map<Droplet *, vector<RoutePoint *> *>::iterator routesIt = routes->begin();
	for (; routesIt != routes->end(); routesIt++)
	{
		vector<RoutePoint *> *route = routesIt->second;
		if (!route->empty() && route->back())
			if (route->back()->cycle > maxCycle)
				maxCycle = route->back()->cycle;
	}
	return maxCycle;
}
