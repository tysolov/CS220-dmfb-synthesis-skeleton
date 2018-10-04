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
 * Source: expression.cc														*
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
#include "expression.h"

int Expression::next_id = 1;

/////////////////////////////////////////////////////////////////////
// Constructors
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Creating an expression that will be manually created externally.
// This method should only be called by the FileIn class when reading
// in a CFG from a file.
/////////////////////////////////////////////////////////////////////
Expression::Expression(int expId)
{
	// Set ID and increase next_id if necessary
	id = expId;
	if (id >= next_id)
		next_id = id + 1;

	//operationType = ot;
	operands = NULL;
	//operandType = OP_TWO_SENSORS;
	constant = 0;
	sensor1 = NULL;
	sensor2 = NULL;
	unconditionalParent = NULL;
}
/////////////////////////////////////////////////////////////////////
// Creating an expression that compares the readings from two sensors.
/////////////////////////////////////////////////////////////////////
Expression::Expression(AssayNode *s1, ExOperationType ot, AssayNode *s2)
{
	{	// Sanity check: Must be proper operation type
		if (!(ot == OP_GT || ot == OP_LT || ot == OP_GoE || ot == OP_LoE || ot == OP_EQUAL) || !s1 || !s2)
		{
			stringstream msg;
			msg << "ERROR. >, <, <=, >=, == operations allowed for a sensor-sensor comparison. Must be valid sensors." << ends;
			claim(false, &msg);
		}
	}
	id = next_id++;
	operationType = ot;
	operands = NULL;
	operandType = OP_TWO_SENSORS;
	constant = 0;
	sensor1 = s1;
	sensor2 = s2;
	unconditionalParent = NULL;
}
/////////////////////////////////////////////////////////////////////
// Creating an expression that compares the reading from a sensor to
// a constant value
/////////////////////////////////////////////////////////////////////
Expression::Expression(AssayNode *s1, ExOperationType ot, double c)
{
	{	// Sanity check: Must be proper operation type
		if (!(ot == OP_GT || ot == OP_LT || ot == OP_GoE || ot == OP_LoE || ot == OP_EQUAL)	|| !s1)
		{
			stringstream msg;
			msg << "ERROR. >, <, <=, >=, == operations allowed for a sensor-sensor comparison. Must be valid sensors." << ends;
			claim(false, &msg);
		}
	}
	id = next_id++;
	operationType = ot;
	operands = NULL;
	operandType = OP_ONE_SENSOR;
	constant = c;
	sensor1 = s1;
	sensor2 = NULL;
	unconditionalParent = NULL;
}

/////////////////////////////////////////////////////////////////////
// Creating an BioExpression that compares the run count of a repeatable
// assay (eventually, a DAG) to a static runcount (constant runCount)
/////////////////////////////////////////////////////////////////////
Expression::Expression(DAG *repeatableDag, ExOperationType ot, double runCount)
{
	{	// Sanity check: Must be proper operation type
		if (!(ot == OP_GT || ot == OP_LT || ot == OP_GoE || ot == OP_LoE || ot == OP_EQUAL)	|| !repeatableDag)
		{
			stringstream msg;
			msg << "ERROR. >, <, <=, >=, == operations allowed for a sensor-sensor comparison. Must be valid assay/dag being checked for repetition." << ends;
			claim(false, &msg);
		}
	}
	id = next_id++;
	operationType = ot;
	operands = NULL;
	operandType = OP_RUN_COUNT;
	constant = runCount;
	sensor1 = NULL;
	sensor2 = NULL;
	unconditionalParent = repeatableDag;
}

/////////////////////////////////////////////////////////////////////
// Creating an expression that performs a NOT operation
/////////////////////////////////////////////////////////////////////
Expression::Expression(Expression *notExp)
{
	id = next_id++;
	operationType = OP_NOT;
	operands = new vector<Expression*>();
	operands->push_back(notExp);
	operandType = OP_SUB_EXP;
	constant = 0;
	sensor1 = NULL;
	sensor2 = NULL;
	unconditionalParent = NULL;
}

