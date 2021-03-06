/*
 * $Revision: $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2010-01-25 21:14:02 +1100 (Mon, 25 Jan 2010) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration of the master class for the Branch&Cut algorithm
 * for the Maximum C-Planar SubGraph problem.
 * 
 * This class is managing the optimization.
 * Variables and initial constraints are generated and pools are initialized.
 * 
 * \author Mathias Jansen
 * 
 * 
 * \par License:
 * This file is part of the Open Graph Drawing Framework (OGDF).
 * Copyright (C) 2005-2007
 * 
 * \par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 or 3 as published by the Free Software Foundation
 * and appearing in the files LICENSE_GPL_v2.txt and
 * LICENSE_GPL_v3.txt included in the packaging of this file.
 *
 * \par
 * In addition, as a special exception, you have permission to link
 * this software with the libraries of the COIN-OR Osi project
 * (http://www.coin-or.org/projects/Osi.xml), all libraries required
 * by Osi, and all LP-solver libraries directly supported by the
 * COIN-OR Osi project, and distribute executables, as long as
 * you follow the requirements of the GNU General Public License
 * in regard to all of the software in the executable aside from these
 * third-party libraries.
 * 
 * \par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * \par
 * You should have received a copy of the GNU General Public 
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 * 
 * \see  http://www.gnu.org/copyleft/gpl.html
 ***************************************************************/

#ifndef OGDF_MAX_CPLANAR_MASTER_H
#define OGDF_MAX_CPLANAR_MASTER_H

//#include <ogdf/timer.h>
#include <ogdf/cluster/ClusterGraph.h>
#include <ogdf/cluster/ClusterGraphAttributes.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/GraphCopy.h>
#include <ogdf/internal/cluster/MaxCPlanar_Edge.h>
#include <ogdf/basic/Logger.h>
#include <ogdf/basic/ArrayBuffer.h>

#include <abacus/master.h>
#include <abacus/string.h>

namespace ogdf {

class ChunkConnection; 

//! Struct for storing the two corresponding nodes of an edge.
struct nodePair {
	node v1;
	node v2;
	nodePair() {}
	nodePair(node u1, node u2) : v1(u1), v2(u2) {}
	void printMe(ostream& out) const { out << "("<<v1<<","<<v2<<")"; }
};
std::ostream &operator<<(std::ostream &os, const nodePair& v);


//! Struct for attaching the current lp-value to the corresponding edge.
//! Used in the primal heuristic.
struct edgeValue {
	node src;
	node trg;
	double lpValue;
	bool original;
	edge e;
};

//! Basic constraint type
class BaseConstraint : public ABA_CONSTRAINT {
	
public:
	BaseConstraint(ABA_MASTER *master, const ABA_SUB *sub, ABA_CSENSE::SENSE sense, double rhs, bool dynamic, bool local, bool liftable) :
		ABA_CONSTRAINT(master, sub, sense, rhs, dynamic, local, liftable) {}
	
	virtual ~BaseConstraint() {};
	
	virtual int coeff(const nodePair& n) = 0;
};


class Master : public ABA_MASTER {
		
	friend class Sub;
	
	// Pointers to the given Clustergraph and underlying Graph are stored.
	const ClusterGraph *m_C;
	const Graph *m_G;
		
	
	// Each time the primal bound is improved, the integer solution induced Graph is built.
	// \a m_solutionGraph is a pointer to the currently best solution induced Graph.
	GraphCopy *m_solutionGraph;
	
	List<nodePair> m_allOneEdges;		  //<! Contains all nodePairs whose variable is set to 1.0
	List<nodePair> m_originalOneEdges;	  //<! Contains original nodePairs whose variable is set to 1.0
	List<nodePair> m_connectionOneEdges;  //<! Contains connection nodePairs whose variable is set to 1.0
	List<edge> m_deletedOriginalEdges;	  //<! Contains original edges whose variable is set to 0.0
	
public:
	
