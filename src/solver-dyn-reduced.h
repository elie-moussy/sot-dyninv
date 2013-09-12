/*
 * Copyright 2011, Nicolas Mansard, LAAS-CNRS
 *
 * This file is part of sot-dyninv.
 * sot-dyninv is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 * sot-dyninv is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.  You should
 * have received a copy of the GNU Lesser General Public License along
 * with sot-dyninv.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __sot_dyninv_SolverDynReduced_H__
#define __sot_dyninv_SolverDynReduced_H__
/* --------------------------------------------------------------------- */
/* --- API ------------------------------------------------------------- */
/* --------------------------------------------------------------------- */

#if defined (WIN32)
#  if defined (solver_op_space_EXPORTS)
#    define SOTSOLVERDYNREDUCED_EXPORT __declspec(dllexport)
#  else
#    define SOTSOLVERDYNREDUCED_EXPORT __declspec(dllimport)
#  endif
#else
#  define SOTSOLVERDYNREDUCED_EXPORT
#endif

/* --------------------------------------------------------------------- */
/* --- INCLUDE --------------------------------------------------------- */
/* --------------------------------------------------------------------- */


/* SOT */
#include <sot-dyninv/signal-helper.h>
#include <sot-dyninv/entity-helper.h>
#include <sot-dyninv/stack-template.h>
#include <sot-dyninv/task-dyn-pd.h>
#include <soth/HCOD.hpp>
#include <Eigen/QR>
//#include <Eigen/SVD>
#include <sot-dyninv/col-piv-qr-solve-in-place.h>

namespace dynamicgraph {
  namespace sot {
    namespace dyninv {

      /* --------------------------------------------------------------------- */
      /* --- CLASS ----------------------------------------------------------- */
      /* --------------------------------------------------------------------- */

