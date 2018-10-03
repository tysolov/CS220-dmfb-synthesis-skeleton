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
 * Name: Expression																*
 *																				*
 * Code Developed and Described in:												*
 * Authors: Dan Grissom, Chris Curtis & Philip Brisk							*
 * Title: Interpreting Assays with Control Flow on Digital Microfluidic			*
 * 			Biochips															*
 * Publication Details: In ACM JETC in Computing Systems, Vol. 10, No. 3,		*
 * 					    Apr 2014, Article No. 24								*
 * 					    														*
 * Details: A logical structure used to represent conditional boolean			*
 * expressions.																	*
 *-----------------------------------------------------------------------------*/
#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include "assay_node.h"
#include "../Resources/enums.h"
#include "../Testing/claim.h"
#include "dag.h"
#include "entity.h"

class DAG;
class AssayNode;

/////////////////////////////////////////////////////////////////
// Expression: Node Structure for holding an expression/value/
// operand, as well as the operation type and children.  Together,
// forms a tree which represents an entire boolean expression
/////////////////////////////////////////////////////////////////
class Expression : public Entity
{
    protected:
        // Variables
		ExOperationType operationType;
		vector<Expression*> *operands;
		ExOperandType operandType;
		double constant;
		AssayNode *sensor1;
		AssayNode *sensor2;
		DAG *unconditionalParent;
		static int next_id;

		// Methods
		static bool recursiveValidate(Expression *e);
		static void recursivePrint(Expression *e, stringstream *ss, bool printLiveValues);
		static bool recursiveEvaluate(Expression *e);
		static void recursiveGetParents(Expression *e, list<DAG *> *parents);


    public:
        // Constructors
		//Expression();
		Expression(AssayNode *s1, ExOperationType ot, AssayNode *s2); // Two-sensor constructor
		Expression(AssayNode *s1, ExOperationType ot, double c); // One-sensor constructor
		Expression(DAG *repeatableDag, ExOperationType ot, double runCount); // Run-count constructor
		Expression(Expression *notExp); // NOT constructor
		Expression(ExOperationType andOr); // AND/OR constructor
		Expression(DAG *unconPar, bool unconditional); // Either unconditionally true or false
        ~Expression();

        // Getters/Setters
        vector<Expression*> * getOperands() { return operands; }

        // Methods
        void addOperand(Expression *op);
        bool isValidExpression();
        string printExpression(bool printLiveValues);
        bool evaluateExpression();
        void getParentDags(list<DAG *> *parents);
    private:
        Expression(int expId);

	// Friend classes
	friend class FileOut;
	friend class FileIn;

 };
#endif
