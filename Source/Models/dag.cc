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
 * Source: dag.cc																*
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
#include "dag.h"
#include <fstream>
#include <set>

int DAG::orderNum = 1;
int DAG::next_id = 1;

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
DAG::DAG()
{
	id = next_id++;
	status = UN_RUN;
	runCount = 0;
}
/*
 * 		static int next_id;
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
 */

/////////////////////////////////////////////////////////////////////////////////////
//// Copy Constructor
/////////////////////////////////////////////////////////////////////////////////////
//DAG::DAG(const DAG &oldDag) {
//	id = oldDag.id;
//	status = oldDag.status;
//	runCount = oldDag.runCount;
//	freqInHz = oldDag.freqInHz;
//	orderNum = oldDag.orderNum;
//	name = oldDag.name;
//
//	for (AssayNode* node : oldDag.allNodes) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		n->startTimeStep = 0;
//		node->startTimeStep = 10;
//		cout << "n: " << n->startTimeStep << "node: " << node->startTimeStep << endl;
//
//		allNodes.push_back(n);
//	}
//	for (AssayNode* node : oldDag.heads) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		heads.push_back(n);
//	}
//	for (AssayNode* node : oldDag.tails) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		tails.push_back(n);
//	}
//	for (AssayNode* node : oldDag.mixes) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		mixes.push_back(n);
//	}
//	for (AssayNode* node : oldDag.dilutes) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		dilutes.push_back(n);
//	}
//
//	for (AssayNode* node : oldDag.splits) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		splits.push_back(n);
//	}
//	for (AssayNode* node : oldDag.heats) {
//			AssayNode* n = new AssayNode(node->GetType());
//			n = node;
//			heats.push_back(n);
//	}
//	for (AssayNode* node : oldDag.cools) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		cools.push_back(n);
//	}
//	for (AssayNode* node : oldDag.detects) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		detects.push_back(n);
//	}
//	for (AssayNode* node : oldDag.storage) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		storage.push_back(n);
//	}
//	for (AssayNode* node : oldDag.storageHolders) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		storageHolders.push_back(n);
//	}
//	for (AssayNode* node : oldDag.others) {
//		AssayNode* n = new AssayNode(node->GetType());
//		n = node;
//		others.push_back(n);
//	}
//	for (ConditionalGroup* cg : oldDag.conditionalGroups) {
//		ConditionalGroup* n = new ConditionalGroup();
//		for (Condition* c : *cg->getConditions()) {
//			n->addExistingCondition(c);
//		}
//		conditionalGroups.push_back(n);
//	}
//	for (ConditionalGroup* cg : oldDag.cgsLeftToProcess) {
//		ConditionalGroup* n = new ConditionalGroup();
//		for (Condition* c : *cg->getConditions()) {
//			n->addExistingCondition(c);
//		}
//		cgsLeftToProcess.push_back(n);
//	}
//}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
DAG::~DAG()
{
	heads.clear();
	tails.clear();
	mixes.clear();
	dilutes.clear();
	splits.clear();
	heats.clear();
	cools.clear();
	detects.clear();
	storage.clear();
	others.clear();

	// Delete the actual AssayNodes
	while (allNodes.size() > 0)
	{
		AssayNode *n = allNodes.front();
		allNodes.erase(allNodes.begin());
		delete n;
	}
	allNodes.clear();

	// Storage Holder nodes are separate from rest of nodes (allNodes)
	while (storageHolders.size() > 0)
	{
		AssayNode *n = storageHolders.front();
		storageHolders.erase(storageHolders.begin());
		delete n;
	}
	storageHolders.clear();
}

