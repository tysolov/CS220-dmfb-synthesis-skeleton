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
 * Source: compiled_cfg.cc														*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: August 15, 2015							*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * TL		10/02/18	Refactored for CS220									*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "compiled_cfg.h"
#include "file_out.h"

/////////////////////////////////////////////////////////////////////////////////
// CompiledCFG Constructor/Destructor
/////////////////////////////////////////////////////////////////////////////////
CompiledCFG::CompiledCFG(Synthesis *synthEngine, CFG * cfgToCompile)
{
	synthesisEngine = synthEngine;
	uncompiledCFG = cfgToCompile;
	allNodes = new vector<CompiledDAG *>();
	uncompToCompDAG = new map<DAG *, CompiledDAG *>();
	dagsToExecute = new list<DAG *>();
	dagsBeingExecuted = new list<DAG *>();
	cgsToProcess = new list<ConditionalGroup *>();
	preRoutingDagsToExecute = new list<CompiledDAG *>();
	numDagsExecuted = 0;

	rModules = new vector<ReconfigModule *>();
	routes = new map<Droplet *, vector<RoutePoint *> *>();
	dirtyCells = new vector<vector<RoutePoint*> *>();
	pinActivations = new vector<vector<int> *>();
	tsBeginningCycle = new vector<unsigned long long>();

	// Create vectors for each condition
	conditionToRoutingDAGs = new map<Condition *, vector<CompiledDAG *> *>();
	for (ConditionalGroup *cg : uncompiledCFG->conditionalGroups)
		for (Condition *c : *(cg->getConditions()))
			(*conditionToRoutingDAGs)[c] = new vector<CompiledDAG *>();

	// Set the seed to something random....
	string r1 = "RANDOM SEED "; string r2 = " CAN BE USED TO RE-PRODUCE THIS RUN.";
	//seed = time(NULL); stringstream ssr; ssr << "RANDOM SEED " << seed << " CAN BE USED TO RE-PRODUCE THIS RUN."; seedMsg = ssr.str(); // This is the proper seed to use for random results
	// ...or instead, comment out line above and uncomment line below for manual seed...
	seed = 1534884714; stringstream ssm; ssm << "MANUAL SEED " << seed << " IS BEING USED TO RE-PRODUCE THIS RUN."; seedMsg = ssm.str(); // To manually set a seed for testing

	srand (seed);

	staticCompile();
}

CompiledCFG::~CompiledCFG()
{
	uncompiledCFG = NULL; // Deleted elsewhere

	while (!allNodes->empty())
	{
		CompiledDAG *d = allNodes->back();
		allNodes->pop_back();
		delete d;
	}
	delete allNodes;

	delete uncompToCompDAG;

	// Delete the routing DAGs
	while (!conditionToRoutingDAGs->empty())
	{
		Condition *c = conditionToRoutingDAGs->begin()->first;
		vector<CompiledDAG *> *compiledDAGs = conditionToRoutingDAGs->begin()->second;
		conditionToRoutingDAGs->erase(c);
		//delete d;
		while (!compiledDAGs->empty())
		{
			CompiledDAG *routingDAG = compiledDAGs->back();
			compiledDAGs->pop_back();
			delete routingDAG;
		}
		delete compiledDAGs;
	}
	delete conditionToRoutingDAGs;



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
		//delete d; // The droplet reference here is a copy and will be deleted by the original compiledDAG in which it was created
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


	// DAGs deleted in CFG destructor, so just delete local variables here
	dagsToExecute->clear();
	delete dagsToExecute;
	dagsBeingExecuted->clear();
	delete dagsBeingExecuted;
	cgsToProcess->clear();
	delete cgsToProcess;
	preRoutingDagsToExecute->clear();
	delete preRoutingDagsToExecute;
}