/////////////////////////////////////////////////////////////////////
// Creating an expression that performs an AND or OR operation
/////////////////////////////////////////////////////////////////////
Expression::Expression(ExOperationType andOr)
{
	{	// Sanity check: Must be proper operation type
		stringstream msg;
		msg << "ERROR. Only AND, OR operations allowed for this expression." << ends;
		claim(andOr == OP_AND || andOr == OP_OR, &msg);
	}
	id = next_id++;
	operationType = andOr;
	operands = new vector<Expression*>();
	operandType = OP_SUB_EXP;
	constant = 0;
	sensor1 = NULL;
	sensor2 = NULL;
	unconditionalParent = NULL;
}

/////////////////////////////////////////////////////////////////////
// Creating an expression that is either unconditionally true or false.
// Must pass the unconditional parent from which to branch from
/////////////////////////////////////////////////////////////////////
Expression::Expression(DAG *unconPar, bool unconditional)
{
	id = next_id++;
	if (unconditional)
		operandType = OP_TRUE;
	else
		operandType = OP_FALSE;
	//operands = new vector<Expression*>();
	operands = NULL;
	operationType = OP_UNCOND;
	constant = 0;
	sensor1 = NULL;
	sensor2 = NULL;
	unconditionalParent = unconPar;
}
/////////////////////////////////////////////////////////////////////
// Destructor.
/////////////////////////////////////////////////////////////////////
Expression::~Expression()
{
	if (operands)
	{
		// TODO: Traverse and do recursive delete...MAYBE
		//for (int i = 0; i < operands->size(); i++)
		//recursiveDelete();

		operands->clear();
		delete operands;
	}
	if (sensor1)
		sensor1 = NULL;
	if (sensor2)
		sensor2 = NULL;
}
/////////////////////////////////////////////////////////////////////
// Methods
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Add an operand to an AND/OR statement.
/////////////////////////////////////////////////////////////////////
void Expression::addOperand(Expression *op)
{
	{	// Sanity check: Must be AND/OR to add operand
		stringstream msg;
		msg << "ERROR. Only AND, OR operations allowed to add more operands." << ends;
		claim(operationType == OP_AND || operationType == OP_OR, &msg);
	}
	{	// Sanity check: Expression must not be NULL
		stringstream msg;
		msg << "ERROR. Expression is not valid." << ends;
		claim(op, &msg);
	}
	operands->push_back(op);
}

