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
 * Name: ConditionalGroup														*
 *																				*
 * Code Developed and Described in:												*
 * Authors: Dan Grissom, Chris Curtis & Philip Brisk							*
 * Title: Interpreting Assays with Control Flow on Digital Microfluidic			*
 * 			Biochips															*
 * Publication Details: In ACM JETC in Computing Systems, Vol. 10, No. 3,		*
 * 					    Apr 2014, Article No. 24								*
 * 					    														*
 * Details: A logical structure used to mimic IF-ELSEIF-ELSE structures.		*
 *-----------------------------------------------------------------------------*/
#ifndef _CONDITIONAL_GROUP_H
#define _CONDITIONAL_GROUP_H

#include "../Resources/enums.h"
#include "../Testing/claim.h"
#include "../Resources/structs.h"
#include "../Util/sort.h"
#include "../Resources/structs.h"

struct Condition;
struct TransferEdge;
class DAG;
class Expression;

/////////////////////////////////////////////////////////////////
// ConditionalGroup: Contains the boolean statements
// for an IF/ELSE-IF/ELSE conditional group statement.
// The first statement is an "IF"; if 2+ statements, the last is
// an "ELSE"; if 3+ statements, the middle are "ELSE-IFs".
/////////////////////////////////////////////////////////////////
class ConditionalGroup
{
    protected:
        // Variables
		vector<Condition *> *conditions;

		// Methods



    public:
        // Constructors
		ConditionalGroup();
        ~ConditionalGroup();

        // Getters/Setters
        vector<Condition *> * getConditions() { return conditions; }


        // Methods
        void addNewCondition(Expression *e, DAG *d);
        void addNewCondition(Expression *e, DAG *d, vector<TransferEdge *> transferEdges);
        void addExistingCondition(Condition *c);
        void swapConditions(int i1, int i2);
        void deleteCondition(int i);
        void deleteCondition(Condition *c);
        void printConditionalGroup();
        Condition * evaluate();
        Condition * evaluateAndDbPrint(stringstream *ifElseFlowResults);

 };
#endif