///////////////////////////////////////////////////////////////
// Add the node to the DAG and to the appropriate vectors
///////////////////////////////////////////////////////////////
AssayNode * DAG::AddDispenseNode(string inputWell, double volume)
{
	return AddDispenseNode(inputWell, volume, "");
}
AssayNode * DAG::AddMixNode(int numDropsBefore, double seconds)
{
	return AddMixNode(numDropsBefore, seconds, "");
}
AssayNode * DAG::AddDiluteNode(int numDropsBefore, double seconds)
{
	return AddDiluteNode(numDropsBefore, seconds, "");
}
AssayNode * DAG::AddSplitNode(bool isFaultTolerant, int numDropsAfter, double seconds)
{
	return AddSplitNode(isFaultTolerant, numDropsAfter, seconds, "");
}
AssayNode * DAG::AddHeatNode(double seconds)
{
	return AddHeatNode(seconds, "");
}
AssayNode * DAG::AddCoolNode(double seconds)
{
	return AddCoolNode(seconds, "");
}
AssayNode * DAG::AddDetectNode(int numDropsIO, double seconds)
{
	return AddDetectNode(numDropsIO, seconds, "");
}
AssayNode * DAG::AddOutputNode(string outputSink)
{
	return AddOutputNode(outputSink, "");
}
AssayNode * DAG::AddStorageNode()
{
	return AddStorageNode("");
}
AssayNode * DAG::AddGeneralNode()
{
	return AddGeneralNode("");
}
AssayNode * DAG::AddWashNode()
{
	return AddWashNode("");
}
AssayNode * DAG::AddTransInNode()
{
	return AddTransInNode("");
}
AssayNode * DAG::AddTransOutNode()
{
	return AddTransOutNode("");
}