      class SOTSOLVERDYNREDUCED_EXPORT SolverDynReduced
	:public ::dynamicgraph::Entity
	,public ::dynamicgraph::EntityHelper<SolverDynReduced>
	,public sot::Stack< TaskDynPD >
	{

	public: /* --- CONSTRUCTOR ---- */

	  SolverDynReduced( const std::string & name );

	public: /* --- STACK INHERITANCE --- */

	  typedef sot::Stack<TaskDynPD> stack_t;
	  using stack_t::TaskDependancyList_t;
	  using stack_t::StackIterator_t;
	  using stack_t::StackConstIterator_t;
	  using stack_t::stack;

	  virtual TaskDependancyList_t getTaskDependancyList( const TaskDynPD& task );
	  virtual void addDependancy( const TaskDependancyList_t& depList );
	  virtual void removeDependancy( const TaskDependancyList_t& depList );
	  virtual void resetReady( void );

	public: /* --- ENTITY INHERITANCE --- */

	  static const std::string CLASS_NAME;
	  virtual void display( std::ostream& os ) const;
	  virtual const std::string& getClassName( void ) const { return CLASS_NAME; }

	public:  /* --- SIGNALS --- */

	  DECLARE_SIGNAL_IN(matrixInertia,dynamicgraph::Matrix);
	  DECLARE_SIGNAL_IN(inertiaSqroot,dynamicgraph::Matrix);
	  DECLARE_SIGNAL_IN(inertiaSqrootInv,dynamicgraph::Matrix);
	  DECLARE_SIGNAL_IN(velocity,dynamicgraph::Vector);
	  DECLARE_SIGNAL_IN(dyndrift,dynamicgraph::Vector);
	  DECLARE_SIGNAL_IN(damping,double);
	  DECLARE_SIGNAL_IN(breakFactor,double);
	  DECLARE_SIGNAL_IN(posture,dynamicgraph::Vector);
	  DECLARE_SIGNAL_IN(position,dynamicgraph::Vector);

	  DECLARE_SIGNAL_OUT(precompute,int);

	  DECLARE_SIGNAL_OUT(inertiaSqrootOut,dynamicgraph::Matrix);
	  DECLARE_SIGNAL_OUT(inertiaSqrootInvOut,dynamicgraph::Matrix);

	  DECLARE_SIGNAL_OUT(sizeForcePoint,int);
	  DECLARE_SIGNAL_OUT(sizeForceSpatial,int);
	  DECLARE_SIGNAL_OUT(sizeConfiguration,int);

	  DECLARE_SIGNAL_OUT(Jc,dynamicgraph::Matrix);
	  DECLARE_SIGNAL_OUT(forceGenerator,dynamicgraph::Matrix);
	  DECLARE_SIGNAL_OUT(freeMotionBase,dynamicgraph::Matrix);
	  DECLARE_SIGNAL_OUT(freeForceBase,dynamicgraph::Matrix);
	  DECLARE_SIGNAL_OUT(driftContact,dynamicgraph::Vector);
	  DECLARE_SIGNAL_OUT(sizeMotion,int);
	  DECLARE_SIGNAL_OUT(sizeActuation,int);

	  DECLARE_SIGNAL_OUT(solution,dynamicgraph::Vector);
	  DECLARE_SIGNAL_OUT(reducedControl,dynamicgraph::Vector);
	  DECLARE_SIGNAL_OUT(reducedForce,dynamicgraph::Vector);
	  DECLARE_SIGNAL_OUT(acceleration,dynamicgraph::Vector);
	  DECLARE_SIGNAL_OUT(forces,dynamicgraph::Vector);
	  DECLARE_SIGNAL_OUT(torque,dynamicgraph::Vector);

	  DECLARE_SIGNAL_OUT(forcesNormal,dynamicgraph::Vector);
	  DECLARE_SIGNAL_OUT(activeForces,dynamicgraph::Vector);


	  /* Temporary time-dependant shared variables. */
	  DECLARE_SIGNAL(Jcdot,OUT,dynamicgraph::Matrix);

	private:  /* --- CONTACT POINTS --- */

	  typedef boost::shared_ptr<dynamicgraph::SignalPtr<dynamicgraph::Matrix,int> > matrixSINPtr;
	  typedef boost::shared_ptr<dynamicgraph::SignalPtr<dynamicgraph::Vector,int> > vectorSINPtr;
	  typedef boost::shared_ptr<dynamicgraph::Signal<dynamicgraph::Vector,int> > vectorSOUTPtr;
	  struct Contact
	  {
	    matrixSINPtr jacobianSIN;
	    matrixSINPtr JdotSIN;
	    matrixSINPtr supportSIN;
	    vectorSINPtr correctorSIN;
	    vectorSOUTPtr forceSOUT,fnSOUT;
	    int position;
	    std::pair<int,int> range;
	  };
	  typedef std::map< std::string,Contact > contacts_t;
	  contacts_t contactMap;

	public:
	  void addContact( const std::string & name,
			   dynamicgraph::Signal<dynamicgraph::Matrix,int> * jacobianSignal,
			   dynamicgraph::Signal<dynamicgraph::Matrix,int> * JdotSignal,
			   dynamicgraph::Signal<dynamicgraph::Vector,int> * corrSignal,
			   dynamicgraph::Signal<dynamicgraph::Matrix,int> * contactPointsSignal );
	  void addContactFromTask( const std::string & taskName, const std::string & contactName );
	  void removeContact( const std::string & name );
	  void dispContacts( std::ostream& os ) const;

	  matrixSINPtr getSupportSIN( const std::string & contacName );


	public: /* --- COMMANDS --- */
	  void debugOnce( void );


	private: /* --- INTERNAL COMPUTATIONS --- */
	  void refreshTaskTime( int time );
	  void resizeSolver( void );
	  void computeSizesForce( int t );

	private:
	  typedef boost::shared_ptr<soth::HCOD> hcod_ptr_t;
	  hcod_ptr_t hsolver;

	  int G_rank;
	  Eigen::ColPivQRSolveInPlace X_qr,Gt_qr;


	  Eigen::MatrixXd Cforce,Czero;
	  soth::VectorBound bforce,bzero;
	  std::vector< Eigen::MatrixXd > Ctasks;
	  std::vector< soth::VectorBound > btasks;

	  Eigen::MatrixXd BV;

	  /* Force drift = xddot^* - Jdot qdot. */
	  Eigen::VectorXd solution,forceDrift;

	}; // class SolverDynReduced

    } // namespace dyninv
  } // namespace sot
} // namespace dynamicgraph



#endif // #ifndef __sot_dyninv_SolverDynReduced_H__
