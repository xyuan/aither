/*  This file is part of aither.
    Copyright (C) 2015-18  Michael Nucci (michael.nucci@gmail.com)

    Aither is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Aither is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef CHEMISTRYHEADERDEF
#define CHEMISTRYHEADERDEF

// This header file contains the classes for chemistry models

#include <vector>
#include <memory>
#include "reactions.hpp"
#include "fluid.hpp"
#include "input.hpp"
#include "matrix.hpp"

using std::vector;
using std::unique_ptr;

// forward declarations
class thermodynamic;

// abstract base class for chemistry models
class chemistry {

 public:
  // Constructor
  chemistry() {}

  // move constructor and assignment operator
  chemistry(chemistry&&) noexcept = default;
  chemistry& operator=(chemistry&&) noexcept = default;

  // copy constructor and assignment operator
  chemistry(const chemistry&) = default;
  chemistry& operator=(const chemistry&) = default;

  // Member functions for abstract base class
  virtual int NumReactions() const { return 0; }
  virtual vector<double> SourceTerms(
      const vector<double>& rho, const double& t, const double &R,
      const unique_ptr<thermodynamic>& thermo) const = 0;
  virtual bool IsReacting() const { return false; }
  virtual double SrcSpecRad(const vector<double>& rho, const double& t,
                            const double& vol) const {
    return 0.0;
  }
  virtual squareMatrix SourceJac(
      const vector<double>& rho, const double& t, const double &R,
      const unique_ptr<thermodynamic>& thermo) const {
    return squareMatrix();
  }

  // Destructor
  virtual ~chemistry() noexcept {}
};

// this class models no reacting chemistry
class frozen : public chemistry {

 public:
  // Constructors
  frozen() : chemistry() {}

  // move constructor and assignment operator
  frozen(frozen&&) noexcept = default;
  frozen& operator=(frozen&&) noexcept = default;

  // copy constructor and assignment operator
  frozen(const frozen&) = default;
  frozen& operator=(const frozen&) = default;

  // Member functions
  vector<double> SourceTerms(
      const vector<double>& rho, const double& t, const double &R,
      const unique_ptr<thermodynamic>& thermo) const override {
    return vector<double>(rho.size(), 0.0);
  }

  // Destructor
  ~frozen() noexcept {}
};


// this class models reacting chemistry
class reacting : public chemistry {
  double freezingTemperature_;
  double refP_;
  vector<reaction> reactions_;
  vector<double> molarMass_;

  // private member functions
  void ReadFromFile(const input&);

 public:
  // Constructors
  reacting(const input &inp)
      : chemistry(), freezingTemperature_(inp.FreezingTemperature()) {
    refP_ = inp.Fluid(0).ReferencePressure();
    molarMass_.reserve(inp.Fluids().size());
    for (const auto& f : inp.Fluids()) {
      molarMass_.push_back(f.MolarMass());
      if (refP_ < f.ReferencePressure() * 0.999 || refP_ > f.ReferencePressure() * 1.001) {
        cerr << "ERROR: reference pressures for fluids are not the same!"
             << endl;
        exit(EXIT_FAILURE);
      }
    }
    this->ReadFromFile(inp);
  }

  // move constructor and assignment operator
  reacting(reacting&&) noexcept = default;
  reacting& operator=(reacting&&) noexcept = default;

  // copy constructor and assignment operator
  reacting(const reacting&) = default;
  reacting& operator=(const reacting&) = default;

  // Member functions
  int NumReactions() const override { return reactions_.size(); }
  vector<double> SourceTerms(
      const vector<double>& rho, const double& t, const double &R,
      const unique_ptr<thermodynamic>& thermo) const override;
  bool IsReacting() const override { return true; }
  double SrcSpecRad(const vector<double>& rho, const double& t,
                    const double& vol) const override {
    return 0.0;
  }
  squareMatrix SourceJac(
      const vector<double>& rho, const double& t, const double &R,
      const unique_ptr<thermodynamic>& thermo) const override {
    return squareMatrix();
  }

  // Destructor
  ~reacting() noexcept {}
};

// --------------------------------------------------------------------------
// function declarations



#endif