///////////////////////////////////////////////////////////////
// Add the node to the DAG and to the appropriate vectors;
// also lets you specify a name
///////////////////////////////////////////////////////////////
AssayNode * DAG::AddDispenseNode(string inputWell, double volume, string nodeName)
{
	AssayNode *node = new AssayNode(DISPENSE);
	node->portName = inputWell;
	node->volume = volume;
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddMixNode(int numDropsBefore, double seconds, string nodeName)
{
	AssayNode *node = new AssayNode(MIX);
	node->numDrops = numDropsBefore;
	node->seconds = seconds;
	//node->cycles = (unsigned)ceil((double)freqInHz * seconds);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddDiluteNode(int numDropsBefore, double seconds, string nodeName)
{
	AssayNode *node = new AssayNode(DILUTE);
	node->numDrops = numDropsBefore;
	node->seconds = seconds;
	//node->cycles = (unsigned)ceil((double)freqInHz * seconds);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddSplitNode(bool isFaultTolerant, int numDropsAfter, double seconds, string nodeName)
{
	{	// Sanity check: Must split into at least 2 droplets
		stringstream msg;
		msg << "ERRORL. SPLIT node splits into " << numDropsAfter << " drops. Must split into at least 2 drop." << ends;
		claim(numDropsAfter >= 2, &msg);
	}

	AssayNode *node = NULL;
	if (isFaultTolerant)
		node = new AssayNode(FTSPLIT);
	else
		node = new AssayNode(SPLIT);
	node->numDrops = numDropsAfter;
	node->seconds = seconds;
	//node->cycles = (unsigned)ceil((double)freqInHz * seconds);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddHeatNode(double seconds, string nodeName)
{
	AssayNode *node = new AssayNode(HEAT);
	node->seconds = seconds;
	//node->cycles = (unsigned)ceil((double)freqInHz * seconds);
	node->name = nodeName;
	node->numDrops = 1;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddCoolNode(double seconds, string nodeName)
{
	AssayNode *node = new AssayNode(COOL);
	node->seconds = seconds;
	//node->cycles = (unsigned)ceil((double)freqInHz * seconds);
	node->name = nodeName;
	node->numDrops = 1;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddDetectNode(int numDropsIO, double seconds, string nodeName)
{
	AssayNode *node = new AssayNode(DETECT);
	node->seconds = seconds;
	//node->cycles = (unsigned)ceil((double)freqInHz * seconds);
	node->name = nodeName;
	node->numDrops = numDropsIO;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddOutputNode(string outputSink, string nodeName)
{
	AssayNode *node = new AssayNode(OUTPUT);
	node->portName = outputSink;
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddStorageNode(string nodeName)
{
	AssayNode *node = new AssayNode(STORAGE);
	node->name = nodeName;
	//node->status = COMPLETE;
	node->order = orderNum++;
	AddNodeToDAG(node);
	return node;
}
AssayNode *DAG::AddStorageHolderNode()
{
	AssayNode *node = new AssayNode(STORAGE_HOLDER);
	storageHolders.push_back(node);
	return node;

}
/*AssayNode * DAG::AddStorageNode(string nodeName)
{
	AssayNode *node = new AssayNode(STORAGE);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}*/
AssayNode * DAG::AddGeneralNode(string nodeName)
{
	AssayNode *node = new AssayNode(GENERAL);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddWashNode(string nodeName)
{
	AssayNode *node = new AssayNode(WASH);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddTransInNode(string nodeName)
{
	AssayNode *node = new AssayNode(TRANSFER_IN);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}
AssayNode * DAG::AddTransOutNode(string nodeName)
{
	AssayNode *node = new AssayNode(TRANSFER_OUT);
	node->name = nodeName;
	AddNodeToDAG(node);
	return node;
}


void DAG::AddNodeToDAG(AssayNode *node)
{
	OperationType opType = node->type;
	node->dag = this;

	stringstream ss("");

	allNodes.push_back(node);
	if (opType == DISPENSE)
	{
		heads.push_back(node);
		ss << "DIS" << node->id;
	}
	else if (opType == TRANSFER_IN)
	{
		heads.push_back(node);
		ss << "TI" << node->id;
	}
	else if (opType == OUTPUT)
	{
		tails.push_back(node);
		ss << "OUT" << node->id;
	}
	else if (opType == TRANSFER_OUT)
	{
		tails.push_back(node);
		ss << "TO" << node->id;
	}
	else if (opType == MIX)
	{
		mixes.push_back(node);
		ss << "MIX" << node->id;
	}
	else if (opType == DILUTE)
	{
		dilutes.push_back(node);
		ss << "DIL" << node->id;
	}
	else if (opType == SPLIT)
	{
		splits.push_back(node);
		ss << "SPL" << node->id;
	}
	else if (opType == FTSPLIT)
	{
		splits.push_back(node);
		ss << "FTSPL" << node->id;
	}
	else if (opType == HEAT)
	{
		heats.push_back(node);
		ss << "HEAT" << node->id;
	}
	else if (opType == COOL)
	{
		cools.push_back(node);
		ss << "COOL" << node->id;
	}
	else if (opType == DETECT)
	{
		detects.push_back(node);
		ss << "DET" << node->id;
	}
	else if (opType == STORAGE)
	{
		storage.push_back(node);
		ss << "STOR" << node->id;
	}
	else
	{
		others.push_back(node);
		ss << "GEN" << node->id;
	}

	// Make sure some name is set
	if (node->name.empty())
		node->name = ss.str();
}

///////////////////////////////////////////////////////////////
// This function is a utility function that is most likely run
// once to convert a function that has contiguous mix-split
// operations that can be better represented as a single
// dilute operation.  Older assays represented dilutions as
// a mix followed by a split b/c the dilution operation was
// not supported (it now is).
//
// So if p=parent, m=mix, s=split, c=child andwe have:
// p->m->s->c, we want to convert to p->d->c
//
// NOTE: This operation should only be performed on unscheduled
// DAGs.
///////////////////////////////////////////////////////////////
void DAG::ConvertMixSplitsToDilutes()
{
	for (int i = mixes.size()-1; i >= 0; i--)
	{
		AssayNode *m = mixes.at(i);
		claim (m->status == UNBOUND_UNSCHED, "Mix-split-to-dilution conversion can only be performed on unscheduled/unbound DAGs.");

		// Gather the needed info from the split and then remove it
		double splitTime = 0;
		//int splitChildren = 0;
		if (m->children.size() == 1 && m->children.at(0)->type == SPLIT)
		{
			AssayNode *s = m->children.at(0);
			splitTime = s->seconds;
			//splitChildren = s->children.size();




			// Now, create the dilute node
			AssayNode *d = AddDiluteNode(m->parents.size(), m->seconds + splitTime);
			//m->type = DILUTE;
			//m->seconds += splitTime;
			//m->children.clear();

			// Insert dilute before mix parents
			for (unsigned j = 0; j < m->parents.size(); j++)
			{
				AssayNode *p = m->parents.at(j);
				vector<AssayNode*>::iterator it;
				for (it = p->children.begin(); it != p->children.end(); it++)
				{
					if ((*it) == m)
					{
						p->children.erase(it);
						break;
					}
				}
				p->children.push_back(d);
				d->parents.push_back(p);
			}
			m->parents.clear();
			m->children.clear();
			RemoveNodeFromDAG(m);
			delete m;

			// Now add child node references to the dilute
			for (unsigned j = 0; j < s->children.size(); j++)
			{
				AssayNode *c = s->children.at(j);

				// Remove child node's references to the original split
				vector<AssayNode*>::iterator it;
				for (it = c->parents.begin(); it != c->parents.end(); it++)
				{
					if ((*it) == s)
					{
						c->parents.erase(it);
						break;
					}
				}
				c->parents.push_back(d);
				d->children.push_back(c);
			}
			s->parents.clear();
			s->children.clear();
			RemoveNodeFromDAG(s); // Remove split nodes from DAG vectors
			delete s;


		}
	}
	//OutputGraphFile("TestDag.txt", true, true);
	//PrintParChildRelationships();
}

///////////////////////////////////////////////////////////////
// This function is a utility function that is most likely run
// once to convert a function that has mixes that all need to
// be turned into dilutes where the second (new) droplet being
// output from the operation should be output.  IN the past, a
// mix would always result in a droplet that got twice as big.
// This is unrealistic and half of the droplet should exit the
// device.  This function should be called after
// ConvertMixSplitsToDilutes() because those operations already
// have the handling of the second droplet incorporated with the
// subsequent split operation.
//
// NOTE: This operation should only be performed on unscheduled
// DAGs.
///////////////////////////////////////////////////////////////
void DAG::ConvertMixesToDiluteWithSecondDropletOutputting()
{
	// Typically only one output port name/type, so find the first output node
	// that has a valid output port and let's output droplets there.
	string outputPort = "";
	for (unsigned i = 0; i < tails.size(); i++)
	{
		AssayNode *out = tails.at(i);
		if (out->type == OUTPUT)
		{
			outputPort = out->portName;
			break;
		}
	}
	claim(!outputPort.empty(), "Must have a valid output name to convert mixes to dilutions with droplet outputing.");



	for (unsigned i = 0; i < mixes.size(); i++)
	{
		AssayNode *m = mixes.at(i);
		claim (m->status == UNBOUND_UNSCHED, "Mix-split-to-dilution conversion can only be performed on unscheduled/unbound DAGs.");

		// First, convert the mix node to a dilute node
		m->type = DILUTE;
		stringstream ss;
		ss << "DIL" << m->id;
		m->name = ss.str();
		dilutes.push_back(m); // Add to dilutes list

		// Now, add new output node and add to dilute
		AssayNode *outputNode = AddOutputNode(outputPort);
		m->children.push_back(outputNode);
		outputNode->parents.push_back(m);
	}
	mixes.clear(); // Remove nodes from mixes list
	//OutputGraphFile("TestDag.txt", true, true);
	//PrintParChildRelationships();
}
///////////////////////////////////////////////////////////////
// Removes the node from the DAG (if found) vectors of nodes.
// Does not alter any edges or connections.
///////////////////////////////////////////////////////////////
void DAG::RemoveNodeFromDAG(AssayNode *node)
{
	OperationType opType = node->type;

	bool found = false;
	vector<AssayNode *>::iterator it = allNodes.begin();
	for (; it != allNodes.end(); it++)
	{
		if (node == *it)
		{
			allNodes.erase(it);
			found = true;
			break;
		}
	}

	{	// Sanity check: Node must be in DAG
		//stringstream msg;
		//msg << "ERRORL. Assay node " << node->GetName() << " cannot be removed because not found in dag " << GetPrintableName() << "." << endl;
		//claim(found, &msg);

		// Instead of erring, if not found, just return b/c it has already been removed
		if (!found)
			return;
	}

	vector<AssayNode *> *indivList = NULL;

	if (opType == DISPENSE || opType == TRANSFER_IN)
		indivList = &heads;
	else if (opType == OUTPUT || opType == TRANSFER_OUT)
		indivList = &tails;
	else if (opType == MIX)
		indivList = &mixes;
	else if (opType == DILUTE)
		indivList = &dilutes;
	else if (opType == SPLIT || opType == FTSPLIT)
		indivList = &splits;
	else if (opType == HEAT)
		indivList = &heats;
	else if (opType == COOL)
		indivList = &cools;
	else if (opType == DETECT)
		indivList = &detects;
	else if (opType == STORAGE)
		indivList = &storage;
	else
		indivList = &others;

	found = false;
	it = indivList->begin();
	for (; it != indivList->end(); it++)
	{
		if (node == *it)
		{
			indivList->erase(it);
			found = true;
			break;
		}
	}

	{	// Sanity check: Node must be in DAG
		stringstream msg;
		msg << "ERRORL. Assay node " << node->GetName() << " cannot be removed because not found in individual list in dag " << GetPrintableName() << "." << endl;
		claim(found, &msg);
	}
}

///////////////////////////////////////////////////////////////
// Insert a node in the middle of the original parent/child
// by changing the graph
///////////////////////////////////////////////////////////////
void DAG::InsertNode(AssayNode *p, AssayNode *c, AssayNode *insert)
{
	// Remove old connections
	vector<AssayNode*>::iterator it;
	for (it = p->children.begin(); it != p->children.end(); it++)
	{
		if ((*it) == c)
		{
			p->children.erase(it);
			break;
		}
	}
	for (it = c->parents.begin(); it != c->parents.end(); it++)
	{
		if ((*it) == p)
		{
			c->parents.erase(it);
			break;
		}
	}
	// Insert new connections
	p->children.push_back(insert);
	insert->parents.push_back(p);
	insert->children.push_back(c);
	c->parents.push_back(insert);
}

///////////////////////////////////////////////////////////////
// Make 'c' a child of 'p' (and 'p' and parent of 'c')
///////////////////////////////////////////////////////////////
void DAG::ParentChild(AssayNode *p, AssayNode *c)
{
	p->children.push_back(c);
	c->parents.push_back(p);
}

///////////////////////////////////////////////////////////////
// Get a name for printing
///////////////////////////////////////////////////////////////
string DAG::GetPrintableName()
{
	stringstream str;
	str << "DAG" << id;
	if (name != "")
		str << "(" << name << ")";
	return str.str();
}

///////////////////////////////////////////////////////////////
// Get a short name consisting of "DAG" + the id
///////////////////////////////////////////////////////////////
string DAG::GetIdName()
{
	stringstream str;
	str << "DAG" << id;
	return str.str();
}

///////////////////////////////////////////////////////////////
// Prints the scheduling time-step information for this DAG
///////////////////////////////////////////////////////////////
void DAG::PrintSchedule()
{
	cout << "Printing schedule for DAG:" << endl;
	cout << "Node Name: [start time-step, end time-step), Resource Type" << endl;
	for (unsigned i = 0; i < allNodes.size(); i++)
		allNodes.at(i)->Print();
}

///////////////////////////////////////////////////////////////
// Prints the parent/child relationships for this DAG
///////////////////////////////////////////////////////////////
void DAG::PrintParChildRelationships()
{

	for (unsigned i = 0; i < allNodes.size(); i++)
	{
		AssayNode *c = allNodes.at(i);
		cout << "---------------------------------------------" << endl;
		for (unsigned j = 0; j < c->parents.size(); j++)
		{
			cout << "\t";
			c->parents.at(j)->Print();
		}
		c->Print();
		for (unsigned j = 0; j < c->children.size(); j++)
		{
			cout << "\t";
			c->children.at(j)->Print();
		}
	}
}

///////////////////////////////////////////////////////////////
// Searches all nodes to find the node with a specific id.
///////////////////////////////////////////////////////////////
AssayNode * DAG::getNode(int id)
{
	for (AssayNode *n : allNodes)
		if (n->id == id)
			return n;
	return NULL;
}

///////////////////////////////////////////////////////////////
// Sets the frequency of the DAG by calculating the number of
// cycles each node will take at that frequency.
///////////////////////////////////////////////////////////////
void DAG::setFreq(unsigned long long freqHz)
{
	freqInHz = freqHz;
	for (unsigned i = 0; i < allNodes.size(); i++)
		allNodes.at(i)->cycles = (unsigned)ceil((double)freqInHz * allNodes.at(i)->seconds);
}

///////////////////////////////////////////////////////////////
//Creates a graph of the Dag in .dot format
///////////////////////////////////////////////////////////////
void DAG::OutputGraphFile(string filename, bool color, bool fullStats)
{

	//fullStats = false; // DTG DEBUG PRINT

	ofstream out;
	filename = "Output/" + filename + ".dot";

	out.open(filename.c_str());

	{
		stringstream str;
		str << "Failed to properly write DAG Graph file: " << filename << endl;
		claim (out.good(), &str);
	}

	out<<"digraph G {\n";
	//out<<"  size=\"8.5,10.25\";\n";
	//out << "label=\"" << filename << "\"";

	for (unsigned i = 0; i < allNodes.size(); ++i)
	{
		AssayNode *n = allNodes.at(i);

		string labelName;
		string colorName;
		stringstream ss;
		switch( n->GetType()){
			case GENERAL:{
				ss << "GEN_" << n->id << " ";
				//ss << "G" << " ";
				labelName = ss.str();
				colorName = "steelblue";
				break;
			}
			case STORAGE_HOLDER:{
				ss << "STORE_" << n->id << " ";
				//ss << "S" << " ";
				labelName = ss.str();
				colorName = "grey65";
				//colorName = "black";
				break;
			}
			case STORAGE:{
				ss << "IndivSTORE_" << n->id << " ";
				//ss << "S" << " ";
				labelName = ss.str();
				colorName = "grey65";
				//colorName = "black";
				break;
			}
			case DISPENSE:{
				ss << "IN_" << n->id << " " << n->GetPortName() << " ";
				//ss << "D" << " ";
				labelName = ss.str();
				colorName = "lightsteelblue";
				break;
			}
			case MIX:{
				ss << "MIX_" << n->id;
				//ss << "M" << " ";
				labelName = ss.str();
				colorName = "darkorange";
				break;
			}
			case DILUTE:{
				ss << "DIL_" << n->id;
				//ss << "M" << " ";
				labelName = ss.str();
				colorName = "goldenrod";
				break;
			}
			case SPLIT:{
				ss << "SPLT_" << n->id << " ";
				//ss << "S" << " ";
				labelName = ss.str();
				colorName = "deepskyblue";
				break;
			}
			case FTSPLIT:{
				ss << "FTSPLT_" << n->id << " ";
				//ss << "FTS" << " ";
				labelName = ss.str();
				colorName = "seagreen1";
				break;
			}
			case HEAT:{
				ss << "HEAT_" << n->id << " ";
				//ss << "H" << " ";
				labelName = ss.str();
				colorName = "tomato";
				break;
			}
			case COOL:{
				ss << "COOL_" << n->id << " ";
				//ss << "C" << " ";
				labelName = ss.str();
				colorName = "lightskyblue1";
				break;
			}
			case DETECT:{
				ss << "DET_" << n->id << " ";
				//ss << "Dt" << " ";
				labelName = ss.str();
				colorName = "tan";
				break;
			}
			case OUTPUT:{
				ss << "OUT_" << n->id << " " << n->GetPortName() << " ";
				//ss << "O" << " ";
				labelName = ss.str();
				colorName = "olivedrab";
				break;
			}
			case TRANSFER_IN: {
				ss << "TI_" << n->id << " ";
				//ss << "TI" << " ";
				labelName = ss.str();
				colorName = "cyan3";
				break;
			}
			case TRANSFER_OUT: {
				ss << "TO_" << n->id << " ";
				//ss << "TO" << " ";
				labelName = ss.str();
				colorName = "orchid3";
				break;
			}
			default:
			    stringstream msg;
			    msg << "Type not handled: " << n->GetType();
				claim(false, msg.str());
		}
		if (n->GetName() != "")
			labelName = labelName + "(" + n->GetName() + ")";
		out << n->getId() << " [label = \"" << labelName;
		//out << n->getId() << " [label = \"" << "";
		if(fullStats)
		{
			if (n->seconds > 0)
				out << " (" << n->seconds <<"s)\\n";
			else
				out << "\\n";
			if (n->GetStatus() >= SCHEDULED)
				out << "Sched: TS[" << n->startTimeStep << ", " << n->endTimeStep << ")\\n";
			if (n->GetStatus() >= BOUND)
			{
				if (n->GetType() == DISPENSE || n->GetType() == OUTPUT)
				{
					if (n->ioPort->getSide() == NORTH)
						out << "IoPort: " << n->ioPort->getId() << ", (" << n->ioPort->getPosXY() << ", NORTH)\\n";
					else if (n->ioPort->getSide() == SOUTH)
						out << "IoPort: " << n->ioPort->getId() << ", (" << n->ioPort->getPosXY() << ", SOUTH)\\n";
					else if (n->ioPort->getSide() == EAST)
						out << "IoPort: " << n->ioPort->getId() << ", (EAST, " << n->ioPort->getPosXY() << ")\\n";
					else if (n->ioPort->getSide() == WEST)
						out << "IoPort: " << n->ioPort->getId() << ", (WEST, " << n->ioPort->getPosXY() << ")\\n";
				}
				else if (n->reconfigMod != NULL)
				{
					if (n->reconfigMod->getTileNum() >= 0)
						out << "Place: M#" << n->reconfigMod->getTileNum() << ": LxTy(" << n->reconfigMod->getLX() << ", " << n->reconfigMod->getTY() << ")\\n";
					else
						out << "Place: LxTy(" << n->reconfigMod->getLX() << ", " << n->reconfigMod->getTY() << ")\\n";
				}

			}
			out << "\"";
		}
		else
			out << "\"";
		if (color)
			out << " fillcolor=" << colorName << ", style=filled];\n";
		else
			out << "];\n";

		for (unsigned j=0; j < n->children.size(); ++j)
			out << n->getId() << " -> " << n->children.at(j)->getId() << ";\n";
	}
	out<<"}\n";
}

///////////////////////////////////////////////////////////////
// Takes a constructed DAG and splits the heating operations]
// into two classes: heating and cooling. For all the temps
// specified in the assay, we simply remove the coolor 1/2 of
// temps from the heat operations and convert to cooling ops.
///////////////////////////////////////////////////////////////
void DAG::ConvertHeatOpsToHeatAndCool()
{
	set<double> tempSet;
	vector<double> temps;

	// Get unique temperatures
	for (int i = 0; i < heats.size(); i++)
	{
		AssayNode *h = heats.at(i);
		tempSet.insert(h->temperature);
	}

	// Now sort
	set<double>::iterator it = tempSet.begin();
	for (; it != tempSet.end(); it++)
		temps.push_back(*it);
	sort(temps.begin(), temps.end());

	double maxTemp = 0;
	for (int i = 0; i < temps.size() / 2; i++)
		maxTemp = temps.at(i);

	for (int i = heats.size()-1; i >= 0; i++)
	{
		AssayNode *h = heats.at(i);

		// Convert to a COOL operation
		if (h->temperature <= maxTemp)
		{
			heats.erase(heats.begin()+i);
			h->type = COOL;
			cools.push_back(h);
		}
	}
}

///////////////////////////////////////////////////////////////
// Add DAG to children list only if it isn't already in list
///////////////////////////////////////////////////////////////
void DAG::AddUniqueCG(ConditionalGroup *cg)
{
	conditionalGroups.remove(cg);
	conditionalGroups.push_front(cg);
}

///////////////////////////////////////////////////////////////
// Re-initializes DAG/Assay nodes' sensor readings to 0 for
// a new run.
///////////////////////////////////////////////////////////////
void DAG::ReInitSensorReadings()
{
	for (AssayNode *n : detects)
	{
		n->reading = 0;
		n->status = ROUTED;
	}
}

///////////////////////////////////////////////////////////////
// Simulates DAG/Assay nodes' sensor readings to a pseudo-random
// value between 0 and .99, with a resolution of .01
///////////////////////////////////////////////////////////////
void DAG::SimulateSensorReadings()
{
	for (AssayNode *n : detects)
	{
		n->reading = ((double)(rand() % 100)) / 100.0;
		n->status = COMPLETE;
	}
}

///////////////////////////////////////////////////////////////
// Re-initializes DAG/Assay so it can be run again if the CFG
// dictates such a thing.
///////////////////////////////////////////////////////////////
void DAG::ReInitAssay()
{

	for (int i = 0; i < allNodes.size(); i++)
	{
		AssayNode * n = allNodes.at(i);
		if (n->GetType() != TRANSFER_OUT)
		{
			n->reading = 0;
			n->reconfigMod = NULL;
			n->status = UNBOUND_UNSCHED;
			n->boundedResType = UNKNOWN_RES;
			n->startCycle = 1000000000;
			n->priority = 0;
			n->startTimeStep = 0;
			n->endTimeStep = 0;
			//n->dropletGIDs.clear();
			n->droplets.clear();
			//n->inputDropletGIDs.clear();
			n->storageOps.clear();


			/* From current assay_node.cc constructor - Added indicates was not in original dynamic method
			n->order = 0; // Added
			n->cycles = 0; // Added
			n->volume = 0; // Added
			n->reading = 0;
			n->seconds = 0; // Added
			n->portName = "EMPTY"; // Added
			n->status = UNBOUND_UNSCHED;
			n->boundedResType = UNKNOWN_RES;
			n->startCycle = 1000000000;
			n->priority = 0;
			n->startTimeStep = 0;
			n->endTimeStep = 0;
			n->numDrops = 0; // Added
			n->transfer = NULL; // Added
			n->reconfigMod = NULL;
			n->ioPort = NULL; // Added
			n->temperature = 0; // Added
			n->droplets.clear(); // Sort of added
			n->storageOps.clear()
			*/
		}
	}

	// Clear all storage elements so we can re-schedule DAG
	for (int i = allNodes.size() - 1; i >= 0; i--)
		if (allNodes.at(i)->GetType() == STORAGE)
			allNodes.erase(allNodes.begin()+i);
	while (!storage.empty())
	{
		AssayNode *sn = storage.back();
		AssayNode *p = sn->parents.front();
		AssayNode *c = sn->children.front();
		p->children.clear();
		p->children.push_back(c);
		c->parents.clear();
		c->parents.push_back(p);
		storage.pop_back();
		delete sn;
	}
}

///////////////////////////////////////////////////////////////
// Returns the latest time-step at which any operation starts.
///////////////////////////////////////////////////////////////
unsigned long long DAG::getMaxOpStartingTS()
{
	unsigned long long max = 0;
	for (AssayNode *n : tails)
		if (n->startTimeStep > max)
			max = n->startTimeStep;
	return max;
}
