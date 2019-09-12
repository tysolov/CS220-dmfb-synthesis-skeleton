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
 * Source: synthesis.cc															*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "synthesis.h"

#include "file_out.h"
#include "file_in.h"
#include "analyze.h"

#include "cs220_scheduler.h"
#include "list_scheduler.h"
#include "scheduler.h"

#include "cs220_placer.h"
#include "grissom_left_edge_binder.h"
#include "placer.h"

#include "cs220_router.h"
#include "roy_maze_router.h"
#include "router.h"

//#include "cs220_wire_router.h"
#include "path_finder_wire_router.h"
#include "wire_router.h"

//#include "cs220_pin_mapper.h"
#include "indiv_addr_pin_mapper.h"
#include "pin_mapper.h"


///////////////////////////////////////////////////////////////
// Synthesis constructor. Initializes synthesis methods
///////////////////////////////////////////////////////////////
Synthesis::Synthesis(SchedulerType st, PlacerType pt, RouterType rt, bool performWash, ResourceAllocationType rat, PinMapType pmt, WireRouteType wrt, CompactionType ct, ProcessEngineType pet, ExecutionType et, DAG *assay, DmfbArch *dmfbArch)
{
	arch = dmfbArch;
	dag = assay;
	executionType = et;
	rModules = new vector<ReconfigModule *>();
	routes = new map<Droplet *, vector<RoutePoint *> *>();
	dirtyCells = new vector<vector<RoutePoint*> *>();
	tsBeginningCycle = new vector<unsigned long long>();
	pinActivations = new vector<vector<int> *>();

	scheduler = getNewSchedulingMethod(st);
	placer = getNewPlacementMethod(pt);
	router = getNewRoutingMethod(rt, arch);
	router->setWash(performWash);

	scheduler->setType(st);
	placer->setType(pt);
	placer->setPastSchedType(st);
	router->setType(rt);
	router->setPastSchedType(st);
	router->setPastPlacerType(pt);

	setWireRoutingMethod(wrt, arch);
	setPinMappingMethod(pmt, rat, arch);
	arch->getPinMapper()->setType(pmt);
	arch->getPinMapper()->setMapPreSched(); // Set Pin-mapping (if implemented)
	router->setRoutingParams(ct, pet);
}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
Synthesis::~Synthesis()
{
	delete scheduler;
	delete placer;
	delete router;
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
		v->clear(); // Not new RoutePoints; deleted elsewhere
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
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Simply deletes and re-news the variables in synthesis.  This function is called in-between
// scheduling, placement and routing so that the interfaces (file input and output) can be
// properly used.  This is used to ensure that the scheduling, placement and routing stages
// are properly separated from each other.
///////////////////////////////////////////////////////////////////////////////////////////////
void Synthesis::deleteAndRenewVariables()
{
	delete arch;
	delete dag;
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
	while (!pinActivations->empty())
	{
		vector<int> *pins = pinActivations->back();
		pinActivations->pop_back();
		pins->clear();
		delete pins;
	}


	arch = new DmfbArch();
	dag = new DAG();
	rModules = new vector<ReconfigModule *>();
	routes = new map<Droplet *, vector<RoutePoint *> *>();
	pinActivations = new vector<vector<int> *>();
	tsBeginningCycle = new vector<unsigned long long>();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// This function calls the scheduilng, placement and routing functions to compute a design
// for the given assay and DMFB architecture.  Also performs timing calculations.
///////////////////////////////////////////////////////////////////////////////////////////////
void Synthesis::synthesizeDesign()
{
	stringstream fName;
	string dir = "Output/";

	// Set the frequency for the DAG based on the architecture frequency
	dag->setFreq(arch->getFreqInHz());

	/////////////////////////////////////////////////////////
	// Pre-synthesis
	/////////////////////////////////////////////////////////
	if (executionType == SIM_EX || executionType == ALL_EX)
	{
		FileOut::WriteDagToFile(dag, "Output/0_DAG_to_SCHED.txt");
		fName.str("0_" + dag->getName());
		dag->OutputGraphFile(fName.str(), true, true);
		cout << dag->getAllNodes().size() << " total nodes; " << dag->getNumNonIoNodes() << " non-I/O nodes." << endl;
	}

	/////////////////////////////////////////////////////////
	// Set Pin-mapping (if implemented)
	/////////////////////////////////////////////////////////
	//arch->getPinMapper()->setMapPreSched();

	/////////////////////////////////////////////////////////
	// Do compatability checks
	/////////////////////////////////////////////////////////
	CompatChk::PreScheduleChk(scheduler, arch, dag, true);

	/////////////////////////////////////////////////////////
	// Scheduling ///////////////////////////////////////////
	/////////////////////////////////////////////////////////
	if (scheduler->hasExecutableSynthMethod())
	{
		ElapsedTimer sTime("Scheduling Time");
		sTime.startTimer();
		scheduler->schedule(arch, dag);
		sTime.endTimer();
		sTime.printElapsedTime();

		/////////////////////////////////////////////////////////
		// Scheduling --> Placement Interface
		// NOTE: This is unnecessary, but is used to keep the
		// stages of synthesis properly separated.
		/////////////////////////////////////////////////////////
		if (executionType == SIM_EX || executionType == ALL_EX)
		{
			FileOut::WriteScheduledDagAndArchToFile(dag, arch, scheduler, "Output/1_SCHED_to_PLACE.txt");
			//deleteAndRenewVariables();
			//Util::ReadScheduledDagAndArchFromFile(dag, arch, placer, "Output/1_SCHED_to_PLACE.txt");
			fName.str("1_" + dag->getName() + "_Sched");
			dag->OutputGraphFile(fName.str(), true, true);
		}

		/////////////////////////////////////////////////////////
		// Schedule analysis
		/////////////////////////////////////////////////////////
		FileOut::WriteStringToFile(Analyze::AnalyzeSchedule(dag, arch, scheduler), "Output/4_ANALYSIS_SCHEDULE.txt");
		cout << endl;
	}
	else
		cout << "Basic scheduling bypassed according to synthesis flow." << endl;

	/////////////////////////////////////////////////////////
	// Do compatability checks
	/////////////////////////////////////////////////////////
	CompatChk::PrePlaceChk(placer, arch, true);

	/////////////////////////////////////////////////////////
	// Placement ////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	if (placer->hasExecutableSynthMethod())
	{
		ElapsedTimer pTime("Placement Time");
		pTime.startTimer();
		placer->place(arch, dag, rModules);
		pTime.endTimer();
		pTime.printElapsedTime();

		/////////////////////////////////////////////////////////
		// Placement --> Routing Interface
		// NOTE: This is unnecessary, but is used to keep the
		// stages of synthesis properly separated.
		/////////////////////////////////////////////////////////
		if (executionType == SIM_EX || executionType == ALL_EX)
		{
			FileOut::WritePlacedDagAndArchToFile(dag, arch, placer, rModules, "Output/2_PLACE_to_ROUTE.txt");
			//deleteAndRenewVariables();
			//Util::ReadPlacedDagAndArchFromFile(dag, arch, rModules, "Output/2_PLACE_to_ROUTE.txt");
			fName.str("2_" + dag->getName() + "_Placed");
			dag->OutputGraphFile(fName.str(), true, true);
		}

		/////////////////////////////////////////////////////////
		// Placement analysis
		/////////////////////////////////////////////////////////
		FileOut::WriteStringToFile(Analyze::AnalyzePlacement(arch, rModules), "Output/4_ANALYSIS_PLACEMENT_WARNINGS_AND_FAILURES.txt");
		cout << endl;
	}
	else
		cout << "Basic placement bypassed according to synthesis flow." << endl;

	/////////////////////////////////////////////////////////
	// Do compatability checks
	/////////////////////////////////////////////////////////
	CompatChk::PreRouteChk(router, arch, true);

	/////////////////////////////////////////////////////////
	// Routing //////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	if (router->hasExecutableSynthMethod())
	{
		ElapsedTimer rTime("Routing Time");
		rTime.startTimer();
		router->route(dag, arch, rModules, routes, pinActivations, tsBeginningCycle);
		rTime.endTimer();
		rTime.printElapsedTime();

		/////////////////////////////////////////////////////////
		// Compute droplet locations from pin-activations
		// (if required)
		/////////////////////////////////////////////////////////
		if (router->getDropLocationsFromSim())
		{
			ElapsedTimer simTime("Simulation Time");
			simTime.startTimer();
			router->simulateDropletMotion(routes, pinActivations);
			simTime.endTimer();
			simTime.printElapsedTime();
		}
		else
		{
			ElapsedTimer simTime("Pin-Compute From Droplet-Motion Time");
			simTime.startTimer();
			router->setPinActivationsFromDropletMotion(routes, pinActivations);
			simTime.endTimer();
			simTime.printElapsedTime();
		}
		router->computeDirtyCells(routes, dirtyCells);
		printRoutingStats(tsBeginningCycle, arch);

		/////////////////////////////////////////////////////////
		// Droplet concentration and route interference analysis
		/////////////////////////////////////////////////////////
		if (CompatChk::CanPerformRouteAnalysis(router))
		{
			FileOut::WriteStringToFile(Analyze::AnalyzeDropletConcentrationAndIO(dag, arch, routes), "Output/4_ANALYSIS_DROPLET_IO_AND_CONCENTRATIONS.txt");
			FileOut::WriteStringToFile(Analyze::AnalyzeRoutes(arch, routes), "Output/4_ANALYSIS_ROUTING_FAILURES.txt");
			cout << endl;
		}
	}
	else
		cout << "Basic droplet routing bypassed according to synthesis flow." << endl;

	/////////////////////////////////////////////////////////
	// Set Pin-mapping (if implemented)
	/////////////////////////////////////////////////////////
	ElapsedTimer pmTime("Pin-Mapping (Post Route) Time");
	pmTime.startTimer();
	arch->getPinMapper()->setMapPostRoute(pinActivations, routes);
	pmTime.endTimer();
	pmTime.printElapsedTime();

	/////////////////////////////////////////////////////////
	// Do compatibility checks
	/////////////////////////////////////////////////////////
	CompatChk::PreWireRouteChk(arch, true);

	/////////////////////////////////////////////////////////
	// Compute wire-routes from pin-mapping and pin-activations
	// (if required)
	/////////////////////////////////////////////////////////
	if (arch->getWireRouter()->hasExecutableSynthMethod())
	{
		ElapsedTimer wrTime("Wire-Routing Time");
		wrTime.startTimer();
		arch->getWireRouter()->computeWireRoutes(pinActivations, false);
		wrTime.endTimer();
		wrTime.printElapsedTime();
	}
	else
		cout << "Basic wire-routing bypassed according to synthesis flow." << endl;

	printWireRoutingStats(arch);

	/////////////////////////////////////////////////////////
	// Routing --> Output Interface
	// NOTE: Must run the WriteCompacted() function last b/c
	// it deletes the routes as it goes.
	/////////////////////////////////////////////////////////
	if (executionType == PROG_EX || executionType == ALL_EX)
	{
		FileOut::WriteDmfbProgramToFile(routes, "Output/3_ELEC_ACTIVATIONS_COORDS.mfprog");
		FileOut::WriteDmfbBinaryProgramToFile(arch, pinActivations, "Output/3_PIN_ACTIVATIONS_BINARY.mfprog");
	}
	if (executionType == SIM_EX || executionType == ALL_EX)
	{
		FileOut::WriteRoutedDagAndArchToFile(dag, arch, router, rModules, routes, dirtyCells, pinActivations, tsBeginningCycle, "Output/3_ROUTE_to_SIM.txt");
		if (CompatChk::CanPerformCompactSimulation(router))
			FileOut::WriteCompactedRoutesToFile(dag, arch, rModules, routes, tsBeginningCycle, "Output/3_COMPACT_ROUTE_to_SIM.txt");
	}
	FileOut::WriteHardwareFileWithWireRoutes(arch, dir + "4_HARDWARE_DESCRIPTION.txt", true);
}

///////////////////////////////////////////////////////////////////////////////////////
// This creates a new synthesis-engine, which essentially acts as a holder of synthesis
// settings and types for an external system to perform partial synthesis.
///////////////////////////////////////////////////////////////////////////////////////
Synthesis * Synthesis::CreateNewSynthesisEngine(string archFile, SchedulerType st, PlacerType pt, RouterType rt, bool performWash, ResourceAllocationType rat, PinMapType pmt, WireRouteType wrt, CompactionType ct, ProcessEngineType pet, ExecutionType et, int maxStorageDropsPerMod, int cellsBetweenModIR, int numHorizTracks, int numVertTracks)
{
	// Read Architectural Description File
	DmfbArch * arch = FileIn::ReadDmfbArchFromFile(archFile);

	// Select Synthesis Methods and synthesize/create design
	Synthesis *syn = new Synthesis(st, pt, rt, performWash, rat, pmt, wrt, ct, pet, et, NULL, arch);
	syn->getScheduler()->setMaxStoragePerModule(maxStorageDropsPerMod);
	syn->getPlacer()->setMaxStoragePerModule(maxStorageDropsPerMod);
	syn->getPlacer()->setHCellsBetweenModIR(cellsBetweenModIR);
	syn->getPlacer()->setVCellsBetweenModIR(cellsBetweenModIR);
	syn->getRouter()->setHCellsBetweenModIR(cellsBetweenModIR);
	syn->getRouter()->setVCellsBetweenModIR(cellsBetweenModIR);
	syn->getArch()->getWireRouter()->setNumTracksAndCreateModel(numHorizTracks, numVertTracks);

	return syn;
}


///////////////////////////////////////////////////////////////////////////////////////
// Initializes a new scheduler
///////////////////////////////////////////////////////////////////////////////////////
Scheduler * Synthesis::getNewSchedulingMethod(SchedulerType st)
{
    Scheduler *s = new Scheduler();
	if (st == LIST_S)
		return new ListScheduler();
	else if (st == CS220_S)
		return new CS220Scheduler();
	else
		claim(false, "No valid scheduler type was specified.");
	return s;
}
///////////////////////////////////////////////////////////////////////////////////////
// Initializes a new placer
///////////////////////////////////////////////////////////////////////////////////////
Placer * Synthesis::getNewPlacementMethod(PlacerType pt)
{
    Placer* p = new Placer();
	if (pt == GRISSOM_LE_B)
		return new GrissomLEBinder();
	else if (pt == CS220_P)
		return new CS220Placer;
	else
		claim(false, "No valid placement type was specified.");
	return p;
}
///////////////////////////////////////////////////////////////////////////////////////
// Initializes a new router
///////////////////////////////////////////////////////////////////////////////////////
Router * Synthesis::getNewRoutingMethod(RouterType rt, DmfbArch *arch)
{
    Router *r = new Router();
	if (rt == ROY_MAZE_R)
		return new RoyMazeRouter(arch);
	else if (rt == CS220_R)
		return new CS220Router(arch);
	else
		claim(false, "No valid router type was specified.");
	r->setHasExecutableSynthMethod(false);
	return r;
}

///////////////////////////////////////////////////////////////////////////////////////
// Initializes a new router
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::setWireRoutingMethod(WireRouteType wrt, DmfbArch *arch)
{
	WireRouter *wr;
	if (wrt == PATH_FINDER_WR)
		wr = new PathFinderWireRouter(arch);
	else if (wrt == NONE_WR || PIN_MAPPER_INHERENT_WR)
	{
		wr = new WireRouter(arch);
		wr->setType(wrt);
		wr->setHasExecutableSynthMethod(false);
	}
	else
		claim(false, "No valid wire-router type was specified.");

	wr->setArch(arch); // Shouldn't be necessary, but 'arch' is not "sticking" in the constructor
	arch->setWireRouter(wr);
}
///////////////////////////////////////////////////////////////////////////////////////
// Initializes a new pin-mapper
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::setPinMappingMethod(PinMapType pmt, ResourceAllocationType rat, DmfbArch *arch)
{
	PinMapper *pm;
	if (pmt == INDIVADDR_PM)
		pm = new IndivAddrPinMapper(arch);
//	else if (pmt == CS220_PM)
//		pm = new CS220PinMapper(arch);
	else
		claim(false, "No valid pin-mapper type was specified.");

	pm->setType(pmt);
	pm->setArch(arch); // Shouldn't be necessary, but 'arch' is not "sticking" in the constructor
	pm->setResAllocType(rat);
	arch->setPinMapper(pm);
}

///////////////////////////////////////////////////////////////////////////////////////
// Prints number of cycles spent routing
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::printRoutingStats(vector<unsigned long long> *tsBeginningCycle, DmfbArch *arch)
{
	unsigned long long routingCycles = 0;
	for (unsigned i = 0; i < tsBeginningCycle->size(); i++)
	{
		unsigned long long lastTSEnd = 0;
		if (i > 0)
			lastTSEnd = tsBeginningCycle->at(i-1) + (int)(arch->getFreqInHz()*arch->getSecPerTS());
		routingCycles += (tsBeginningCycle->at(i)-lastTSEnd);
	}
	cout << "Number of cycles spent routing: " << routingCycles << " (" << (double)((double)routingCycles/arch->getFreqInHz()) << "s @" << arch->getFreqInHz() << "Hz)" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////
// Prints the total length of wiring on each layer in wire-routing grid units
///////////////////////////////////////////////////////////////////////////////////////
void Synthesis::printWireRoutingStats(DmfbArch *arch)
{
	// Print the usage of pins and electrodes
	int elecCount = 0;
	set<int> pins;
	for (int x = 0; x < arch->getPinMapper()->getPinMapping()->size(); x++)
	{
		for (int y = 0; y < arch->getPinMapper()->getPinMapping()->at(x)->size(); y++)
		{

			if (arch->getPinMapper()->getPinMapping()->at(x)->at(y) >= 0)
			{
				pins.insert(arch->getPinMapper()->getPinMapping()->at(x)->at(y));
				elecCount++;
			}
		}
	}
	cout << "DMFB consumes area of " << arch->getNumCellsX() << "x" << arch->getNumCellsY() << endl;
	cout << "Total number of electrodes wired: " << elecCount << endl;
	cout << "Total number of external pins: " << pins.size() << endl;
}