/////////////////////////////////////////////////////////////////////
// Evaluates if the expression is valid.  That is, if all the leaves
// actually evaluate to a true or false (the leaves all compare
// sensor/constant values).
/////////////////////////////////////////////////////////////////////
bool Expression::isValidExpression()
{
	return recursiveValidate(this);
}
bool Expression::recursiveValidate(Expression *e)
{
	if (!e)
		return false;
	if (e->operationType == OP_GT || e->operationType == OP_LT
			|| e->operationType == OP_GoE || e->operationType == OP_LoE
			|| e->operationType == OP_EQUAL	|| e->operationType == OP_UNCOND)
		return true;
	else if (e->operationType == OP_AND && e->operationType == OP_OR && e->operands->size() <= 1)
		return false;
	else if (e->operands->size() == 0) // e->operationType == OP_NOT
		return false;

	bool isValid = true;
	for (int i = 0; i < e->operands->size(); i++)
	{
		isValid = isValid && recursiveValidate(e->operands->at(i));
		if (!isValid)
			return false;
	}
	return true;
}
/////////////////////////////////////////////////////////////////////
// Prints the boolean expression. If printLiveValues is set, prints
// live values of the sensors, run-count, etc.; if not, then just
// prints the inequality w/o live values.
/////////////////////////////////////////////////////////////////////
string Expression::printExpression(bool printLiveValues)
{
	stringstream ss;
	recursivePrint(this, &ss, printLiveValues);
	return ss.str();
}
void Expression::recursivePrint(Expression *e, stringstream *ss, bool printLiveValues)
{
	if (!e)
	{
		*ss << "(No Condition)";
		return;
	}

	*ss << "(";
	if (e->operationType == OP_UNCOND)
	{
		if (e->operandType == OP_TRUE)
			*ss << "Unconditional TRUE";
		else if (e->operandType == OP_FALSE)
			*ss << "Unconditional FALSE";
	}
	else if (e->operationType == OP_GT || e->operationType == OP_LT
			|| e->operationType == OP_GoE || e->operationType == OP_LoE || e->operationType == OP_EQUAL)
	{
		// Print out run-count or sensor-reading
		if (printLiveValues)
		{
			if (e->operandType == OP_RUN_COUNT)
				*ss << e->unconditionalParent->GetIdName() << "_RUN_COUNT = " << e->unconditionalParent->getRunCount();
			else // 1- or 2-Sensor reading
				*ss << e->sensor1->GetName() << "_READ = " << e->sensor1->GetReading();
		}
		else
		{
			if (e->operandType == OP_RUN_COUNT)
				*ss << e->unconditionalParent->GetIdName() << "_RUN_COUNT";
			else // 1- or 2-Sensor reading
				*ss << e->sensor1->GetName() << "_READ";
		}

		if (e->operationType == OP_GT)
			*ss << " > ";
		else if (e->operationType == OP_LT)
			*ss << " < ";
		else if (e->operationType == OP_GoE)
			*ss << " >= ";
		else if (e->operationType == OP_LoE)
			*ss << " <= ";
		else if (e->operationType == OP_EQUAL)
			*ss << " = ";
		else
			*ss << " ??? ";

		if (e->operandType == OP_ONE_SENSOR || e->operandType == OP_RUN_COUNT)
			*ss << e->constant;
		else if (e->operandType == OP_TWO_SENSORS)
		{
			if (printLiveValues)
				*ss << e->sensor2->GetName() << "_READ = " << e->sensor2->GetReading();
			else
				*ss << e->sensor2->GetName() << "_READ";
		}
		else
			claim(false, "Unsupported operand type: Expression::recursivePrint()");
	}
	else if (e->operationType == OP_AND || e->operationType == OP_OR)
	{
		for (int i = 0; i < e->operands->size(); i++)
		{
			recursivePrint(e->operands->at(i), ss, printLiveValues);
			if (i < e->operands->size()-1 && e->operationType == OP_AND)
				*ss << " AND ";
			else if (i < e->operands->size()-1 && e->operationType == OP_OR)
				*ss << " OR ";
		}

	}
	else if (e->operationType == OP_NOT)
	{
		*ss << " NOT";
		recursivePrint(e->operands->front(), ss, printLiveValues);
	}
	else
		claim(false, "Unsupported operation/operand type: Expression::recursivePrint().");
	*ss << ")";
}

