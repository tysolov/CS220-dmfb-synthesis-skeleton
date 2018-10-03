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
 * Name: Control Flow Graph (CFG)												*
 *																				*
 * Code Developed and Described in:												*
 * Authors: Dan Grissom, Chris Curtis & Philip Brisk							*
 * Title: Interpreting Assays with Control Flow on Digital Microfluidic			*
 * 			Biochips															*
 * Publication Details: In ACM JETC in Computing Systems, Vol. 10, No. 3,		*
 * 					    Apr 2014, Article No. 24								*
 * 					    														*
 * Details: A logical structure used to mimic the behavior of a					*
 * control-flow graph (CFG).													*
 *-----------------------------------------------------------------------------*/

#ifndef _CFG_H
#define _CFG_H

#include "dag.h"
#include "conditional_group.h"
using namespace std;

//////////////////////////////////////////////////
// CFG does not have to be a DAG
//////////////////////////////////////////////////
class CFG
{
	protected:
		// Variables
		//static unsigned next_id;
		vector<DAG*> allDags;
		list<DAG*> heads;
		vector<ConditionalGroup *> conditionalGroups;
		string name;

	public:
		// Constructors
		CFG();
		virtual ~CFG();

		// Getters/Setters
		void setName(string n) { name = n; }
		string getName() { return name; }

		// Methods
		DAG * AddNewDAG(string name);
		DAG * AddNewDAG();
		DAG * AddExistingDAG(DAG *dag);

		ConditionalGroup * AddNewCG();

		// Print/Debug
		void OutputGraphFile(string filename, bool outputDAGs, bool color, bool fullStats);

		// Utility
		void ConstructAndValidateCFG();
		void convertFTSplits();
		void separateFTSplitParents(DAG *cd, DAG *pd, AssayNode *split);
		void recursiveSeparate(DAG *cd, DAG *pd, AssayNode *n);

		friend class CompiledCFG;
		friend class FileOut;

};
#endif
