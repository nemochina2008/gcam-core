/*! 
* \file cpriceSubsector.cpp
* \ingroup CIAM
* \brief cpriceSubsector class source file.
* \author Sonny Kim
* \date $Date$
* \version $Revision$
*/


#include "util/base/include/definitions.h"
#include <string>
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "util/base/include/configuration.h"
#include "sectors/include/subsector.h"
#include "technologies/include/technology.h"
#include "containers/include/scenario.h"
#include "sectors/include/sector.h"
#include "util/base/include/model_time.h"
#include "util/base/include/xml_helper.h"
#include "marketplace/include/marketplace.h"
#include "util/base/include/summary.h"
#include "emissions/include/indirect_emiss_coef.h"
#include "containers/include/world.h"
#include "containers/include/gdp.h"
#include "sectors/include/cpriceSubsector.h"

using namespace std;
using namespace xercesc;

extern Scenario* scenario;
// static initialize.
const string Cpricesubsector::XML_NAME = "cpricesubsector";

/*! \brief Default constructor.
*
* Constructor initializes member variables with default values, sets vector sizes, etc.
*
* \author Sonny Kim, Steve Smith, Josh Lurz
*/
Cpricesubsector::Cpricesubsector( const string regionName, const string sectorName )
		:Subsector (regionName,  sectorName ) {
}

/*! \brief calculate Subsector unnormalized shares including GHG tax
*
* Variant of base class which includes the carbon price in share calculation
* Is temporary fix to allow mixed sub-sectors until top-down price calcuation is completed.
*
* \author Sonny Kim, Josh Lurz
* \param regionName region name
* \param period model period
* \param gdp_cap GDP per capita, relative to base year
*/
void Cpricesubsector::calcShare(const int period, const GDP* gdp ) {
    const World* world = scenario->getWorld();
    const Marketplace* marketplace = scenario->getMarketplace();
    const double CVRT90 = 2.212; // 1975 $ to 1990 $
    const double CVRT_tg_MT = 1e-3; // to get teragrams of carbon per EJ to metric tons of carbon per GJ

    // call function to compute technology shares
    calcTechShares( period );
    
    // calculate and return Subsector share; uses above price function
    // calc_price() uses normalized technology shares calculated above
    // Logit exponential should not be zero
    
    // compute Subsector weighted average price of technologies
    calcPrice( period);

    // Subsector logit exponential check
    if(lexp[period]==0) cerr << "SubSec Logit Exponential is 0." << endl;

	 // Get fuel used by technology (assume only one technology)
	 string fuelName = techs[ 0 ][ period ]->getFuelName();
	 
	 // Add carbon price to subsector price here
	 double ghgCost = 0;
	 if ( marketplace->doesMarketExist( "CO2", regionName, period ) ) {
		double GHGTax = marketplace->getPrice( "CO2", regionName, period );
		const double coefFuel = world->getPrimaryFuelCO2Coef( regionName, fuelName );
		ghgCost = GHGTax * (coefFuel) / CVRT90 * CVRT_tg_MT;
    }
	 
    if( subsectorprice[period]==0) {
        share[period] = 0;
    }
    else {
		double gdp_cap = gdp->getBestScaledGDPperCap( period );
		share[period] = shrwts[period]*pow(subsectorprice[period]+ghgCost,lexp[period])*pow(gdp_cap,fuelPrefElasticity[period]);
	}
	
   if (shrwts[period]  > 1e4) {
    cout << "WARNING: Huge shareweight for sub-sector " << name << " : " << shrwts[period] 
         << " in region " << regionName <<endl;
   }
      
   if (share[period] < 0) {
     cerr << "Share is < 0 for " << name << " in " << regionName << endl;
     cerr << "    subsectorprice[period]: " << subsectorprice[period] << endl;
     cerr << "    shrwts[period]: " << shrwts[period] << endl;
   }   
}

/*! \brief Get the XML node name for output to XML.
*
* This public function accesses the private constant string, XML_NAME.
* This way the tag is always consistent for both read-in and output and can be easily changed.
* This function may be virtual to be overriden by derived class pointers.
* \author Josh Lurz, James Blackwood
* \return The constant XML_NAME.
*/
const std::string& Cpricesubsector::getXMLName() const {
	return XML_NAME;
}

/*! \brief Get the XML node name in static form for comparison when parsing XML.
*
* This public function accesses the private constant string, XML_NAME.
* This way the tag is always consistent for both read-in and output and can be easily changed.
* The "==" operator that is used when parsing, required this second function to return static.
* \note A function cannot be static and virtual.
* \author Josh Lurz, James Blackwood
* \return The constant XML_NAME as a static.
*/
const std::string& Cpricesubsector::getXMLNameStatic() {
	return XML_NAME;
}