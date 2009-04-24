// Copyright (C) 2009 International Business Machines.
// All Rights Reserved.
// This code is published under the Common Public License.
//
// $Id$
//
// Author:  Andreas Waechter               IBM    2009-04-02

// This file is part of the Ipopt tutorial.  It is a correct version
// of a C++ implemention of the coding exercise problem (in AMPL
// formulation):
//
// param n := 4;
//
// var x {1..n} <= 0, >= -1.5, := -0.5;
//
// minimize obj:
//   sum{i in 1..n} (x[i]-1)^2;
//
// subject to constr {i in 2..n-1}:
//   (x[i]^2+1.5*x[i]-i/n)*cos(x[i+1]) - x[i-1] = 0;
//
// The constant term "i/n" in the constraint is supposed to be input data
//

#include "TutorialCpp_nlp.hpp"

// We use sin and cos
#include <cmath>

using namespace Ipopt;

// constructor
TutorialCpp_NLP::TutorialCpp_NLP(Index N, const Number* a)
  :
  N_(N)
{
  // Copy the values for the constants appearing in the constraints
  a_ = new Number[N_-2];
  for (Index i=0; i<N_-2; i++) {
    a_[i] = a[i];
  }
}

//destructor
TutorialCpp_NLP::~TutorialCpp_NLP()
{
  // make sure we delete everything we allocated
  delete [] a_;
}

// returns the size of the problem
bool TutorialCpp_NLP::get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
				   Index& nnz_h_lag,
				   IndexStyleEnum& index_style)
{
  // number of variables is given in constructor
  n = N_;

  // we have N_-2 constraints
  m = N_-2;

  // each constraint has three nonzeros
  nnz_jac_g = n*m;  // DENSE FOR THIS FIRST TEST!!!

  // We have the full diagonal, and the first off-diagonal except for
  // the first and last variable
  nnz_h_lag = n + (n-2);

  // use the C style indexing (0-based) for the matrices
  index_style = TNLP::C_STYLE;

  return true;
}

// returns the variable bounds
bool TutorialCpp_NLP::get_bounds_info(Index n, Number* x_l, Number* x_u,
				      Index m, Number* g_l, Number* g_u)
{
  // here, the n and m we gave IPOPT in get_nlp_info are passed back to us.
  // If desired, we could assert to make sure they are what we think they are.
  //assert(n == );
  //assert(m == );

  // the variables have lower bounds of -1.5
  for (Index i=0; i<n; i++) {
    x_l[i] = -1.5;
  }

  // the variables have upper bounds of 0
  for (Index i=0; i<n; i++) {
    x_u[i] = 0.;
  }

  // all constraints are equality constraints with right hand side zero
  for (Index j=0; j<m; j++) {
    g_l[j] = g_u[j] = 0.;
  }

  return true;
}

// returns the initial point for the problem
bool TutorialCpp_NLP::get_starting_point(Index n, bool init_x, Number* x,
					 bool init_z, Number* z_L, Number* z_U,
					 Index m, bool init_lambda,
					 Number* lambda)
{
  // Here, we assume we only have starting values for x, if you code
  // your own NLP, you can provide starting values for the dual variables
  // if you wish
  assert(init_x == true);
  assert(init_z == false);
  assert(init_lambda == false);

  // initialize to the given starting point
  for (Index i=0; i<n; i++) {
    x[i] = -0.5;
  }

  return true;
}

// returns the value of the objective function
// sum{i in 1..n} (x[i]-1)^2;
bool TutorialCpp_NLP::eval_f(Index n, const Number* x,
			     bool new_x, Number& obj_value)
{
  obj_value = 0.;
  for (Index i=0; i<n; i++) {
    obj_value += (x[i]-1.)*(x[i]-1.);
  }

  return true;
}

// return the gradient of the objective function grad_{x} f(x)
bool TutorialCpp_NLP::eval_grad_f(Index n, const Number* x,
				  bool new_x, Number* grad_f)
{
  for (Index i=0; i<n; i++) {
    grad_f[i] = 2.*(x[i]-1.);
  }

  return true;
}

// return the value of the constraints: g(x)
// (x[j+1]^2+1.5*x[j+1]-a[j])*cos(x[j+2]) - x[j] = 0;
bool TutorialCpp_NLP::eval_g(Index n, const Number* x,
			     bool new_x, Index m, Number* g)
{
  for (Index j=0; j<m; j++) {
    g[j] = (x[j+1]*x[j+1] + 1.5*x[j+1] -a_[j])*cos(x[j+2]) - x[j];
  }

  return true;
}

// return the structure or values of the jacobian
bool TutorialCpp_NLP::eval_jac_g(Index n, const Number* x, bool new_x,
				 Index m, Index nele_jac, Index* iRow,
				 Index *jCol, Number* values)
{
  if (values == NULL) {
    Index inz = 0;
    for (Index i = 0; i < m; i++) {
      for (Index j = 0; j < n; j++) {
	iRow[inz] = i;
	jCol[inz] = j;
	inz++;
      }
    }
  }
  else {
    return false;
  }

  return true;
}

//return the structure or values of the hessian
bool TutorialCpp_NLP::eval_h(Index n, const Number* x, bool new_x,
			     Number obj_factor, Index m, const Number* lambda,
			     bool new_lambda, Index nele_hess, Index* iRow,
			     Index* jCol, Number* values)
{
  return false;
  if (values == NULL) {
    // return the structure (lower or upper triangular part) of the
    // Hessian of hte Lagrangian function

    // HERE FILL iRow and jCol

  }
  else {
    // return the values of the Hessian of hte Lagrangian function

    // HERE FILL values
  }

  return true;
}

void TutorialCpp_NLP::finalize_solution(SolverReturn status,
					Index n, const Number* x,
					const Number* z_L, const Number* z_U,
					Index m, const Number* g,
					const Number* lambda,
					Number obj_value,
					const IpoptData* ip_data,
					IpoptCalculatedQuantities* ip_cq)
{
  // here is where we would store the solution to variables, or write
  // to a file, etc so we could use the solution.

  // For this example, we write the solution to the console
  printf("\n\nSolution of the primal variables, x\n");
  for (Index i=0; i<n; i++) {
    printf("x[%d] = %e\n", i, x[i]);
  }

  printf("\n\nSolution of the bound multipliers, z_L and z_U\n");
  for (Index i=0; i<n; i++) {
    printf("z_L[%d] = %e\n", i, z_L[i]);
  }
  for (Index i=0; i<n; i++) {
    printf("z_U[%d] = %e\n", i, z_U[i]);
  }

  printf("\n\nObjective value\n");
  printf("f(x*) = %e\n", obj_value);
}