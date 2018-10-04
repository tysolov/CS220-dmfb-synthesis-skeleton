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
 * Name: Compiled DAG															*
 *																				*
 * Details: This class contains code for a compiled DAG. It contains a 			*
 * reference to the original DAG, but also contains a number of other data		*
 * structures that are used to store compiled details of the DAG				*
 * (i.e., pin-activations) as well as other auxilary info for visualization 	*
 * purposes (i.e., dirty cells, module locations/times, etc.).					*
 *-----------------------------------------------------------------------------*/
#ifndef HEADERS_MODELS_COMPILED_DAG_H_
#define HEADERS_MODELS_COMPILED_DAG_H_

#include "dag.h"
#include "synthesis.h"
#include <unordered_set>
#include <unordered_map>
#include "operation_coalescing_pool.h"
#include "conditional_group.h"

class CompiledDAG
{
	private:
		// Variables needed for compilation/visualization
		Synthesis *synthesisEngine;
		string printableName;
		DAG *uncompiledDAG;
		vector<ReconfigModule *> *rModules;
		map<Droplet *, vector<RoutePoint *> *> *routes;
		vector<vector<RoutePoint*> *> *dirtyCells;
		vector<vector<int> *> *pinActivations;
		vector<unsigned long long> *tsBeginningCycle;
		map<AssayNode *, AssayNode *> *ownTranInToParentTranOut;
		map<AssayNode *, AssayNode *> *ownTranOutToChildTranIn;

		// Methods
		void compileDAG();
		void scheduleDAG();
		void placeDAG(CoalescingPool* = nullptr);
		void routeDAG();
		void generateRoutingDAGs(vector<ConditionalGroup *> conditionalGroups, map<DAG *, CompiledDAG *> *uncompToCompDAG, map<Condition *, vector<CompiledDAG *> *> *conditionToRoutingDAGs);
		void initTransferLocations(map<DAG *, CompiledDAG *> *uncompToCompDAG);

	public:
		// Constructors
		CompiledDAG(Synthesis *synthEngine, DAG *uncompiledDAG);
		virtual ~CompiledDAG();

		// Getters/Setters
		string getPrintableName() { return printableName; }
		unsigned long long getNumExecutionCycles();

		// Methods

		// Friend Classes
		friend class CompiledCFG;
};

#endif /* HEADERS_MODELS_COMPILED_DAG_H_ */

