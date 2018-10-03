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
 * Source: cfg.cc																*
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
#include "../../Headers/Models/cfg.h"
#include <fstream>

CFG::CFG()
{

}

CFG::~CFG()
{
	heads.clear();
	//tails.clear();
	//others.clear();

	// Delete the actual AssayNodes
	while (allDags.size() > 0)
	{
		DAG *d = allDags.front();
		allDags.erase(allDags.begin());
		delete d;
	}
	while (conditionalGroups.size() > 0)
	{
		ConditionalGroup *cg = conditionalGroups.back();
		conditionalGroups.pop_back();
		delete cg;
	}
}

///////////////////////////////////////////////////////////////
// Create a new DAG and add to the CFG; the returned DAG can
// be modified to specify the assay
///////////////////////////////////////////////////////////////
DAG * CFG::AddNewDAG(string name)
{
	DAG *d = new DAG();
	d->setName(name);
	allDags.push_back(d);
	return d;

}
DAG * CFG::AddNewDAG()
{
	return AddNewDAG("Unnamed Assay");
}

///////////////////////////////////////////////////////////////
// Adds existing DAG to the CFG; should be used primarily when
// importing/reading a CFG with DAGs from a file.
///////////////////////////////////////////////////////////////
DAG * CFG::AddExistingDAG(DAG *dag)
{
	allDags.push_back(dag);
	return dag;
}

///////////////////////////////////////////////////////////////
// Create a new CG and add to the CFG; the returned CG can
// be modified to specify the conditions.  Should point to an
// assay/DAG in the CFG
///////////////////////////////////////////////////////////////
ConditionalGroup * CFG::AddNewCG()
{
	ConditionalGroup *cg = new ConditionalGroup();
	conditionalGroups.push_back(cg);
	return cg;
}

///////////////////////////////////////////////////////////////
//Creates a graph of the Dag in .dot format
///////////////////////////////////////////////////////////////
void CFG::OutputGraphFile(string filename, bool outputDAGs, bool color, bool fullStats)
{
	ofstream out;
	string formattedFilename = "Output/" + filename + ".dot";

	out.open(formattedFilename.c_str());
	out<<"digraph G {\n";
	out << "node [shape=box, style=\"rounded, filled\"]\n";
	//out<<"  size=\"8.5,10.25\";\n";

	// Draw nodes
	for (int i = 0; i < allDags.size(); i++)
	{
		DAG *d = allDags.at(i);

		// Output DAG, if requested
		if (outputDAGs)
		{
			stringstream dagFileName;
			dagFileName << filename << "_" + d->GetPrintableName();
			d->OutputGraphFile(dagFileName.str(), color, fullStats);
		}


		out << d->getId() <<" [label = \"" << d->GetPrintableName();
		if (fullStats)
		{
			int numTransIn = 0;
			int numTransOut = 0;
			for (int j = 0; j < d->heads.size(); j++)
				if (d->heads.at(j)->GetType() == TRANSFER_IN)
					numTransIn++;
			for (int j = 0; j < d->tails.size(); j++)
				if (d->tails.at(j)->GetType() == TRANSFER_OUT)
					numTransOut++;

			out << "\\n(" << d->allNodes.size() << " nodes";
			if (numTransIn > 0)
				out << ",\\n" << numTransIn <<" drops in";
			if (numTransOut > 0)
				out << ",\\n" << numTransOut <<" drops out";
			out << ")\"";
		}
		else
			out<<"\"";

		if (color)
			out << ", fillcolor=gray90];\n";
		else
			out << ", fillcolor=white];\n";
	}
	// Draw edges
	for (int i = 0; i < conditionalGroups.size(); i++)
	{
		ConditionalGroup *cg = conditionalGroups.at(i);
		for (int j = 0; j < cg->getConditions()->size(); j++)
		{
			Condition *c = cg->getConditions()->at(j);
			list<DAG *>::iterator it = c->dependents.begin();
			for (; it != c->dependents.end(); it++)
			{
				if (c->branchIfTrue)
				{
					out << (*it)->getId() << " -> " << c->branchIfTrue->getId();

					// Library of colors:
					string color;
					int rem = 0;
					int nc = 12;//num colors
					if (i % nc == rem++)
						color = "0000FF";
					else if (i % nc == rem++)
						color = "22B14C";// "00FF00";
					else if (i % nc == rem++)
						color = "FF0000";
					else if (i % nc == rem++)
						color = "00FFFF";
					//else if (i % nc == rem++)
					//	color = "FFFF00";
					else if (i % nc == rem++)
						color = "FF00FF";
					else if (i % nc == rem++)
						color = "000088";
					else if (i % nc == rem++)
						color = "008800";
					else if (i % nc == rem++)
						color = "880000";
					else if (i % nc == rem++)
						color = "008888";
					else if (i % nc == rem++)
						color = "888800";
					else if (i % nc == rem++)
						color = "880088";
					else if (i % nc == rem++)
						color = "888888";
					else
						color = "888888";


					out << " [";
					if (fullStats)
					{
						if (j == 0) // "IF"
							out << "fontcolor=\"#" << color << "\", label=\"IF" << c->statement->printExpression(false) << "\", ";
						else if (j == cg->getConditions()->size()-1) // "ELSE"
							out << "fontcolor=\"#" << color << "\", label=\"ELSE //" << c->statement->printExpression(false) << "\", ";
						else // "ELSE-IF
							out << "fontcolor=\"#" << color << "\", label=\"ELSE_IF" << c->statement->printExpression(false) << "\", ";
					}

					out << "color=\"#" << color << "\"]";
					out << ";\n";
				}
			}
		}
	}
	out<<"}\n";

	// Close file
	out.close();
}