/////////////////////////////////////////////////////////////////////////////////
// This method is used internally to statically compile the DAGs contained within
// the CFG. In essence, a new CFG is created which contains compiled information
// along-side the normal DAG data structures.
/////////////////////////////////////////////////////////////////////////////////
void CompiledCFG::staticCompile() {
    cout << "Compiling CFG" << endl << "-----------------------------------------------" << endl;

    // Schedule/map each DAG
    for (int i = 0; i < uncompiledCFG->allDags.size(); i++) {
        // Initialize data-structures
        DAG *uncompiledDAG = uncompiledCFG->allDags.at(i);
        CompiledDAG *compiledDAG = new CompiledDAG(synthesisEngine, uncompiledDAG);
        allNodes->push_back(compiledDAG);
        (*uncompToCompDAG)[uncompiledDAG] = compiledDAG; // Map uncompiled to compiled

        // Schedule initial DAGs
        compiledDAG->scheduleDAG();
    }

    // Place/route each DAG, given the schedule
    for (DAG *uncompiledDAG : uncompiledCFG->allDags) {
        CompiledDAG *compiledDAG = uncompToCompDAG->at(uncompiledDAG);
        compiledDAG->placeDAG();
        if (uncompiledDAG->getAllNodes().size() > 0)
            compiledDAG->routeDAG();
    }

	// Now that we have obtained the initial schedules, refactor DAGs to add (pure) routing DAGs
	for (DAG * uncompiledDAG : uncompiledCFG->allDags)
    {
		CompiledDAG * compiledDAG = uncompToCompDAG->at(uncompiledDAG);
		compiledDAG->generateRoutingDAGs(uncompiledCFG->conditionalGroups, uncompToCompDAG, conditionToRoutingDAGs);
	}

	// Next, iterate through all the conditional groups to compile the routing DAGs
	// (they can now pull source/destination info from the normal (non-routing) DAGs
	cout << "Synthesizing routing DAGs" << endl;
	for (ConditionalGroup *cg : uncompiledCFG->conditionalGroups)
	{
		for (Condition *c : *(cg->getConditions()))
		{
			// If found the matching target CompiledDAG, then process
			map<Condition *, vector<CompiledDAG *> *>::iterator it = conditionToRoutingDAGs->find(c);
			if (it != conditionToRoutingDAGs->end())
			{
				for (CompiledDAG *routingDAG : (*it->second))
				{
					routingDAG->scheduleDAG();
					routingDAG->placeDAG();
					routingDAG->initTransferLocations(uncompToCompDAG);
					routingDAG->routeDAG();
				}
			}
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////
// The main public method which executes the contained uncompiledCFG on the
// contained (DMFB) arch(itecture). It must first pre-compile uncompiledCFG's
// contained DAGs, and then dynamically execute compiledCFG's protocol.
/////////////////////////////////////////////////////////////////////////////////
void CompiledCFG::execute()
{
	initForExecution();
	dynamicallyExecute();
}

/////////////////////////////////////////////////////////////////////////////////
// Simply clears the data-structures needed to execute the CFG.
/////////////////////////////////////////////////////////////////////////////////
void CompiledCFG::initForExecution()
{
	dagsToExecute->clear();
	dagsBeingExecuted->clear();
	cgsToProcess->clear();
	preRoutingDagsToExecute->clear();
	numDagsExecuted = 0;
}

/////////////////////////////////////////////////////////////////////////////////
// This method references the compiled CFG and dynamically executes it by
// choosing the next CFG node (i.e., DAG) to execute based on previous/current
// sensor-based feedback referenced in any conditional statemetns in the CFG.
/////////////////////////////////////////////////////////////////////////////////
void CompiledCFG::dynamicallyExecute()
{
	cout << "-----------------------------------------------" << endl;
	cout << "-----------------Executing CFG-----------------" << endl;
	cout << "-----------------------------------------------" << endl;

	unsigned long long totalCycles = 0;
	unsigned long long totalTS = 0;
	unsigned long long totalPinActivations = 0;
	map<DAG *, string> executingDagToIfElseFlow;

	/////////////////////////////////////////////////////////////////////////////////
	// Generate CFG flow-chart based on execution path below
	dagOutputNum = 0;
	ofstream exPathOut;
	exPathOut.open("Output/EXECUTION_PATH_CFG.dot");
	exPathOut<<"digraph G {\n";
	exPathOut << "node [shape=box, style=\"rounded, filled\"]\n";

	/////////////////////////////////////////////////////////////////////////////////
	// Add initial head Assays to list to execute
	list<Condition *> dagBranchedFrom; // the corresponding conditon that the dagToExecute came from (NULL if header)
	list<DAG *>::iterator headIt = uncompiledCFG->heads.begin();
	for (; headIt != uncompiledCFG->heads.end(); headIt++)
	{
		dagsToExecute->push_back(*headIt);
		dagBranchedFrom.push_back(NULL);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Determine which DAG to execute next
	while (!dagsToExecute->empty() || !dagsBeingExecuted->empty())
	{
		///////////////////////////////////////
		// First, try to execute the ready dags
		list<DAG *>::iterator dteIt = dagsToExecute->begin();
		list<Condition *>::iterator dbfIt = dagBranchedFrom.begin();
		while (dteIt != dagsToExecute->end())
		{
			DAG * dag = *dteIt;
			Condition *c = *dbfIt;

			// If it still needs to process some conditional groups...
			if (dag->cgsLeftToProcess.size() == 0)
			{
				// Add to dagBeingExecuted list and mark as running
				dagsBeingExecuted->push_back(dag);
				dag->status = RUNNING;

				// Grab any pre-routing stage that needs to be run and save
				if (conditionToRoutingDAGs->count(c))
				{
					if (!conditionToRoutingDAGs->at(c)->empty())
					{
						claim(conditionToRoutingDAGs->at(c)->size() == 1, "A condition which has more than 1 dependent DAG is not supported in code yet as there were no working benchmarks to test this feature.");
						preRoutingDagsToExecute->push_back(conditionToRoutingDAGs->at(c)->front());
					}
					else
					    preRoutingDagsToExecute->push_back(NULL);
				}
				else
					preRoutingDagsToExecute->push_back(NULL);

				// Add the CGs to the dag's personal todoList
				list<ConditionalGroup *>::iterator cgIt = dag->conditionalGroups.begin();
				for (; cgIt != dag->conditionalGroups.end(); cgIt++)
					dag->cgsLeftToProcess.push_back(*cgIt); // Add to list
				dagsToExecute->erase(dteIt++);
				dagBranchedFrom.erase(dbfIt++);
			}
			else // else...examine the next ready DAG
			{
				dteIt++;
				dbfIt++;
			}
		}

		/////////////////////////////////////////////////////////////////////////////////
		// Add pin-activation sequences (and other data for visualization) for DAGs in
		// executing and mark as RUN_FINISHED
		list<CompiledDAG *>::iterator prdIt = preRoutingDagsToExecute->begin();
		for (DAG *executingDAG : *dagsBeingExecuted)
		{
			// First, find and execute any pre-routing DAGs
			CompiledDAG *preRoutingDag = *prdIt;
			if (preRoutingDag)
			{
				executeCompiledDag(preRoutingDag, totalTS, totalCycles, true);
				unsigned long long dagCycles = preRoutingDag->getNumExecutionCycles();
				totalCycles += dagCycles;
				unsigned long long dagTS = preRoutingDag->uncompiledDAG->getMaxOpStartingTS();
				totalTS += dagTS;
				unsigned long long dagPins = preRoutingDag->pinActivations->size();
				totalPinActivations += dagPins;
				cout << "R" << numDagsExecuted << ") Executing " << preRoutingDag->getPrintableName() << endl;
				cout << "\t" << dagPins << " sub-assay pin activations; " << totalPinActivations << " total CFG pin activations" << endl;
				cout << "\t" << dagTS << " sub-assay time-steps; " << totalTS << " total CFG time-steps" << endl;
				cout << "\t" << dagCycles << " sub-assay execution cycles; " << totalCycles << " total CFG cycles" << endl;
				addFlowChartTransition(&exPathOut, "(Routing)", preRoutingDag->uncompiledDAG, true, true, true);
			}
			cout << "-----------------------------------------------" << endl;

			// Simulate sensor readings, process DAG as executed and add to global
			// data-structures for one complete visualization
			executingDAG->SimulateSensorReadings();
			CompiledDAG *compExecutingDAG = uncompToCompDAG->at(executingDAG);
			executingDAG->status = RUN_FINISHED;
			executingDAG->IncrementRunCount();
			executeCompiledDag(compExecutingDAG, totalTS, totalCycles, false);

			// Print simple statistics for the assay that was just executed
			unsigned long long dagCycles = compExecutingDAG->getNumExecutionCycles();
			totalCycles += dagCycles;
			unsigned long long dagTS = executingDAG->getMaxOpStartingTS();
			totalTS += dagTS;
			unsigned long long dagPins = compExecutingDAG->pinActivations->size();
			totalPinActivations += dagPins;
			cout << ++numDagsExecuted << ") Executing " << compExecutingDAG->getPrintableName() << endl;
			cout << "\t" << dagPins << " sub-assay pin activations; " << totalPinActivations << " total CFG pin activations" << endl;
			cout << "\t" << dagTS << " sub-assay time-steps; " << totalTS << " total CFG time-steps" << endl;
			cout << "\t" << dagCycles << " sub-assay execution cycles; " << totalCycles << " total CFG cycles" << endl;
			cout << endl;

			// Add to flow chart
			map<DAG *, string>::iterator ieIt = executingDagToIfElseFlow.find(compExecutingDAG->uncompiledDAG);

			if (ieIt != executingDagToIfElseFlow.end() && ieIt->second != "")
			{
				// Fix \n symbols and remove \t symbols
				string ifElseFixedForGraphs = ieIt->second;
				size_t start_pos = 0;
				while((start_pos = ifElseFixedForGraphs.find("\n", start_pos)) != std::string::npos)
				{
					ifElseFixedForGraphs.replace(start_pos, 1, "\\l");
				}
				start_pos = 0;

				addFlowChartTransition(&exPathOut, ifElseFixedForGraphs, compExecutingDAG->uncompiledDAG, false, true, true);
				executingDagToIfElseFlow[compExecutingDAG->uncompiledDAG] = "";
			}
			else
				addFlowChartTransition(&exPathOut, "(Unknown Condition Met)", compExecutingDAG->uncompiledDAG, false, true, true);

			prdIt++;
		}

		/////////////////////////////////////////////////////////////////////////////////
		// Check if any of the executing dags are done; add appropriate/ready CGs to
		// processing list
		list<DAG *>::iterator exIt = dagsBeingExecuted->begin();
		prdIt = preRoutingDagsToExecute->begin();
		while (exIt != dagsBeingExecuted->end())
		{
			DAG *d = *exIt;
			if (d->status == RUN_FINISHED)
			{
				// If this is a tail, then it can be run again immediately
				if (d->conditionalGroups.size() == 0)
				{
					d->status = CAN_RUN_AGAIN;
					d->ReInitSensorReadings();
				}

				// If any of d's CGs are ready to be evaluated, add them to the cgsToProcess list
				list<ConditionalGroup *>::iterator cglIt = d->cgsLeftToProcess.begin();
				for (; cglIt != d->cgsLeftToProcess.end(); cglIt++)
				{
					ConditionalGroup * cgl = *cglIt;

					// If all the dependent DAGs are finished, we can evaluate the CG
					// The last (Else) condition has all dependents (still true for 1-condition CGs)
					Condition * elseCon = cgl->getConditions()->back();
					bool canEvaluateCG = true;
					list<DAG *>::iterator depIt = elseCon->dependents.begin();
					while (depIt != elseCon->dependents.end())
					{
						if ((*depIt)->status != RUN_FINISHED)
							canEvaluateCG = false;
						depIt++;
					}
					if (canEvaluateCG)
					{	// Add if unique
						cgsToProcess->remove(cgl);
						cgsToProcess->push_back(cgl);
					}
				}
				dagsBeingExecuted->erase(exIt++);
				preRoutingDagsToExecute->erase(prdIt++);
			}
			else
			{
				exIt++;
				prdIt++;
			}
		}
		/////////////////////////////////////////////////////////////////////////////////
		// Then, process the conditions and add new branching dags to the ready list.
		// If a CG is in this list, it is ready to be processed.
		while (!cgsToProcess->empty())
		{
			stringstream ifElseFlowResults;
			ConditionalGroup *cg = cgsToProcess->front();
			Condition *c = cg->evaluateAndDbPrint(&ifElseFlowResults);
			if (c && c->branchIfTrue)
			{
				dagsToExecute->push_back(c->branchIfTrue);
				dagBranchedFrom.push_back(c);
				executingDagToIfElseFlow[c->branchIfTrue] = ifElseFlowResults.str();
			}

			// Must remove this CG from the dependent dags' "cgsLeftToProcess" lists
			Condition * elseCond = cg->getConditions()->back();
			list<DAG *>::iterator depIt2 = elseCond->dependents.begin();
			for (; depIt2 != elseCond->dependents.end(); depIt2++)
			{
				DAG *depDag = *depIt2;
				depDag->cgsLeftToProcess.remove(cg);

				// If possible, Reset so the DAG can be run again in the future if necessary
				if (depDag->cgsLeftToProcess.size() == 0 && depDag->status == RUN_FINISHED)
				{
					depDag->status = CAN_RUN_AGAIN;
					depDag->ReInitSensorReadings();
				}
			}
			cgsToProcess->pop_front();
		}
	} // End of dagsToExecute WHILE-LOOP


	cout << "-----------------------------------------------" << endl;
	cout << "-----------------------------------------------" << endl;
	cout << "Computing dirty cells..." << std::flush;

	/////////////////////////////////////////////////////////////////////////////////
	// Now that the routes data-structure is properly populated, compute dirty cells
	synthesisEngine->getRouter()->computeDirtyCells(routes, dirtyCells);

	/////////////////////////////////////////////////////////////////////////////////
	// Close CFG flow-chart based on execution path below
	exPathOut << "}\n";
	exPathOut.close();

	cout << "Done." << endl;

	/////////////////////////////////////////////////////////////////////////////////
	// Final output of simple statistics
	cout << "-----------------------------------------------" << endl;
	cout << "-----------------------------------------------" << endl;
	cout << "Total pin activations for CFG:  " << totalPinActivations << endl;
	cout << "Total scheduled time-steps: " << totalTS << " (" << (totalTS * synthesisEngine->getArch()->getSecPerTS()) << " seconds)" << endl;
	cout << "Total routing time-steps: " << totalCycles / synthesisEngine->getArch()->getFreqInHz() - totalTS << " (" << (totalCycles / synthesisEngine->getArch()->getFreqInHz()) - (totalTS * synthesisEngine->getArch()->getSecPerTS()) << " seconds)" << endl;
	cout << "Total execution cycles (scheduled + routing): " << totalCycles << " (" << (totalCycles / synthesisEngine->getArch()->getFreqInHz()) << " seconds)" << endl;
	cout << seedMsg << endl;
}


/////////////////////////////////////////////////////////////////////////////////
// This method "executes" the compiled DAG, which essentially adds the pin-
// activations and other relevant data-structures to the overall execution
// history for proper playback of the assay and proper visualization.
/////////////////////////////////////////////////////////////////////////////////
void CompiledCFG::executeCompiledDag(CompiledDAG *cDag, unsigned long long tsOffset, unsigned long long cycleOffset, bool isRoutingDag)
{
	/////////////////////////////////////////////////////////////////////////////////
	// Make new copies of all the reconfigurable modules with updated time-step
	// offsets to reflect the actual run-time results
	for (ReconfigModule *rm : *(cDag->rModules))
	{
		ReconfigModule *newRm = new ReconfigModule(rm->getResourceType(), rm->getLX(), rm->getTY(), rm->getRX(), rm->getBY());
		newRm->startTimeStep = rm->startTimeStep + tsOffset;
		newRm->endTimeStep = rm->endTimeStep + tsOffset;
		newRm->boundNode = rm->boundNode;
		rModules->push_back(newRm);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Make new copies of the routes (droplet references are being reproduced,
	// so don't delete in this constructor)
	map<Droplet *, vector<RoutePoint *> *>::iterator routeIt = cDag->routes->begin();
	bool containsTransferIn = false;
	bool containsTransferOut = false;
	unsigned long long maxNonOffsetCycle = 0;
	for (; routeIt != cDag->routes->end(); routeIt++)
	{
		// Grab the info from the compiled DAG
		Droplet *d = routeIt->first;
		vector<RoutePoint *> *route = routeIt->second;

		// Create a "new" route to contain copies of the route points
		if (routes->count(d) == 0)
			(*routes)[d] = new vector<RoutePoint *>();

		// Make copies of each route point to original droplets
		if (d && route && !route->empty())
		{
			for (RoutePoint *rp : *route)
			{
				// If this is a routing DAG and the transfer-in point, then skip;
				// otherwise, add new routing point as seen below
				if (rp->dStatus != DROP_TRANSFER_IN)
				{
					RoutePoint *newRp = new RoutePoint();
					newRp->cycle = rp->cycle + cycleOffset;
					newRp->dStatus = rp->dStatus;
					newRp->x = rp->x;
					newRp->y = rp->y;
					routes->at(d)->push_back(newRp);

					if (rp->cycle > maxNonOffsetCycle)
						maxNonOffsetCycle = rp->cycle;

					if (rp->dStatus == DROP_TRANSFER_OUT)
						containsTransferOut = true;
				}
				else
					containsTransferIn = true;
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Add copies of the time-step and routing phase beginnings with the updated
	// cycle offsets
	for (unsigned long long cycle : *(cDag->tsBeginningCycle))
		tsBeginningCycle->push_back(cycle + cycleOffset);
	// If this DAG contains a transfer-out, then there is a routing stage immediately
	// following this DAG, thus, the end of the sub-assay cannot be the beginning of
	// a time-step
	if (containsTransferOut)
		tsBeginningCycle->pop_back();

	/////////////////////////////////////////////////////////////////////////////////
	// First, equalize lengths of pin-activation vectors; it should be as
	// long as the current cycle offset....
	for (unsigned long long i = pinActivations->size(); i < cycleOffset; i++)
		pinActivations->push_back(new vector<int>());

	// ...then, make copies of the pin-activations; usually, last cycle is a "blank" cycle with no droplets,
	// which is why we only go to the next-to-last pinActivation vector
	for (int i = 0; i < cDag->pinActivations->size()-1; i++)
	{
		// For dags that contain a transfer-in, ignore the first cycle as the transfer_in
		// point has been removed
		if (!(containsTransferIn && i == 0))
		{
			vector<int> *pinActs = cDag->pinActivations->at(i);
			pinActivations->push_back(new vector<int>());
			for (int pinNum : *pinActs)
				pinActivations->back()->push_back(pinNum);
		}
	}

}

/////////////////////////////////////////////////////////////////////////////////
// Outputs the main simulation file(s) needed for visualizations of the entire
// simulation (including the unique path taken through the CFG during this run/
// instance).
/////////////////////////////////////////////////////////////////////////////////
void CompiledCFG::outputSimulationFiles()
{
	DmfbArch *arch = synthesisEngine->getArch();

	/////////////////////////////////////////////////////////
	// Routing --> Output Interface
	// NOTE: Must run the WriteCompacted() function last b/c
	// it deletes the routes as it goes.
	/////////////////////////////////////////////////////////

	cout << "-----------------------------------------------" << endl;
    cout << "-----------------------------------------------" << endl;
	cout << "Outputting simulation files..." << endl;

	cout << "Done.\nOutputting routes..." << std::flush;
	FileOut::WriteRoutedDagAndArchToFile(NULL, arch, synthesisEngine->getRouter(), rModules, routes, dirtyCells, pinActivations, tsBeginningCycle, "Output/3_ROUTE_to_SIM.txt");

	if (CompatChk::CanPerformCompactSimulation(synthesisEngine->getRouter()))
	    FileOut::WriteCompactedRoutesToFile(NULL, arch, rModules, routes, tsBeginningCycle, "Output/3_COMPACT_ROUTE_to_SIM.txt", uncompiledCFG);

	cout << "Done." << endl;
}

/////////////////////////////////////////////////////////////////////////////////
// Constructs flow-chart of execution history based on results of dynamic
// simulation.
/////////////////////////////////////////////////////////////////////////////////
void CompiledCFG::addFlowChartTransition(ofstream *out, string transitionReason, DAG *nextDAG, bool isRoutingDag, bool fullStats, bool color)
{
	dagOutputNum++;

	/////////////////////////////////////////////////////////
	// Draw node
	DAG *d = nextDAG;

	// Make the routing-DAGs dotted so they don't stick out as much
	if (isRoutingDag)
		*out << dagOutputNum <<" [style=\"rounded, filled, dotted\", label = \"" << dagOutputNum << ") " << d->GetPrintableName();
	else
		*out << dagOutputNum <<" [label = \"" << dagOutputNum << ") " << d->GetPrintableName();

	if (fullStats)
	{
		int numTransIn = 0;
		int numTransOut = 0;
		for (int j = 0; j < d->heads.size(); j++)
			if (d->heads.at(j)->GetType() == TRANSFER_IN)
				numTransIn++;
		for (int j = 0; j < d->tails.size(); j++)
			if (d->tails.at(j)->GetType() == TRANSFER_OUT)
				numTransOut++;

		*out << "\\n(" << d->allNodes.size() << " nodes";
		if (numTransIn > 0)
			*out << ",\\n" << numTransIn <<" drops in";
		if (numTransOut > 0)
			*out << ",\\n" << numTransOut <<" drops out";
		*out << ")\"";
	}
	else
		*out<<"\"";

	if (color)
	{
		if (isRoutingDag)
			*out << ", fillcolor=gray97];\n";
		else
			*out << ", fillcolor=gray90];\n";
	}
	else
		*out << ", fillcolor=white];\n";

	/////////////////////////////////////////////////////////
	// Draw transition
	if (dagOutputNum != 1)
	{
		*out << (dagOutputNum-1) << " -> " << dagOutputNum;
		*out << " [";
		if (fullStats)
			*out << "fontcolor=\"black" << color << "\", label=\"" << transitionReason << "\", ";
		*out << "color=\"black" << color << "\"]";
		*out << ";\n";
	}
}