	// Construction and default values
	Master(
		const ClusterGraph &C,
		int heuristicLevel=1,
		int heuristicRuns=2,
		double heuristicOEdgeBound=0.3,
		int heuristicNPermLists=5,
		int kuratowskiIterations=3,
		int subdivisions=10,
		int kSupportGraphs=3,
		double kuratowskiHigh=0.7,
		double kuratowskiLow=0.3,
		bool perturbation=false,
		double branchingGap=0.4,
		const char *time="00:20:00", //maximum computation time
		bool dopricing = true,
		bool checkCPlanar = false,   //just check c-planarity
		bool checkCPlanar2 = false,
		int numAddVariables = 15,
		double strongConstraintViolation = 0.3,
		double strongVariableViolation = 0.3,
		ABA_MASTER::OUTLEVEL ol=Silent);
		
	// Destruction
	virtual ~Master();
		
	// Initialisation of the first Subproblem
	virtual ABA_SUB *firstSub();
	
	// Returns the objective function coefficient of C-edges
	double epsilon() const {return m_epsilon;}
	
	// Returns the number of variables
	int nMaxVars() const {return m_nMaxVars;}
		
	// Returns a pointer to the underlying Graph
	const Graph *getGraph() const {return m_G;}
		
	// Returns a pointer to the given Clustergraph.
	const ClusterGraph *getClusterGraph() const {return m_C;}
		
	// Updates the "best" Subgraph \a m_solutionGraph found so far and fills edge lists with
	// corresponding edges (nodePairs). 
	void updateBestSubGraph(List<nodePair> &original, List<nodePair> &connection, List<edge> &deleted);
	
	// Returns the optimal solution induced Clustergraph
	Graph *solutionInducedGraph() {return (Graph*)m_solutionGraph;}
	
	// Returns nodePairs of original, connection, deleted or all optimal solution edges in list \a edges.
	void getAllOptimalSolutionEdges(List<nodePair> &edges) const;
	void getOriginalOptimalSolutionEdges(List<nodePair> &edges) const;
	void getConnectionOptimalSolutionEdges(List<nodePair> &egdes) const;
	void getDeletedEdges(List<edge> &edges) const;
		
	// Get parameters
	const int getKIterations() const {return m_nKuratowskiIterations;}
	const int getNSubdivisions() const {return m_nSubdivisions;}
	const int getNKuratowskiSupportGraphs() const {return m_nKuratowskiSupportGraphs;}
	const int getHeuristicLevel() const {return m_heuristicLevel;}
	const int getHeuristicRuns() const {return m_nHeuristicRuns;}
	const double getKBoundHigh() const {return m_kuratowskiBoundHigh;}
	const double getKBoundLow() const {return m_kuratowskiBoundLow;}
	const bool perturbation() const {return m_usePerturbation;}
	const double branchingOEdgeSelectGap() const {return m_branchingGap;}
	const double getHeuristicFractionalBound() const {return m_heuristicFractionalBound;}
	const int numberOfHeuristicPermutationLists() const {return m_nHeuristicPermutationLists;}
	const bool getMPHeuristic() const {return m_mpHeuristic;}
	const bool getCheckCPlanar() const {return m_checkCPlanar;}
	const bool getCheckCPlanar2() const {return m_checkCPlanar2;}
	const int getNumAddVariables() const {return m_numAddVariables;}
	const double getStrongConstraintViolation() const {return m_strongConstraintViolation;}
	const double getStrongVariableViolation() const {return m_strongVariableViolation;}
	
