#ifndef _SOLVER_LIBRARY_H_
#define _SOLVER_LIBRARY_H_
#if defined(_MSC_VER)
#pragma once
#endif

/*! 
* \file solver_library.h
* \ingroup Objects
* \brief A file containing the header for the static SolverLibrary class which
* contains helper methods used by SolverComponents.
* \author Josh Lurz
* \date $Date$
* \version $Revision$
*/
#if defined(_MSC_VER)
#pragma warning( disable: 4275 )
#endif

#include <mtl/matrix.h>
#include <vector>
#include <map>
#include <string>
#include <functional>

typedef mtl::matrix<double, mtl::rectangle<>, mtl::dense<>, mtl::row_major>::type Matrix;

class Marketplace;
class World;
class SolverInfo;
class SolverInfoSet;

/*!
* \ingroup Objects
* \brief A class with all static functions which are used by the SolverComponents classes and 
* contains common functionality. 
* \author Josh Lurz
*/

class SolverLibrary {
public:
    // Some of these still might go.
   static double getRelativeED( const double excessDemand, const double demand, const double excessDemandFloor );
   static bool isWithinTolerance( const double excessDemand, const double demand, const double solutionTolerance, const double excessDemandSolutionFloor );
   static void derivatives( Marketplace* marketplace, World* world, SolverInfoSet& solutionVector, const int per );
   static void invertMatrix( Matrix& A );
	
   static void updateMatrices( SolverInfoSet& sol, Matrix& JFSM, Matrix& JFDM, Matrix& JF );
   static void calculateNewPricesLogNR( SolverInfoSet& solverSet, Matrix& JFSM, Matrix& JFDM, Matrix& JF );
   static bool bracket( Marketplace* marketplace, World* world, const double bracketInterval, SolverInfoSet& sol, const int period );
   
private:
    typedef std::map<std::string, std::vector<double> > RegionalMarketValues;
    typedef std::vector<std::string>::const_iterator RegionIterator;
    
    //! A simple struct to link Supplies and Demands.
    struct RegionalSDDifferences {
        RegionalMarketValues supplies;
        RegionalMarketValues demands;
    };
    
    //! A function object to compare to values and see if they are approximately equal. 
    struct ApproxEqual : public std::unary_function<double, bool> {
        const double compareValue; //!< A value to compare the argument value against.
        const double tolerance; //!< The tolerance within which to return that the values are equal.
        ApproxEqual( double compareValueIn, double toleranceIn ):compareValue( compareValueIn ), tolerance( toleranceIn ){}
        operator()( const double value ){
            return fabs( value - compareValue ) < tolerance;
        }
    };
    
    static bool doRegionalValuesSum( const RegionalMarketValues& regionalValues, const std::vector<double>& worldTotals, const bool doPrint = false );
    static const RegionalSDDifferences calcRegionalSDDifferences( Marketplace* marketplace, World* world, SolverInfoSet& sol, const int per );
};

#endif // _SOLVER_LIBRARY_H_