/////////////////////////////////////////////////////////////////////
// Evaluates the expression and returns the value.
/////////////////////////////////////////////////////////////////////
bool Expression::evaluateExpression()
{
	return recursiveEvaluate(this);
}
bool Expression::recursiveEvaluate(Expression *e)
{
	{	// Sanity check: Expression must be valid
		stringstream msg;
		msg << "ERROR. Expression not valid." << ends;
		claim(e->isValidExpression(), &msg);
	}

	if (e->operationType == OP_UNCOND)
	{
		if (e->operandType == OP_TRUE)
			return true;
		else if (e->operandType == OP_FALSE)
			return false;
	}
	else if (e->operationType == OP_GT || e->operationType == OP_LT
			|| e->operationType == OP_GoE || e->operationType == OP_LoE || e->operationType == OP_EQUAL)
	{
		{	// Sanity check: Detect nodes must be done
			if (e->sensor1)
			{
				stringstream msg;
				msg << "ERROR. Detect sensor " << e->sensor1->GetName() << " status must be 'complete'." << endl;
				claim(e->sensor1->GetStatus() == COMPLETE , &msg);
			}

			if (e->sensor2)
			{
				stringstream msg;
				msg << "ERROR. Detect sensor " << e->sensor2->GetName() << " status must be 'complete'." << endl;
				claim(e->sensor2->GetStatus() == COMPLETE , &msg);
			}
		}

		double lhs;
		double rhs;
		if (e->operandType == OP_ONE_SENSOR)
		{
			lhs = e->sensor1->GetReading();
			rhs = e->constant;
		}
		else if (e->operandType == OP_TWO_SENSORS)
		{
			lhs = e->sensor1->GetReading();
			rhs = e->sensor2->GetReading();
		}
		else if (e->operandType == OP_RUN_COUNT)
		{
			lhs = e->unconditionalParent->getRunCount();
			rhs = e->constant;
		}
		else
			claim(false, "Unsupported operand type.");

		if (e->operationType == OP_GT)
			return lhs > rhs;
		else if (e->operationType == OP_LT)
			return lhs < rhs;
		else if (e->operationType == OP_GoE)
			return lhs >= rhs;
		else if (e->operationType == OP_LoE)
			return lhs <= rhs;
		else if (e->operationType == OP_EQUAL)
			return lhs == rhs;
	}
	else if (e->operationType == OP_AND)
	{
		bool eval = true;
		for (int i = 0; i < e->operands->size(); i++)
			eval = eval && recursiveEvaluate(e->operands->at(i));
		return eval;
	}
	else if (e->operationType == OP_OR)
	{
		bool eval = false;
		for (int i = 0; i < e->operands->size(); i++)
			eval = eval || recursiveEvaluate(e->operands->at(i));
		return eval;
	}
	else if (e->operationType == OP_NOT)
	{
		return !(recursiveEvaluate(e->operands->front()));
	}
	else
	{	// Sanity check: Detect nodes must be done
		stringstream msg;
		msg << "ERROR. Unknown operation type" << ends;
		claim(false , &msg);
	}
    stringstream msg;
    msg << "ERROR. Reached end of non-void function" << ends;
    cout << msg.str() << endl;
    exit(-1);
}

/////////////////////////////////////////////////////////////////////
// Gets the unique DAG parents for this expression
/////////////////////////////////////////////////////////////////////
void Expression::getParentDags(list<DAG *> *parents)
{
	//parents->clear();
	recursiveGetParents(this, parents);
}
void Expression::recursiveGetParents(Expression *e, list<DAG *> *parents)
{
	if (e->operationType == OP_GT || e->operationType == OP_LT
			|| e->operationType == OP_GoE || e->operationType == OP_LoE || e->operationType == OP_EQUAL)
	{
		if (e->operandType == OP_RUN_COUNT)
		{
			parents->remove(e->unconditionalParent);
			parents->push_back(e->unconditionalParent);
		}
		else if (e->operandType == OP_ONE_SENSOR || e->operandType == OP_TWO_SENSORS)
		{
			parents->remove(e->sensor1->GetDAG());
			parents->push_back(e->sensor1->GetDAG());

			if (e->operandType == OP_TWO_SENSORS)
			{
				parents->remove(e->sensor2->GetDAG());
				parents->push_back(e->sensor2->GetDAG());
			}
		}
		else
			claim(false, "Unsupported operandType in Expression:recursiveGetParents().");
	}
	else if (e->operationType != OP_UNCOND)
	{
		for (int i = 0; i < e->operands->size(); i++)
			recursiveGetParents(e->operands->at(i), parents);
	}
	else	// OP_UNCOND
	{
		parents->remove(e->unconditionalParent);
		parents->push_back(e->unconditionalParent);
	}
}