	// Read global constraint counter, i.e. the number of added constraints of specific type.
	const int addedKConstraints() const {return m_nKConsAdded;}
	const int addedCConstraints() const {return m_nCConsAdded;}
	
	
	// Set parameters
	void setKIterations(int n) {m_nKuratowskiIterations = n;}
	void setNSubdivisions(int n) {m_nSubdivisions = n;}
	void setNKuratowskiSupportGraphs(int n) {m_nKuratowskiSupportGraphs = n;}
	void setNHeuristicRuns(int n) {m_nHeuristicRuns = n;}
	void setKBoundHigh(double n) {m_kuratowskiBoundHigh = ((n>0.0 && n<1.0) ? n : 0.8);}
	void setKBoundLow(double n) {m_kuratowskiBoundLow = ((n>0.0 && n<1.0) ? n : 0.2);}
	void heuristicLevel(int level) {m_heuristicLevel = level;}
	void setHeuristicRuns(int n) {m_nHeuristicRuns = n;}
	void setPertubation(bool b) {m_usePerturbation = b;}
	void setHeuristicFractionalBound(double b) {m_heuristicFractionalBound = b;}
	void setHeuristicPermutationLists(int n) {m_nHeuristicPermutationLists = n;}
	void setMPHeuristic(bool b) {m_mpHeuristic = b;}//!< Switches use of lower bound heuristic
	void setNumAddVariables(int i) {m_numAddVariables=i;}
	void setStrongConstraintViolation(double d) { m_strongConstraintViolation=d;}
	void setStrongVariableViolation(double d) { m_strongVariableViolation=d;}
	
	//! If set to true, PORTA output is written in a file
	void setPortaFile(bool b) {m_porta = b;}
	
	// Updating global constraint counter
	void updateAddedCCons(int n) {m_nCConsAdded += n;}
	void updateAddedKCons(int n) {m_nKConsAdded += n;}
	
	// Returns global primal and dual bounds.
	double getPrimalBound() {return globalPrimalBound;}
	double getDualBound() {return globalDualBound;}
	
	// Cut pools for connectivity and planarity
	//! Returns cut pool for connectivity
	ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE> *getCutConnPool() {return m_cutConnPool;}
	//! Returns cut pool for planarity
	ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE> *getCutKuraPool() {return m_cutKuraPool;}
	
	//! Returns true if default cut pool is used. Otherwise, separate 
	//! connectivity and Kuratowski pools are generated and used.
	bool &useDefaultCutPool() { return m_useDefaultCutPool;} 

#ifdef OGDF_DEBUG
	bool m_solByHeuristic; //solution computed by heuristic or ILP
		// Simple output function to print the given graph to the console.
	// Used for debugging only.
	void printGraph(const Graph &G);
#endif

	//! The name of the file that contains the standard, i.e., non-cut,
	//! constraints (may be deleted by ABACUS and shouldn't be stored twice)
	const char* getStdConstraintsFileName()
	{
		return "StdConstraints.txt";
	}
	
	int getNumInactiveVars() { return m_inactiveVariables.size();}

protected:
	
	// Initializes constraints and variables and an initial dual bound.
	virtual void initializeOptimization();
	
	// Function that is invoked at the end of the optimization
	virtual void terminateOptimization();
		
	double heuristicInitialLowerBound();
	
private:
		
	// Computes a dual bound for the optimal solution.
	// Tries to find as many edge-disjoint Kuratowski subdivisions as possible.
	// If k edge-disjoint groups of subdivisions are found, the upper bound can be
	// initialized with number of edges in underlying graph minus k.
	double heuristicInitialUpperBound();
	
	// Is invoked by heuristicInitialUpperBound()
	void clusterConnection(cluster c, GraphCopy &GC, double &upperBound);
	
	// Computes the graphtheoretical distances of edges incident to node \a u.
    void nodeDistances(node u, NodeArray<NodeArray<int> > &dist);
		
	
	// Parameters
	int m_nKuratowskiSupportGraphs; 	// Maximal number of times the Kuratowski support graph is computed
	int m_nKuratowskiIterations; 		// Maximal number of times BoyerMyrvold is invoked
	int m_nSubdivisions; 				// Maximal number of extracted Kuratowski subdivisions
	int m_nMaxVars; 					// Max Number of variables
	int m_heuristicLevel; 				// Indicates if primal heuristic shall be used or not
	int m_nHeuristicRuns; 				// Counts how often the primal heuristic has been called
	
	bool m_usePerturbation; 			// Indicates whether C-variables should be perturbated or not
	double m_branchingGap;				// Modifies the branching behaviour
	double m_heuristicFractionalBound;	
	int m_nHeuristicPermutationLists;	// The number of permutation lists used in the primal heuristic
	bool m_mpHeuristic;                 //!< Indicates if simple max planar subgraph heuristic 
										// should be used to derive lower bound if only root cluster exists 
	
