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
 * Name: CmdLine (Command Line)													*
 *																				*
 * Details: This class handles the command line interface for the simulator.	*
 * It is designed to be easy to add new methods (schedulers, placers, etc.).	*
 * Simply add new entries in the constructor in command_line.cc, as shown in	*
 * that file.																	*
 *-----------------------------------------------------------------------------*/
#ifndef COMMAND_LINE_H_
#define COMMAND_LINE_H_

#include <string>
#include <vector>
#include "enums.h"

class CmdLine
{
	public:
		// Constructors
		CmdLine();
		virtual ~CmdLine();

		void ForceCorrectUsage();
		bool IsUsingCommandLine(int argc);
		void ExecuteCommand(int argc, char **argv);

		friend class FileOut;
		friend class FileIn;

	private:
		std::string GetUsageString();
		std::string GetSchedulerLibrary();
		int GetLibIndexFromSchedKey(std::string sKey);
		int GetLibIndexFromPlaceKey(std::string pKey);
		int GetLibIndexFromRouteKey(std::string rKey);
		int GetLibIndexFromResourceAllocationKey(std::string raKey);
		int GetLibIndexFromPinMapKey(std::string pmKey);
		int GetLibIndexFromCompKey(std::string cKey);
		int GetLibIndexFromPeKey(std::string peKey);
		int GetLibIndexFromEtKey(std::string etKey);
		int GetLibIndexFromWrKey(std::string wrKey);

		std::vector<std::string> sKeys; 		// The command-line key for a scheduler
		std::vector<std::string> sDescrips;	// Description/name of scheduler
		std::vector<SchedulerType> sEnums;// Enum-type of scheduler

		std::vector<std::string> pKeys; 		// The command-line key for a placer
		std::vector<std::string> pDescrips;	// Description/name of placer
		std::vector<PlacerType> pEnums;	// Enum-type of placer

		std::vector<std::string> rKeys; 		// The command-line key for a router
		std::vector<std::string> rDescrips;	// Description/name of router
		std::vector<RouterType> rEnums;	// Enum-type of router

		std::vector<std::string> raKeys; 		// The command-line key for a resource-allocator
		std::vector<std::string> raDescrips;	// Description/name of resource-allocator
		std::vector<ResourceAllocationType> raEnums;	// Enum-type of resource-allocator

		std::vector<std::string> pmKeys; 		// The command-line key for a pin-mapper
		std::vector<std::string> pmDescrips;	// Description/name of pin-mapper
		std::vector<PinMapType> pmEnums;	// Enum-type of pin-mapper

		std::vector<std::string> wrKeys; 		// The command-line key for a wire-router
		std::vector<std::string> wrDescrips;	// Description/name of wire-router
		std::vector<WireRouteType> wrEnums;	// Enum-type of wire-router

		std::vector<std::string> cKeys; 		// The command-line key for a compaction
		std::vector<std::string> cDescrips;	// Description/name of compaction type
		std::vector<CompactionType> cEnums;	// Enum-type of compaction

		std::vector<std::string> peKeys; 		// The command-line key for the time-step processing engine
		std::vector<std::string> peDescrips;	// Description/name of time-step processing engine
		std::vector<ProcessEngineType> peEnums;	// Enum-type of time-step processing

		std::vector<std::string> etKeys; 		// The command-line key for the execution type
		std::vector<std::string> etDescrips;	// Description/name of execution type
		std::vector<ExecutionType> etEnums;	// Enum-type of execution type
};


#endif /* COMMAND_LINE_H_ */
