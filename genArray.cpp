/*  An open source Navier-Stokes CFD solver.
    Copyright (C) 2015  Michael Nucci (michael.nucci@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <cstdlib>  // exit()
#include <iostream>  // cout
#include <cmath>
#include "genArray.hpp"
#include "mpi.h"

using std::cout;
using std::endl;
using std::cerr;

// ------------------------------------------------------------------
// functions for genArray class

// operation overload for << - allows use of cout, cerr, etc.
ostream &operator<<(ostream &os, const genArray &m) {
  for (auto rr = 0; rr < NUMVARS; rr++) {
    os << m[rr] << endl;
  }
  return os;
}

// member function to zero the matrix
void genArray::Zero() {
  for (auto &val : data_) {
    val = 0.0;
  }
}

// member function to sum column matrix
double genArray::Sum() {
  auto sum = 0.0;
  for (auto &val : data_) {
    sum += val;
  }
  return sum;
}

// member function to sum the residuals from all processors
void genArray::GlobalReduceMPI(const int &rank, const int &numEqns) {
  // Get residuals from all processors
  if (rank == ROOTP) {
    MPI_Reduce(MPI_IN_PLACE, &data_[0], numEqns, MPI_DOUBLE, MPI_SUM,
               ROOTP, MPI_COMM_WORLD);
  } else {
    MPI_Reduce(&data_[0], &data_[0], numEqns, MPI_DOUBLE, MPI_SUM, ROOTP,
               MPI_COMM_WORLD);
  }
}