	double m_kuratowskiBoundHigh;		// Upper bound for deterministic edge addition in computation of the Supportgraph
	double m_kuratowskiBoundLow;		// Lower bound for deterministic edge deletion in computation of the Supportgraph
	
	int m_numAddVariables;				// how many variables should i add maximally per pricing round?
	double m_strongConstraintViolation; // when do i consider a constraint strongly violated -> separate in first stage
	double m_strongVariableViolation;   // when do i consider a variable strongly violated (red.cost) -> separate in first stage
	
	ABA_MASTER::OUTLEVEL m_out; 		// Determines the output level of ABACUS
	ABA_STRING *m_maxCpuTime;			// Time threshold for optimization
	

	// The basic objective function coefficient for connection edges.
	double m_epsilon;
	// If pertubation is used, this variable stores the largest occuring coeff,
	// i.e. the one closest to 0. Otherwise it corresponds to \a m_epsilon 
	double m_largestConnectionCoeff;
	
	// Counters for the number of added constraints
	int m_nCConsAdded;
	int m_nKConsAdded;
	int m_solvesLP;
	int m_varsInit;
	int m_varsAdded;
	int m_varsPotential;
	int m_varsMax;
	int m_varsCut;
	int m_varsKura;
	int m_varsPrice;
	int m_varsBranch;
	int m_activeRepairs;
	ArrayBuffer<int> m_repairStat;
	inline void clearActiveRepairs() {
		if(m_activeRepairs) {
			m_repairStat.push(m_activeRepairs);
			m_activeRepairs = 0; 
		}
	}
	
	double globalPrimalBound;
	double globalDualBound;
	
	inline double getDoubleTime(const ABA_TIMER* act) {
    	long tempo = act->centiSeconds()+100*act->seconds()+6000*act->minutes()+360000*act->hours();
    	return  ((double) tempo)/ 100.0;
	}
	
	//number of calls of the fast max planar subgraph heuristic
	const int m_fastHeuristicRuns;
	
	//! Cut pools for connectivity and Kuratowski constraints
	ABA_STANDARDPOOL< ABA_CONSTRAINT, ABA_VARIABLE > *m_cutConnPool; //!< Connectivity Cuts
	ABA_STANDARDPOOL< ABA_CONSTRAINT, ABA_VARIABLE > *m_cutKuraPool; //!< Kuratowski Cuts
	
	//! Defines if the ABACUS default cut pool or the separate Connectivity
	//! and Kuratowski constraint pools are used
	bool m_useDefaultCutPool;
	
	//! Defines if only clustered planarity is checked, i.e.,
	//! all edges of the original graph need to be fixed to be
	//! part of the solution
	bool m_checkCPlanar;  
	bool m_checkCPlanar2;  
	
	//!
	double m_delta;
	double m_deltaCount;	
	double nextConnectCoeff() { return (m_checkCPlanar ? -1 : -m_epsilon) + m_deltaCount--*m_delta; }; // TODO-TESTING
	EdgeVar* createVariable(ListIterator<nodePair>& it) {
		++m_varsAdded;
		EdgeVar* v = new EdgeVar(this, nextConnectCoeff(), EdgeVar::CONNECT, (*it).v1, (*it).v2);
		v->printMe(Logger::slout()); 
		m_inactiveVariables.del(it);
		return v;
	}
	List<nodePair> m_inactiveVariables;
	void generateVariablesForFeasibility(const List<ChunkConnection*>& ccons, List<EdgeVar*>& connectVars);
	
	bool goodVar(node a, node b);
	
	//! If set to true, PORTA output is written in a file
	bool m_porta;
	//! writes coefficients of all orig and connect variables in constraint con into 
	//! emptied list coeffs
	void getCoefficients(ABA_CONSTRAINT* con,  const List<EdgeVar *> & orig, 
		const List<EdgeVar* > & connect, List<double> & coeffs);
};

}//end namespace

#endif
