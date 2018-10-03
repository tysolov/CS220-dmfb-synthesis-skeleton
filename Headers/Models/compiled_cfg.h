/*------------------------------------------------------------------------------*
 *                       (c)2018, All Rights Reserved.     						*
 *       ___           ___           ___     									*
 *      /__/\         /  /\         /  /\    									*
 *      \  \:\       /  /:/        /  /::\   									*
 *       \  \:\     /  /:/        /  /:/\:\  									*
 *   ___  \  \:\   /  /:/  ___   /  /:/~/:/        								*
 *  /__/\  \__\:\ /__/:/  /  /\ /__/:/ /:/___     UCR CS220 Synthesis Framework *
 *  \  \:\ /  /:/ \  \:\ /  /:/ \  \:\/:::::/           						*
 *   \  \:\  /:/   \  \:\  /:/   \  \::/~~~~ 									*
 *    \  \:\/:/     \  \:\/:/     \  \:\     									*
 *     \  \::/       \  \::/       \  \:\    									*
 *      \__\/         \__\/         \__\/    									*
 *-----------------------------------------------------------------------------*/
/*-----------------------------------Details------------------------------------*
 * Name: Compiled CFG															*
 * 																				*
 * Details: This class contains code for a compiled CFG. It contains a 			*
 * reference to the CFG/DAGs that are contained in a standard CFG, but also		*
 * contains a number of other data structures that are used to store compiled	*
 * details of the DAG (i.e., pin-activations) as well as other auxilary info	*
 * for visualization purposes (i.e., dirty cells, module locations/times, etc.).*
 *-----------------------------------------------------------------------------*/
#ifndef HEADERS_MODELS_COMPILED_CFG_H_
#define HEADERS_MODELS_COMPILED_CFG_H_

#include "cfg.h"
#include "../../Headers/Models/compiled_dag.h"
#include "../synthesis.h"

class CompiledCFG
{
	private:
		// Variables
		Synthesis *synthesisEngine;
		CFG *uncompiledCFG;
		vector<CompiledDAG *> *allNodes;
		map<DAG *, CompiledDAG *> *uncompToCompDAG;
		map<Condition *, vector<CompiledDAG *> *> *conditionToRoutingDAGs; // Vector of CompiledDAGs to run if Condition is found to be true
		list<DAG *> *dagsToExecute;
		list<DAG *> *dagsBeingExecuted;
		list<CompiledDAG *> *preRoutingDagsToExecute;
		list<ConditionalGroup *> *cgsToProcess;
		int numDagsExecuted;
		int dagOutputNum;
		int seed; // Seed for generating random, but repeatable traffic
		string seedMsg;
		map<int, vector<int> > adjList_IG;

		// Visualization Variables for Execution
		vector<ReconfigModule *> *rModules;
		map<Droplet *, vector<RoutePoint *> *> *routes;
		vector<vector<RoutePoint*> *> *dirtyCells;
		vector<vector<int> *> *pinActivations;
		vector<unsigned long long> *tsBeginningCycle;

		// Methods
		void staticCompile();
		void initForExecution();
		void dynamicallyExecute();
		void executeCompiledDag(CompiledDAG *cDag, unsigned long long tsOffset, unsigned long long cycleOffset, bool isRoutingDag);
		void addFlowChartTransition(ofstream *out, string transitionReason, DAG *nextDAG, bool isRoutingDag, bool fullStats, bool color);

	public:
		// Constructors
		CompiledCFG(Synthesis *synthEngine, CFG *uncompiledCFG);
		virtual ~CompiledCFG();

		// Getters/Setters

		// Methods
		void execute();
		void outputSimulationFiles();
};

#endif /* HEADERS_MODELS_COMPILED_CFG_H_ */
