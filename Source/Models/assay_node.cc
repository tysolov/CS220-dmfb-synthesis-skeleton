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
 * Source: assay_node.cc														*
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
#include "../../Headers/Models/assay_node.h"

int AssayNode::next_id = 1;

///////////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////////
AssayNode::AssayNode(OperationType opType)
{
	id = next_id++;
	type = opType;
	order = 0;
	cycles = 0;
	volume = 0;
	reading = 0;
	seconds = 0;
	portName = "EMPTY";
	status = UNBOUND_UNSCHED;
	boundedResType = UNKNOWN_RES;
	startCycle = 1000000000;
	priority = 0;
	startTimeStep = 0;
	endTimeStep = 0;
	numDrops = 0;
	reconfigMod = NULL;
	ioPort = NULL;
	temperature = 0;
	transferRef = NULL;
}

//
//AssayNode::AssayNode(const AssayNode& obj1) {
//	id = obj1.id;
//	type = obj1.type;
//	order = obj1.order;
//	cycles = obj1.cycles;
//	volume = obj1.volume;
//	reading = obj1.reading;
//	seconds = obj1.seconds;
//	portName = obj1.portName;
//	status = obj1.status;
//	boundedResType = obj1.boundedResType;
//	startCycle = obj1.startCycle;
//	priority = obj1.priority;
//	startTimeStep = obj1.startTimeStep;
//	endTimeStep = obj1.endTimeStep;
//	numDrops = obj1.numDrops;
//	reconfigMod = obj1.reconfigMod;
//	ioPort = obj1.ioPort;
//	temperature = obj1.temperature;
//	transferRef = obj1.transferRef;
//	dag = obj1.dag;
//	boundedExternalRes = obj1.boundedExternalRes;
//}

///////////////////////////////////////////////////////////////////////////////////
// Deconstructor
///////////////////////////////////////////////////////////////////////////////////
AssayNode::~AssayNode()
{
	children.clear();
	parents.clear();
	droplets.clear();
}

//////////////////////////////////////////////////////////////////
// With the assumption that architectures are using a 'default'
// 2x2 latency calibration (ex. 2x2 mixer at 9.95s to mix 1400nl),
// we scale the time needed for mixing as given by the assay with
// the multiplier (updated_latency/current_latency).
//////////////////////////////////////////////////////////////////
void AssayNode::UpdateLatency(LATENCY updated)
{
	seconds = seconds * (latencyLookup[updated] / latencyLookup[latency]);
	latency = updated;
}

//////////////////////////////////////////////////////////////////
// Tells whether the AssayNode is still "in control" of the droplet
// with global id "gid", or if it has given control to another node
//////////////////////////////////////////////////////////////////
bool AssayNode::isMaintainingDroplet(Droplet *drop)
{
	vector<Droplet *>::iterator it;
	for(it = droplets.begin(); it != droplets.end(); it++)
	{
		if (*it == drop)
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////
// Removes the given droplet from the node's droplet list
//////////////////////////////////////////////////////////////////
void AssayNode::eraseFromDropList(Droplet* drop)
{
	vector<Droplet *>::iterator it;
	for(it = droplets.begin(); it != droplets.end(); it++)
	{
		if (*it == drop)
		{
			droplets.erase(it);
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////
// Prints basic scheduling, name and resource info
//////////////////////////////////////////////////////////////////
string AssayNode::Print()
{
	string sum = GetSummary();
	cout << sum << endl;
	return sum;
}

//////////////////////////////////////////////////////////////////
// Gets basic scheduling, name and resource info
//////////////////////////////////////////////////////////////////
string AssayNode::GetSummary()
{
	stringstream ss;
	string t = "UNKNOWN";
	if (type == MIX)
		t = "mix";
	else if (type == DILUTE)
		t = "dilute";
	else if (type == SPLIT)
		t = "split";
	else if (type == HEAT)
		t = "heat";
	else if (type == DETECT)
		t = "detect";
	else if (type == DISPENSE)
		t = "in";
	else if (type == OUTPUT)
		t = "out";
	else if (type == STORAGE_HOLDER)
		t = "storage";
	else if (type == STORAGE)
		t = "individual storage";

	//cout << "P" << priority << " "; // DTG Debug

	ss << name << "(" << t << "): [" << startTimeStep << ", " << endTimeStep << "),";
	//cout << "\t";
	if (type == DISPENSE)
		ss << " \"" << portName << "\" Input Well";
	else if (type == OUTPUT)
		ss << " \"" << portName << "\" Output Sink";
	else
	{
		switch(boundedResType)
		{
			case BASIC_RES:
				ss << " Basic Module";
				break;
			case D_RES:
				ss << " Detecting Module";
				break;
			case H_RES:
				ss << " Heating Module";
				break;
			case DH_RES:
				ss << " Heating/Detecting Module";
				break;
			default:
				//cerr << endl << ERROR "UNKNOWN RESOURCE TYPE" << endl;
				//exit(1);
				ss << " Unknown Res Type";
				break;
		}
	}
	list<AssayNode*>::iterator it = storageOps.begin();
	for (; it != storageOps.end(); it++)
	{
		ss << endl << "\t";
		ss << (*it)->GetSummary();
	}

	return ss.str();
}
