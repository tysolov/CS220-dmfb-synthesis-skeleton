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
 * Source: conditional_group.cc													*
 * Original Code Author(s): Dan Grissom											*
 * Original Completion/Release Date: April 1, 2014								*
 *																				*
 * Details: N/A																	*
 *																				*
 * Revision History:															*
 * WHO		WHEN		WHAT													*
 * ---		----		----													*
 * FML		MM/DD/YY	One-line description									*
 *-----------------------------------------------------------------------------*/
#include "../../Headers/Models/conditional_group.h"

/////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////
ConditionalGroup::ConditionalGroup()
{
	conditions = new vector<Condition *>();
}
/////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////
ConditionalGroup::~ConditionalGroup()
{
	while (conditions->size() > 0)
	{
		Condition *c = conditions->back();
		while (c->transfers.size() > 0)
		{
			TransferEdge *te = c->transfers.back();
			c->transfers.pop_back();
			delete te;
		}

		conditions->pop_back();
		delete c;
	}
	delete conditions;
}
/////////////////////////////////////////////////////////////////////
// Methods
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Adds new condition to the end of the conditional group.
/////////////////////////////////////////////////////////////////////
void ConditionalGroup::addNewCondition(Expression *e, DAG *d)
{
	vector<TransferEdge *> emptyVect;
	addNewCondition(e, d, emptyVect);
}
/////////////////////////////////////////////////////////////////////
// Adds new condition to the end of the conditional group.  Also adds
// any transfer edges necessary to transfer droplets between DAGs
/////////////////////////////////////////////////////////////////////
void ConditionalGroup::addNewCondition(Expression *e, DAG *d, vector<TransferEdge *> transferEdges)
{
	{	// Sanity check: If e is NULL, definitely an ELSE...must be 1+ conditions already
		stringstream msg;
		msg << "ERROR. Cannot add an ELSE w/o at least one other condition" << ends;
		claim(!(!e && conditions->size() == 0), &msg);
	}
	Condition *c = new Condition();
	c->statement = e;
	c->branchIfTrue = d;
	c->order = conditions->size();

	for (int i = 0; i < transferEdges.size(); i++)
		c->transfers.push_back(transferEdges.at(i));

	conditions->push_back(c);
}

/////////////////////////////////////////////////////////////////////
// Adds an existing condition to the conditional group. Should ONLY
// be used when reading in a CFG from a file; otherwise, organizational
// structure of conditional groups could easily get messed up.
/////////////////////////////////////////////////////////////////////
void ConditionalGroup::addExistingCondition(Condition *c)
{
	c->order = conditions->size();
	conditions->push_back(c);
}


