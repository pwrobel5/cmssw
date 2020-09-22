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
#include <vector>
#include <map>
#include <algorithm>

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
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"

#include "TH2F.h"

#include "DiamondDetectorClass.h"

//
// enums and typedefs
//

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

        void readNTracksCuts(const edm::ParameterSet& iConfig);
        void fillPixelMux(const edm::Event& iEvent);
        std::vector<bool> getSectorsToAnalyze();
        void fillTOTvsLS(const edm::Event& iEvent, const std::vector<bool>& sectorsToAnalyze);
        bool isRecHitValid(const CTPPSDiamondRecHit& recHit, const ChannelKey& recHitKey);

        std::string makeSectorHistogramTitle(const std::string& titlePrefix, int sectorIndex);
        std::string makeSectorHistogramLegend(const std::string& legendPrefix, const std::string& legendSuffix, int sectorIndex);

        // constants
        static const int CHANNELS_NUMBER = 12;
        static const int PLANES_NUMBER = 4;
        static const int SECTORS_NUMBER = 2;
        edm::Service<TFileService> fs;

        // ----------member data ---------------------------

        // objects to retrieve
        edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondRecHit>> tokenRecHit;
        edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondLocalTrack>> tokenLocalTrack;
        edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelLocalTrack>> tokenPixelLocalTrack;

        // external
        DiamondDetectorClass diamondDetector;
        int validOOT;

        // selection parameters
        std::map<std::pair<int, int>, std::pair<int, int>> NTracksCuts; // (sector, station), (Lcut, Ucut)

        std::map<std::pair<int, int>, int> PixelMux; // arm, station

        // directories
        std::vector<TFileDirectory> sectorDirectories;

        // histograms
        std::vector<TH2F*> TOTvsLSSectorHistograms;
};

//
// static data member definitions
//

#endif