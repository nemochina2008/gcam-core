#ifndef _BUILDING_DMD_SUBSECTOR_H_
#define _BUILDING_DMD_SUBSECTOR_H_
#if defined(_MSC_VER)
#pragma once
#endif

/*! 
* \file building_dmd_subsector.h
* \ingroup CIAM
* \brief The building demand subsector
* \author Steve Smith
* \date $Date$
* \version $Revision$
*/

#include <vector>
#include <xercesc/dom/DOMNode.hpp>
#include "sectors/include/subsector.h"

// Forward declarations
class GDP;

/*! 
* \ingroup CIAM
* \brief A class which defines a building demand Subsector of the model.

* The subsector contains a group of building demand technology objects, 
* which act differently than normal technology objects in that they each generate a demand for a 
* specific building service (heating, cooling, lighting, etc.), which is then provided by 
* a supply sector. Therefore, this subsector does not share between technologies.
* This subsector also mediates information flow between the supply sectors and the building demand 
* technologies through marketInfo and other mechanisms.

* \author Steve Smith
*/

class BuildingDemandSubSector : public Subsector
{
public:
    BuildingDemandSubSector( const std::string regionName, const std::string sectorName );
    static const std::string& getXMLNameStatic();
    void initCalc( const MarketInfo* aSectorInfo, NationalAccount& aNationalAccount,
        Demographic* aDemographics, const MoreSectorInfo* aMoreSectorInfo, const int aPeriod );
    void setCalibrationStatus( const int period );
    void adjustForCalibration( double sectorDemand, double totalfixedOutput, double totalCalOutputs, const bool allFixedOutput, const int period );
    void calcPrice( const int period );
    double getOutput( const int period );
    void setoutput( const double demand, const int period, const GDP* gdp );
    void completeInit( );
protected:
    virtual const std::string& getXMLName() const;
    bool XMLDerivedClassParse( const std::string nodeName, const xercesc::DOMNode* curr );
    virtual void toOutputXMLDerived( std::ostream& out, Tabs* tabs ) const;
    virtual void toInputXMLDerived( std::ostream& out, Tabs* tabs ) const;
    virtual void toDebugXMLDerived( const int period, std::ostream& out, Tabs* tabs ) const;
    void setUpSubSectorMarkets();
    bool isNameOfChild  ( const std::string& nodename ) const;
    technology* createChild( const std::string& nodename ) const;
    void adjustTechnologyShareWeights( const int period );
    void MCDerivedClassOutput( ) const;
    std::vector<double> output; //!< Output of the building demand sector.
    std::vector<double> dayLighting; //!< amount of lighting need provided by daylighting
    std::vector<double> aveInsulation; //!< average insulation value (J/s-m^2) for this building type
    std::vector<double> floorToSurfaceArea; //!< conversion from floor space to surface area for this building type
    std::vector<double> nonEnergyCost; //!< non energy service costs of this building type (e.g., construction/rental, etc.)
private:
    static const std::string XML_NAME; //!< node name for toXML methods
};
#endif // _BUILDING_DMD_SUBSECTOR_H_