/////////////////////////////////////////////////////////////////////
// Swap based on 0-based index
/////////////////////////////////////////////////////////////////////
void ConditionalGroup::swapConditions(int i1, int i2)
{
	{	// Sanity check: Must be within range
		stringstream msg;
		msg << "ERROR. Swap indeces not in range." << ends;
		claim(i1 >= 0 && i1 < conditions->size() && i2 >= 0 && i2 < conditions->size(), &msg);
	}
	conditions->at(i1)->order = i2;
	conditions->at(i2)->order = i1;
	Sort::sortConditionsByOrder(conditions);
}
/////////////////////////////////////////////////////////////////////
// Delete based on 0-based index
/////////////////////////////////////////////////////////////////////
void ConditionalGroup::deleteCondition(int i)
{
	{	// Sanity check: Must be within range
		stringstream msg;
		msg << "ERROR. Swap indeces not in range." << ends;
		claim(i >= 0 && i < conditions->size(), &msg);
	}
	conditions->erase(conditions->begin()+i);

	for (int j = i; j < conditions->size(); j++)
		conditions->at(j)->order--; // Update order numbers
}
/////////////////////////////////////////////////////////////////////
// Delete based on actual condition pointer
/////////////////////////////////////////////////////////////////////
void ConditionalGroup::deleteCondition(Condition *c)
{
	for (int i = 0; i < conditions->size(); i++)
	{
		if (c == conditions->at(i))
		{
			deleteCondition(i);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////
// Print the IF/ELSE_IF/ELSE structure
/////////////////////////////////////////////////////////////////////
void ConditionalGroup::printConditionalGroup()
{
	for (int i = 0; i < conditions->size(); i++)
	{
		Expression *s = conditions->at(i)->statement;
		if (i == 0)
			cout << "IF " << s->printExpression(false) << endl << "\t// Do something" << endl;
		else if (i == conditions->size()-1)
			cout << "ELSE //" << s->printExpression(false) << endl << "\t// Do something" << endl;
		else
			cout << "ELSE IF " << s->printExpression(false) << endl << "\t// Do something" << endl;
	}
}
/////////////////////////////////////////////////////////////////////
// Evaluates the conditional group and returns the condition that
// evaluated to true.  Returns NULL if no conditions evaluated to
// true.
/////////////////////////////////////////////////////////////////////
Condition * ConditionalGroup::evaluate()
{
	for (int i = 0; i < conditions->size(); i++)
	{
		Condition *c = conditions->at(i);
		Expression *s = c->statement;
		if (i == 0) // "IF"
		{
			if (s->evaluateExpression())
				return c;
		}
		else if (i == conditions->size()-1) // "ELSE"
			return c;
		else // "ELSE-IF"
		{
			if (s->evaluateExpression())
				return c;
		}
	}
	return NULL; // If gets here, no branching
}
/////////////////////////////////////////////////////////////////////
// Same as "evaluate", but prints the conditions being evaluated.
/////////////////////////////////////////////////////////////////////
Condition * ConditionalGroup::evaluateAndDbPrint(stringstream *ifElseFlowResults)
{
	cout << "Evaluating Condition Dependencies -";
	Condition *last = conditions->back();
	list<DAG *>::iterator it = last->dependents.begin();
	for (; it != last->dependents.end(); it++)
		cout << " " << (*it)->GetPrintableName();
	cout << endl;

	for (int i = 0; i < conditions->size(); i++)
	{
		Condition *c = conditions->at(i);
		Expression *s = c->statement;
		if (i == 0) // "IF"
		{
			*ifElseFlowResults << "IF " << s->printExpression(true) << "\n"; // DEBUG PRINT
			if (s->evaluateExpression())
			{
				*ifElseFlowResults << "\tTRUE";
				if (c->branchIfTrue)
					*ifElseFlowResults << " - Branching to " << c->branchIfTrue->GetPrintableName() << "\n";
				else
					*ifElseFlowResults << " - No Branch." << "\n";
				cout << ifElseFlowResults->str() << endl;
				return c;
			}
			else
			{
				*ifElseFlowResults << "\tFALSE - No Branch." << "\n";
				if (conditions->size() == 1)
					*ifElseFlowResults << "\n";
			}
		}
		else if (i == conditions->size()-1) // "ELSE"
		{
			*ifElseFlowResults << "ELSE // " << s->printExpression(true) << "\n"; // DEBUG PRINT
			if (c->branchIfTrue)
				*ifElseFlowResults << "\tBranching to " << c->branchIfTrue->GetPrintableName() << "\n";
			else
				*ifElseFlowResults << "\tNo Branch." << "\n";
			cout << ifElseFlowResults->str() << endl;
			return c;
		}
		else // "ELSE-IF"
		{
			*ifElseFlowResults << "ELSE-IF " << s->printExpression(true) << "\n"; // DEBUG PRINT
			if (s->evaluateExpression())
			{
				*ifElseFlowResults << "\tTRUE";
				if (c->branchIfTrue)
					*ifElseFlowResults << " - Branching to " << c->branchIfTrue->GetPrintableName() << "\n";
				else
					*ifElseFlowResults << " - No Branch." << "\n";
				cout << ifElseFlowResults->str() << endl;
				return c;
			}
			else
				*ifElseFlowResults << "\tFALSE - No Branch." << "\n";
		}
	}
	return NULL; // If gets here, no branching
}




