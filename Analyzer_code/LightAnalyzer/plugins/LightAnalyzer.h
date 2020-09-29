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
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/CTPPSReco/interface/CTPPSDiamondRecHit.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondLocalTrack.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "CondFormats/RunInfo/interface/LHCInfo.h"
#include "CondFormats/DataRecord/interface/LHCInfoRcd.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"

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
        void readCrossingAngle(const edm::EventSetup& iSetup);
        void readVertexZ(const edm::Event& iEvent);
        void fillTOTvsLS(const edm::Event& iEvent, const std::vector<bool>& sectorsToAnalyze);
        void initializeChannelHistograms(const CTPPSDiamondDetId& detId);
        bool isRecHitValid(const CTPPSDiamondRecHit& recHit, const ChannelKey& recHitKey);
        void performTimingAnalysis(const std::vector<bool>& sectorsToAnalyze);
        int getSectorNumberFromLocalTrack(const std::pair<const CTPPSDiamondLocalTrack, std::vector<std::pair<ChannelKey, CTPPSDiamondRecHit>>>& localTrack);
        double getNextTrackTimeValue(double trackTime, double trackPrecision, double hitTime);
        double getNextTrackPrecisionValue(double trackPrecision);

        void initializeGlobalHistograms();
        void initializeSectorHistograms();

        void calculateAverages();
        void fillHistogramsWithAverages();
        void makeGlobalProfiles();
        void makeSectorProfiles();
        void makeChannelProfiles();

        // constants
        static const int CHANNELS_NUMBER = 12;
        static const int PLANES_NUMBER = 4;
        static const int SECTORS_NUMBER = 2;
        edm::Service<TFileService> fs;

        static const int TRACK_PRECISION_START_VALUE = 1.0;

        // ----------member data ---------------------------

        // objects to retrieve
        edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondRecHit>> tokenRecHit;
        edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondLocalTrack>> tokenLocalTrack;
        edm::EDGetTokenT<edm::DetSetVector<CTPPSPixelLocalTrack>> tokenPixelLocalTrack;
        std::string lhcInfoLabel;
        edm::EDGetTokenT<edm::View<reco::Vertex>> vertexToken;

        // external
        DiamondDetectorClass diamondDetector;
        int validOOT;
        int minBunchCrossing;
        int maxBunchCrossing;

        // selection parameters
        std::map<std::pair<int, int>, std::pair<int, int>> NTracksCuts; // (sector, station), (Lcut, Ucut)

        std::map<std::pair<int, int>, int> PixelMux; // arm, station

        // directories
        TFileDirectory globalInfoDirectory;
        std::vector<TFileDirectory> sectorDirectories;
        std::map<ChannelKey, TFileDirectory> channelDirectories;

        // histograms
        TH2F* AvVertexZvsXAngleHistogram;
        TH2F* XAnglevsLSHistogram;
        TH2F* AvVertexZvsLSHistogram;

        std::vector<TH1F*> TrackTimeSectorHistograms;        
        std::vector<TH2F*> TOTvsLSSectorHistograms;
        std::vector<TH2F*> TrackTimevsLSSectorHistograms;
        std::vector<TH2F*> TrackTimevsBXSectorHistograms;
        std::vector<TH2F*> TrackTimevsXAngleSectorHistograms;
        std::vector<TH2F*> AvVertexZvsAvTrackTimeSectorHistograms;

        std::map<ChannelKey, TH2F*> TOTvsLSChannelHistograms;

        // profiles
        TProfile* AvVertexZvsXAngleProfile;
        TProfile* AvVertexZvsLSProfile;        

        std::map<int, TProfile*> TOTvsLSSectorProfiles;
        std::map<int, TProfile*> TrackTimevsLSSectorProfiles;
        std::map<int, TProfile*> TrackTimevsBXSectorProfiles;
        std::map<int, TProfile*> TrackTimevsXAngleSectorProfiles;
        std::map<int, TProfile*> AvVertexZvsAvTrackTimeSectorProfiles;

        std::map<ChannelKey, TProfile*> TOTvsLSChannelProfiles;

        // values constant for given event
        int lumiSection;
        int bunchCrossing;
        int crossingAngle;

        // averages utils
        std::map<std::pair<int, int>, unsigned int> trackTimeCounters; // key: (sector number, lumiSection)
        std::map<std::pair<int, int>, double> trackTimeSums;        
		std::map<std::pair<int, int>, double> trackTimeAverages;

        std::map<int, unsigned int> vertexZCounters; // key: lumiSection
		std::map<int, double> vertexZSums;
		std::map<int, double> vertexZAverages;		

		std::map<int, int> crossingAngles; // key: lumiSection
};

#endif