///////////////////////////////////////////////////////////////
// Examines the ConditionalGroups and adds all the children
// to the DAGs in the CFG.  Also determines which DAGs are
// heads/tails for later processing.
///////////////////////////////////////////////////////////////
void CFG::ConstructAndValidateCFG()
{
	// Must find headers, assume all DAGs are headers...then remove some as we look at conditions
	for (int i = 0; i < allDags.size(); i++)
	{
		heads.push_back(allDags.at(i));
		allDags.at(i)->conditionalGroups.clear(); // Reset the conditional groups
	}

	// For each conditional groups...
	vector<ConditionalGroup *>::iterator it = conditionalGroups.begin();
	for (; it != conditionalGroups.end(); it++)
	{
		ConditionalGroup *cg = *it;
		//cg->evaluateAndDbPrint();//dtg
		for (int i = 0; i < cg->getConditions()->size(); i++)
		{	// Add dependencies for each condition
			Condition *c = cg->getConditions()->at(i);

			// If transfer droplets, add dependencies
			for (int j = 0; j < c->transfers.size(); j++) // DTG
			{
				c->transfers.at(j)->transOut->GetDAG()->AddUniqueCG(cg);
				c->dependents.remove(c->transfers.at(j)->transOut->GetDAG());
				c->dependents.push_back(c->transfers.at(j)->transOut->GetDAG());
				//cout << "Just added TO " << c->transfers.at(j)->transOut->GetDAG()->getName() << endl;
				//cout << "Didn't add TI " << c->transfers.at(j)->transIn->GetDAG()->getName() << endl;
			}

			// Check if DAG a is branching to DAG b, then DAG b
			// is not a header DAG (unless a == b, in which case
			// b may still be a header DAG)
			bool mayBeHeaderDag = false;
			for (DAG * d : c->dependents)
				if (d == c->branchIfTrue)
					mayBeHeaderDag = true;
			if (!mayBeHeaderDag)
				heads.remove(c->branchIfTrue);

			if (i == cg->getConditions()->size()-1 && cg->getConditions()->size() > 1) // ELSE statement
			{	// The dependents of an ELSE statement are the unique dependents of the related IF/IF-ELSE statements
				for (int j = 0; j < cg->getConditions()->size()-1; j++)
				{	// For each previous condition in the group
					Condition *pc = cg->getConditions()->at(j);
					list<DAG *>::iterator it2 = pc->dependents.begin();
					for (; it2 != pc->dependents.end(); it2++)
					{	// For each dependency in the previous condition, add if new
						c->dependents.remove(*it2);
						c->dependents.push_back(*it2);
					}
				}
			}
			else // IF or IF-ELSE statement
			{
				c->statement->getParentDags(&c->dependents); // Add dependents to condition

				// Add the CG to the dependent DAGs if not already added
				list<DAG *>::iterator it3 = c->dependents.begin();
				for (; it3 != c->dependents.end(); it3++)
					(*it3)->AddUniqueCG(cg);
			}
		}
	}

	// Debug print
	/*for (it = conditionalGroups.begin(); it != conditionalGroups.end(); it++)
	{
		ConditionalGroup *cg = *it;
		cg->printConditionalGroup();

		//cg->evaluateAndDbPrint();//dtg
		for (int i = 0; i < cg->getConditions()->size(); i++)
		{	// Add dependencies for each condition
			Condition *c = cg->getConditions()->at(i);
			cout << c->branchIfTrue->getName() << "-- depends on -->";

			list<DAG *>::iterator itDep = c->dependents.begin();
			for (; itDep != c->dependents.end(); itDep++)
				cout << (*itDep)->getName() << ", ";
		}
	}*/
}

