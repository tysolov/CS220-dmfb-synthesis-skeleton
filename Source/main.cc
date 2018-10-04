/*------------------------------------------------------------------------------*
 *                       (c)2018, All Rights Reserved.     						*
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
 * Source: main.cc																*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: October 7, 2012							*
 *																				*
 * Details: This framework is a bare-bones representation of a much larger one  *
 *            that is used for research in the synthesis of DMFBs.  It has      *
 *            everything you will need in order to implement your assigned      *
 *            synthesis algorithm.  There is a basic scheduler, placer and      *
 *            router provided, which may prove helpful in understanding the     *
 *            available APIs and the structures you must maintain for your      *
 *            given task.
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * TL		10/02/18	Ripped bare bones representation of MFSim for CS220 	*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#ifndef _MAIN
#define _MAIN

#include "synthesis.h"
#include "enums.h"
#include "compatibility_check.h"
#include <math.h>
#include <stdio.h>
#include <dirent.h>
#include "compiled_cfg.h"
#include "cfg.h"
#include "file_out.h"
#include "file_in.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////
// Main function called when starting program.
///////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	/// This skeleton supports basic scheduling, placement, and routing algorithms to successfully synthesize the
	///  provided assays.  Review these classes for insight into using the APIs when building your algorithm.

	SchedulerType st = LIST_S;
	// SchedulerType st = CS220_S;

	PlacerType pt = GRISSOM_LE_B;
	// PlacerType pt = CS220_P;

	RouterType rt = ROY_MAZE_R;
	// RouterType rt = CS220_R;


	// if you are implementing a binder (attached to fixed modules) rather than a reconfigurable placer (free placement)
	//  then modify this so that your binder will use the fixed processing engine
	ProcessEngineType pet = pt == GRISSOM_LE_B ? FIXED_PE : FREE_PE;

	// Provide the input files for testing
	string name = "PCR";
	string path = "Assays/CFGs/" + name + "/";
	Synthesis *synthesisEngine = Synthesis::CreateNewSynthesisEngine("Assays/CFGs/" + name + "/ArchFile/arch.txt",
			st, pt, rt, false, GRISSOM_FIX_0_RA, INDIVADDR_PM, PATH_FINDER_WR, BEG_COMP, pet, ALL_EX, 1, 1, 3, 3, false, false);

	/// Files will be written to the "Output" directory, do not delete this directory.
	DIR *theFolder = opendir("Output");
	struct dirent *next_file;
	char filepath[256];

	// Clear the Output directory
	while ( (next_file = readdir(theFolder)) != NULL )
	{
		if (0==strcmp(next_file->d_name, ".") || 0==strcmp(next_file->d_name, "..") || 0==strcmp(next_file->d_name, ".keep")) { continue; }
		sprintf(filepath, "%s/%s", "Output", next_file->d_name);
		remove(filepath);
	}
	closedir(theFolder);

	string cfg = path + name + ".cfg";
	cout << "Executing " << cfg << endl;
	CFG *cfgIn = FileIn::ReadCfgFromFile(cfg);

	CompiledCFG *compiledCFG = new CompiledCFG(synthesisEngine, cfgIn);

	compiledCFG->execute();
	compiledCFG->outputSimulationFiles();

	delete compiledCFG;
	delete cfgIn;

	return 0;
}


#endif
