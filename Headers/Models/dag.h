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
/*--------------------------------Class Details---------------------------------*
 * Name: DAG (Directed Acyclic Graph)											*
 *																				*
 * Details: This class represents a DAG, which represents an assay.  A DAG is	*
 * composed of a number AssayNodes.												*
 *-----------------------------------------------------------------------------*/
#ifndef _DAG_H
#define _DAG_H

#include "assay_node.h"
#include "enums.h"
#include "claim.h"
#include <stdio.h>
#include <vector>
#include <math.h>
#include <map>
#include "reconfig_module.h"
#include "sort.h"

using namespace std;

class ConditionalGroup;

class DAG : public Entity
{
	protected:
		// Variables
		static int next_id;
		vector<AssayNode*> allNodes;
		vector<AssayNode*> heads;
		vector<AssayNode*> tails;
		vector<AssayNode*> mixes;
		vector<AssayNode*> dilutes;
		vector<AssayNode*> splits;
		vector<AssayNode*> heats;
		vector<AssayNode*> cools;
		vector<AssayNode*> detects;
		vector<AssayNode*> storage;
		vector<AssayNode*> storageHolders;
		vector<AssayNode*> others;

		list<ConditionalGroup *> conditionalGroups;
		list<ConditionalGroup *> cgsLeftToProcess;
		DagStatus status;
		int runCount;
		unsigned long long freqInHz;
		static int orderNum;
		string name;

		// Methods
		void AddNodeToDAG(AssayNode *node);
		void RemoveNodeFromDAG(AssayNode *node);
		//AssayNode * AddStorageNode();
		void InsertNode(AssayNode *p, AssayNode *c, AssayNode *insert);
		void ReInitAssay();
		void ReInitSensorReadings();
		void SimulateSensorReadings();
		void IncrementRunCount() { runCount++; }
		void ResetRunCount() { runCount = 0; }

	public:
		// Constructors
		DAG();
//		DAG(const DAG&); //copy constructor
		virtual ~DAG();

		// Methods
		AssayNode * AddDispenseNode(string inputWell, double volume, string nodeName);
		AssayNode * AddMixNode(int numDropsBefore, double seconds, string nodeName);
		AssayNode * AddDiluteNode(int numDropsBefore, double seconds, string nodeName);
		AssayNode * AddSplitNode(bool isFaultTolerant, int numDropsAfter, double seconds, string nodeName);
		AssayNode * AddHeatNode(double seconds, string nodeName);
		AssayNode * AddCoolNode(double seconds, string nodeName);
		AssayNode * AddDetectNode(int numDropsIO, double seconds, string nodeName);
		AssayNode * AddOutputNode(string outputSink, string nodeName);
		AssayNode * AddStorageNode(string nodeName);
		AssayNode * AddGeneralNode(string nodeName);
		AssayNode * AddWashNode(string nodeName);
		AssayNode * AddTransInNode(string nodeName);
		AssayNode * AddTransOutNode(string nodeName);

		AssayNode * AddDispenseNode(string inputWell, double volume);
		AssayNode * AddMixNode(int numDropsBefore, double seconds);
		AssayNode * AddDiluteNode(int numDropsBefore, double seconds);
		AssayNode * AddSplitNode(bool isFaultTolerant, int numDropsAfter, double seconds);
		AssayNode * AddHeatNode(double seconds);
		AssayNode * AddCoolNode(double seconds);
		AssayNode * AddDetectNode(int numDropsIO, double seconds);
		AssayNode * AddOutputNode(string outputSink);
		AssayNode * AddStorageNode();
		AssayNode * AddStorageHolderNode();
		AssayNode * AddGeneralNode();
		AssayNode * AddWashNode();
		AssayNode * AddTransInNode();
		AssayNode * AddTransOutNode();

		void ConvertHeatOpsToHeatAndCool();
		void AddUniqueCG(ConditionalGroup *cg);
		void ParentChild(AssayNode *p, AssayNode *c);

		// Getters/Setters
		void setFreq(unsigned long long freqHz);
		void setName(string n) { name = n; }
		string getName() { return name; }
		int getRunCount() { return runCount; }
		AssayNode * getNode(int id);
		vector<AssayNode *> getAllNodes() { return allNodes; }
		vector<AssayNode *> getAllInputNodes() { return heads; }
		vector<AssayNode *> getAllOutputNodes() { return tails; }
		vector<AssayNode *> getAllStorageHolders() { return storageHolders; }
		int getNumNonIoNodes() { return (allNodes.size() - heads.size() - tails.size()); }
		bool requiresHeater() { return (heats.size() + cools.size()) > 0; }
		bool requiresDetector() { return !detects.empty(); }
		unsigned long long getMaxOpStartingTS();


		// Print/Debug
		void PrintSchedule();
		void PrintParChildRelationships();
		void OutputGraphFile(string filename, bool color, bool fullStats);
		string GetPrintableName();
		string GetIdName();

		// Transformation Functions - Used only once to convert assays in various ways
		void ConvertMixSplitsToDilutes();
		void ConvertMixesToDiluteWithSecondDropletOutputting();

		// Friend Classes
		friend class RealTimeEvalListScheduler;
		friend class GrissomFppcPathScheduler;
		friend class GrissomFppcScheduler;
		friend class GrissomFppcLEBinder;
		friend class GenetPathScheduler;
		friend class GrissomPathBinder;
		friend class RickettScheduler;
		friend class GrissomLEBinder;
		friend class GenetScheduler;
		friend class KamerLlPlacer;
		friend class NSGAIIPlacer;
		friend class ListScheduler;
		friend class PathScheduler;
		friend class FDLScheduler;
		friend class PinMapper;
		friend class Priority;
		friend class SAPlacer;
		friend class Analyze;
		friend class FileIn;
		friend class Test;
		friend class CFG;
		friend class CS220Scheduler;
		friend class CS220Placer;
		friend class CS220Router;

		friend class CompiledDAG;
		friend class CompiledCFG;
};
#endif //_DAG_H