////////////////////////////////////////////////////////////////////
// Looks through all CFGs/DAGs and converts any fault-tolerant splits
// into normal splits, adding any necessary new DAGs and dependencies
////////////////////////////////////////////////////////////////////
void CFG::convertFTSplits()
{
	{	// Sanity check: Can only convert if a single DAG
		stringstream msg;
		msg << "ERROR. For now, we can only convert FTSplits if they're contained within a single independent DAG. " << endl;
		//for (int i = 0; i < cfgs->size(); i++)
		claim(allDags.size() == 1, &msg);
	}

	DAG *od = allDags.front(); //original dag

	// Output original DAG
	OutputGraphFile("BeforeSplitCFG", true, true, true);
	//od->OutputGraphFile("beforeDAG", true, true);
	//OutputGraphFile("beforeCFG", true, true);

	// Get all the ftsplits in the original DAG
	list<AssayNode *> ftsplits;
	for (int j = 0; j < od->splits.size(); j++)
		if (od->splits.at(j)->GetType() == FTSPLIT)
			ftsplits.push_back(od->splits.at(j));

	int numTrans = 1;
	int numSplits = 0;
	while (!ftsplits.empty())
	{
		numSplits++;
		AssayNode *fts = ftsplits.front();
		vector<AssayNode *> splitChildTIs;
		vector<AssayNode *> pdDdTrans; // parent-dag to detect-dag transfers
		vector<AssayNode *> ddMrsTrans; // detect-dag to merge/re-split-dag transfers
		vector<AssayNode *> mrsDdTrans; // merge/re-split-dag to detect-dag transfers
		vector<AssayNode *> ddCdTrans; // detect-dag to child-dag transfers
		ftsplits.pop_front();

		// First, make the detect DAG to insert
		stringstream ss;
		ss << "detect" << numSplits ;
		DAG * dd = AddNewDAG(ss.str());
		AssayNode *Ddd = dd->AddDetectNode(2, .1, "D_D");
		for (int j = 0; j < fts->numDrops; j++)
		{
			ss.str("");
			ss << "D_TO_" << j+1;
			AssayNode *TO = dd->AddTransOutNode(ss.str());
			ss.str("");
			ss << "D_TI_" << j+1;
			AssayNode *TI = dd->AddTransInNode(ss.str());
			dd->ParentChild(TI, Ddd);
			dd->ParentChild(Ddd, TO);
		}

		// Now, make the merge-reSplit DAG to insert
		ss.str("");
		ss << "mergeResplit" << numSplits;
		DAG * mrs = AddNewDAG(ss.str());
		AssayNode *Mmrs = mrs->AddMixNode(2, .1, "MRS_M");
		AssayNode *Smrs = mrs->AddSplitNode(false, 2, .1, "MRS_S");
		mrs->ParentChild(Mmrs, Smrs);
		for (int j = 0; j < fts->numDrops; j++)
		{
			ss.str("");
			ss << "MRS_TO_" << j+1;
			AssayNode *TO = mrs->AddTransOutNode(ss.str());
			ss.str("");
			ss << "MRS_TI_" << j+1;
			AssayNode *TI = mrs->AddTransInNode(ss.str());
			dd->ParentChild(TI, Mmrs);
			dd->ParentChild(Smrs, TO);
		}

		// The original DAG will now become the child DAG; a new DAG will be created for the parent DAG
		DAG *cd = od;
		ss.str("");
		ss << "parent" << numSplits;
		DAG *pd = AddNewDAG(ss.str());
		separateFTSplitParents(cd, pd, fts); // Put all parents of fts into a new parent dag

		// Add transfer nodes into original dag and parent dag (to bridge the split dags)
		list<AssayNode *> inserts;
		for (int j = 0; j < pd->allNodes.size(); j++)
		{
			AssayNode *par = pd->allNodes.at(j);
			for (int k = 0; k < par->children.size(); k++)
			{
				AssayNode *child = par->children.at(k);
				if (par->GetDAG() != child->GetDAG())
				{
					ss.str("");	ss << "TO_" << numTrans;
					string toName = ss.str();
					ss.str("");	ss << "TI_" << numTrans;
					string tiName = ss.str();
					AssayNode *TO = par->GetDAG()->AddTransOutNode(toName);
					AssayNode *TI = child->GetDAG()->AddTransInNode(tiName);

					inserts.push_back(par);
					inserts.push_back(child);
					inserts.push_back(TO);
					inserts.push_back(TI);

					if (par == fts)
					{	// dd->cd
						//splitChildTIs.push_back(TI);
						ddCdTrans.push_back(dd->tails.at(k));
						ddCdTrans.push_back(TI);
					}
					else
					{
						// Add pass-throughs for the dd and mrs DAGs
						AssayNode *ddOut = dd->AddTransOutNode(toName);
						AssayNode *ddIn = dd->AddTransInNode(tiName);
						AssayNode *mrsOut = mrs->AddTransOutNode(toName);
						AssayNode *mrsIn = mrs->AddTransInNode(tiName);
						ddIn->children.push_back(ddOut);
						ddOut->parents.push_back(ddIn);
						mrsIn->children.push_back(mrsOut);
						mrsOut->parents.push_back(mrsIn);

						pdDdTrans.push_back(TO);
						pdDdTrans.push_back(ddIn);
						ddMrsTrans.push_back(ddOut);
						ddMrsTrans.push_back(mrsIn);
						mrsDdTrans.push_back(mrsOut);
						mrsDdTrans.push_back(ddIn);
						ddCdTrans.push_back(ddOut);
						ddCdTrans.push_back(TI);
						// Maybe add child/parent relationships here

					}
					numTrans++;
				}
			}
		}
		while (!inserts.empty())
		{
			AssayNode *par = inserts.front();
			inserts.pop_front();
			AssayNode *child = inserts.front();
			inserts.pop_front();
			AssayNode *TO = inserts.front();
			inserts.pop_front();
			AssayNode *TI = inserts.front();
			inserts.pop_front();
			par->GetDAG()->InsertNode(par, child, TO);
			par->GetDAG()->InsertNode(TO, child, TI);
			TO->children.clear();
			TI->parents.clear();
		}

		// Add future transfer edges to transfer lists
		for (int j = 0; j < fts->children.size(); j++)
		{
			pdDdTrans.push_back(fts->children.at(j));
			pdDdTrans.push_back(dd->heads.at(j));
			ddMrsTrans.push_back(dd->tails.at(j));
			ddMrsTrans.push_back(mrs->heads.at(j));
			mrsDdTrans.push_back(mrs->tails.at(j));
			mrsDdTrans.push_back(dd->heads.at(j));
		}

		// Now, add conditional groups
		vector<TransferEdge *> transfers;
		TransferEdge *te = NULL;
		ConditionalGroup *cg = NULL;
		Expression *e = NULL;
		// Par->dd
		cg = AddNewCG();
		e = new Expression(pd, true);
		for (int j = 0; j < pdDdTrans.size(); j = j+2)
		{
			te = new TransferEdge();
			te->transOut = pdDdTrans.at(j);
			te->transIn = pdDdTrans.at(j+1);
			transfers.push_back(te);
			//cout << te->transOut->GetName() << "-->" << te->transIn->GetName() << endl; // DTG Debug
		}
		cg->addNewCondition(e, dd, transfers);

		// dd->mrs
		cg = AddNewCG();
		transfers.clear();
		e = new Expression(dd->detects.front(), OP_LT, 0.5);
		for (int j = 0; j < ddMrsTrans.size(); j = j+2)
		{
			te = new TransferEdge();
			te->transOut = ddMrsTrans.at(j);
			te->transIn = ddMrsTrans.at(j+1);
			transfers.push_back(te);
			//cout << te->transOut->GetName() << "-->" << te->transIn->GetName() << endl; // DTG Debug
		}
		cg->addNewCondition(e, mrs, transfers);
		// dd->cd (child dag)
		transfers.clear();
		for (int j = 0; j < ddCdTrans.size(); j = j+2)
		{
			te = new TransferEdge();
			te->transOut = ddCdTrans.at(j);
			te->transIn = ddCdTrans.at(j+1);
			transfers.push_back(te);
			//cout << te->transOut->GetName() << "-->" << te->transIn->GetName() << endl; // DTG Debug
		}
		cg->addNewCondition(NULL, od, transfers);

		// mrs->dd
		cg = AddNewCG();
		transfers.clear();
		e = new Expression(mrs, true);
		for (int j = 0; j < mrsDdTrans.size(); j = j+2)
		{
			te = new TransferEdge();
			te->transOut = mrsDdTrans.at(j);
			te->transIn = mrsDdTrans.at(j+1);
			transfers.push_back(te);
			//cout << te->transOut->GetName() << "-->" << te->transIn->GetName() << endl; // DTG Debug
		}
		cg->addNewCondition(e, dd, transfers);
	}


	// Output refactored DAGs
	for (int j = 0; j < allDags.size(); j++)
	{
		stringstream name;
		name << j << "_after_" << allDags.at(j)->GetPrintableName();
		allDags.at(j)->OutputGraphFile(name.str(), true, true);
	}

	ConstructAndValidateCFG();
	OutputGraphFile("AfterSplitCFG", true, true, true);
	cout << "Fault-tolerant splits successfully converted. Please see output folder for before/after DAG/CFG results." << endl;
}

////////////////////////////////////////////////////////////////////
// Helper method for convertFTSplits()
////////////////////////////////////////////////////////////////////
void CFG::separateFTSplitParents(DAG *cd, DAG *pd, AssayNode *split)
{
	split->type = SPLIT; // Convert to normal split
	recursiveSeparate(cd, pd, split);

}

////////////////////////////////////////////////////////////////////
// Helper method for convertFTSplits()
////////////////////////////////////////////////////////////////////
void CFG::recursiveSeparate(DAG *cd, DAG *pd, AssayNode *n)
{
	// Add to parent DAG
	pd->AddNodeToDAG(n);
	cd->RemoveNodeFromDAG(n);
	for (int i = 0; i < n->parents.size(); i++)
		recursiveSeparate(cd, pd, n->parents.at(i));
}






















