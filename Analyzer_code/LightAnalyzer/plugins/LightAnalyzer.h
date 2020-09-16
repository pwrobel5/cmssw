// -*- C++ -*-
//
// Package:    Analyzer_code/LightAnalyzer
// Class:      LightAnalyzer
//
/**\class LightAnalyzer LightAnalyzer.cc Analyzer_code/LightAnalyzer/plugins/LightAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Piotr Wrobel
//         Created:  Wed, 09 Sep 2020 14:42:32 GMT
//
//

#ifndef LIGHT_ANALYZER_H
#define LIGHT_ANALYZER_H

// system include files
#include <memory>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/CTPPSReco/interface/CTPPSDiamondRecHit.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondLocalTrack.h"

#include "TH2F.h"

#include "DiamondDetectorClass.h"

//
// constants, enums and typedefs
//

static const int CHANNELS_NUMBER = 12;
static const int PLANES_NUMBER = 4;
static const int SECTORS_NUMBER = 2;
edm::Service<TFileService> fs;

enum SectorID {
    SECTOR_45,
	SECTOR_56
};

enum PlaneID
{
	PLANE_0,
	PLANE_1,
	PLANE_2,
	PLANE_3		
};

enum StationID
{
	STATION_210_M,
	STATION_TIMING,
    STATION_220_M		
};

//
// class declaration
//

class LightAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
    public:
        explicit LightAnalyzer(const edm::ParameterSet&);
        ~LightAnalyzer();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


    private:
        virtual void beginJob() override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endJob() override;

        // ----------member data ---------------------------

        // objects to retrieve
        edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondRecHit> > tokenRecHit;
        edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondLocalTrack> > tokenLocalTrack;

        // external
        DiamondDetectorClass DiamondDetector;
};

//
// static data member definitions
//

#endif