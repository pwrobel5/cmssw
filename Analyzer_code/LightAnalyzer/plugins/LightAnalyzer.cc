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

// user include files
#include "LightAnalyzer.h"
#include "HistogramConstants.h"

//
// constructors and destructor
//
LightAnalyzer::LightAnalyzer(const edm::ParameterSet& iConfig):
    tokenRecHit(consumes<edm::DetSetVector<CTPPSDiamondRecHit>>(iConfig.getParameter<edm::InputTag>("tagRecHit"))),
    tokenLocalTrack(consumes<edm::DetSetVector<CTPPSDiamondLocalTrack>>(iConfig.getParameter<edm::InputTag>("tagLocalTrack"))),
    tokenPixelLocalTrack(consumes< edm::DetSetVector<CTPPSPixelLocalTrack>>(iConfig.getParameter<edm::InputTag>("tagPixelLocalTrack"))),
    diamondDetector(iConfig, tokenRecHit, tokenLocalTrack),
    validOOT(iConfig.getParameter<int>("tagValidOOT")),
    sectorDirectories(SECTORS_NUMBER),
    TOTvsLSSectorHistograms(SECTORS_NUMBER),
    TrackTimevsLSSectorHistograms(SECTORS_NUMBER),
    TrackTimevsBXSectorHistograms(SECTORS_NUMBER)
{
    usesResource("TFileService");
    readNTracksCuts(iConfig);
}

void LightAnalyzer::readNTracksCuts(const edm::ParameterSet& iConfig)
{
    NTracksCuts[std::make_pair(SECTOR_45, STATION_210_M)] = std::make_pair(iConfig.getParameter<std::vector<int>>("Ntracks_Lcuts")[0],
																					iConfig.getParameter<std::vector<int>>("Ntracks_Ucuts")[0]);
	NTracksCuts[std::make_pair(SECTOR_45, STATION_220_M)] = std::make_pair(iConfig.getParameter<std::vector<int>>("Ntracks_Lcuts")[1],
																					iConfig.getParameter<std::vector<int>>("Ntracks_Ucuts")[1]);
	NTracksCuts[std::make_pair(SECTOR_56, STATION_210_M)] = std::make_pair(iConfig.getParameter<std::vector<int>>("Ntracks_Lcuts")[2],
																					iConfig.getParameter< std::vector <int> >( "Ntracks_Ucuts" )[2]);
	NTracksCuts[std::make_pair(SECTOR_56, STATION_220_M)] = std::make_pair(iConfig.getParameter<std::vector<int>>("Ntracks_Lcuts")[3],
																					iConfig.getParameter<std::vector<int>>("Ntracks_Ucuts")[3]);
}


LightAnalyzer::~LightAnalyzer() {}


//
// member functions
//

// ------------ method called for each event  ------------
void LightAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;

    fillPixelMux(iEvent);

    std::vector<bool> sectorsToAnalyze = getSectorsToAnalyze();
    if (std::find(sectorsToAnalyze.begin(), sectorsToAnalyze.end(), true) == sectorsToAnalyze.end())
        return;

    diamondDetector.ExtractData(iEvent);
    lumiSection = iEvent.luminosityBlock();
    bunchCrossing = iEvent.bunchCrossing();

    fillTOTvsLS(iEvent, sectorsToAnalyze);
    performTimingAnalysis(sectorsToAnalyze);
}

void LightAnalyzer::fillPixelMux(const edm::Event& iEvent) 
{
    edm::Handle<edm::DetSetVector<CTPPSPixelLocalTrack>> pixelLocalTrack;
    iEvent.getByToken(tokenPixelLocalTrack, pixelLocalTrack);

    PixelMux.clear();

    for (const auto& RPTracks : *pixelLocalTrack) {
        const CTPPSDetId detId(RPTracks.detId());

	  	for (const auto& track : RPTracks) 
	  	{
			if (!track.isValid()) continue; // validity of data is saved in track and provided by the data file			
			PixelMux[std::make_pair(detId.arm(), detId.station())]++;
      	}	 
    }
}

std::vector<bool> LightAnalyzer::getSectorsToAnalyze()
{
    std::vector<bool> sectorsToAnalyze(SECTORS_NUMBER, true);    

    for (const auto& NtracksCutsElement :  NTracksCuts)
	{
		if ((NtracksCutsElement.second.first < 0) || (NtracksCutsElement.second.second < 0)) continue; // don't care condition, values < 0 indicate that nothing happened
		if ((PixelMux[NtracksCutsElement.first] < NtracksCutsElement.second.first) ||
			(PixelMux[NtracksCutsElement.first] > NtracksCutsElement.second.second))  // condition violated
		{
			sectorsToAnalyze[NtracksCutsElement.first.first] = false;
		}		
	}

    return sectorsToAnalyze;
}

void LightAnalyzer::fillTOTvsLS(const edm::Event& iEvent, const std::vector<bool>& sectorsToAnalyze)
{
    edm::Handle<edm::DetSetVector<CTPPSDiamondRecHit>> timingRecHit;    
    iEvent.getByToken(tokenRecHit, timingRecHit);

    for (const auto& recHits : *timingRecHit) {
        const CTPPSDiamondDetId detId(recHits.detId());
        const ChannelKey recHitKey(detId.arm(), detId.plane(), detId.channel());

        if(channelDirectories.find(recHitKey) == channelDirectories.end() && recHits.size() > 0)
            initializeChannelHistograms(detId);

        if (sectorsToAnalyze[detId.arm()]) {
            for (const auto& recHit : recHits) {
                if (isRecHitValid(recHit, recHitKey) && diamondDetector.PadActive(detId.arm(), detId.plane(), detId.channel())) {
                    double ToT = diamondDetector.GetToT(detId.arm(), detId.plane(), detId.channel());

                    TOTvsLSSectorHistograms[recHitKey.sector]->Fill(lumiSection, ToT);
                    TOTvsLSChannelHistograms[recHitKey]->Fill(lumiSection, ToT);
                }
            }
        }
    }
}

void LightAnalyzer::initializeChannelHistograms(const CTPPSDiamondDetId& detId)
{
    int sectorIndex = detId.arm();
    int planeIndex = detId.plane();
    int channelIndex = detId.channel();

    ChannelKey recHitKey(sectorIndex, planeIndex, channelIndex);
    std::string directoryName = makeChannelDirectoryName(planeIndex, channelIndex);

    channelDirectories[recHitKey] = sectorDirectories[sectorIndex].mkdir(directoryName);
    TOTvsLSChannelHistograms[recHitKey] = channelDirectories[recHitKey].make<TH2F>(
        makeChannelHistogramTitle(TOT_VS_LS_HISTOGRAM_NAME, planeIndex, channelIndex).c_str(),
        makeChannelHistogramLegend(TOT_VS_LS_HISTOGRAM_NAME, TOT_VS_LS_HISTOGRAM_LEGEND_SUFFIX, planeIndex, channelIndex).c_str(),
        LS_BINS, LS_MIN, LS_MAX, TOT_BINS, TOT_MIN, TOT_MAX);
}

std::string LightAnalyzer::makeChannelDirectoryName(int planeIndex, int channelIndex)
{
    return "plane " + std::to_string(planeIndex) + "/channel " + std::to_string(channelIndex);
}

std::string LightAnalyzer::makeChannelHistogramTitle(const std::string& titlePrefix, int planeIndex, int channelIndex)
{
    return titlePrefix + " plane " + std::to_string(planeIndex) + " channel " + std::to_string(channelIndex);
}

std::string LightAnalyzer::makeChannelHistogramLegend(const std::string& legendPrefix, const std::string& legendSuffix, int planeIndex, int channelIndex)
{
    return makeChannelHistogramTitle(legendPrefix, planeIndex, channelIndex) + legendSuffix;
}

bool LightAnalyzer::isRecHitValid(const CTPPSDiamondRecHit& recHit, const ChannelKey& recHitKey)
{
    return ((recHit.getOOTIndex() == (int)((diamondDetector.GetSPCMap())[recHitKey].offset / 25)) || validOOT == -1) && !recHit.getMultipleHits();
}

void LightAnalyzer::performTimingAnalysis(const std::vector<bool>& sectorsToAnalyze)
{
    auto localTrackMap = diamondDetector.GetDiamondTrack_map();
    for (const auto& localTrack : localTrackMap) {
        int sectorNumber = getSectorNumberFromLocalTrack(localTrack);
		
		if (sectorsToAnalyze[sectorNumber] && localTrack.second.size() != 0) {            
            double trackTime = 0.0;
            double trackPrecision = TRACK_PRECISION_START_VALUE;

            std::vector<std::pair<ChannelKey, CTPPSDiamondRecHit>>::const_iterator hitIterator;
            for (hitIterator = localTrack.second.begin(); hitIterator < localTrack.second.end(); hitIterator++) // hits in track loop localTrack.second = std::vector<std::pair<ChannelKey,CTPPSDiamondRecHit>> 
            {
                double hitTime = diamondDetector.GetTime((*hitIterator).first.sector, (*hitIterator).first.plane, (*hitIterator).first.channel);              
                if (hitIterator == localTrack.second.begin())
                {			
                    trackTime = hitTime;
                }
                else
                {
                    trackTime = getNextTrackTimeValue(trackTime, trackPrecision, hitTime);
                    trackPrecision = getNextTrackPrecisionValue(trackPrecision);
                }	
            }

            TrackTimevsLSSectorHistograms[sectorNumber]->Fill(lumiSection, trackTime);
            TrackTimevsBXSectorHistograms[sectorNumber]->Fill(bunchCrossing, trackTime);
        }
    }
}

int LightAnalyzer::getSectorNumberFromLocalTrack(const std::pair<const CTPPSDiamondLocalTrack, std::vector<std::pair<ChannelKey, CTPPSDiamondRecHit>>>& localTrack)
{
    return localTrack.first.getZ0() > 0.0 ? SECTOR_45 : SECTOR_56;
}

double LightAnalyzer::getNextTrackTimeValue(double trackTime, double trackPrecision, double hitTime)
{
    return (trackTime * pow(trackPrecision, -2) + hitTime * 1.0) / (pow(trackPrecision, -2) + 1.0);
}

double LightAnalyzer::getNextTrackPrecisionValue(double trackPrecision)
{
    return pow((pow(trackPrecision, -2) + 1.0), -0.5);
}

// ------------ method called once each job just before starting event loop  ------------
void LightAnalyzer::beginJob()
{
    sectorDirectories[SECTOR_45] = fs->mkdir(SECTOR_45_HISTOGRAM_PATH);
    sectorDirectories[SECTOR_56] = fs->mkdir(SECTOR_56_HISTOGRAM_PATH);

    for (int sectorNumber = 0; sectorNumber < SECTORS_NUMBER; sectorNumber++) {
        TOTvsLSSectorHistograms[sectorNumber] = sectorDirectories[sectorNumber].make<TH2F>(
            makeSectorHistogramTitle(TOT_VS_LS_HISTOGRAM_NAME, sectorNumber).c_str(),
            makeSectorHistogramLegend(TOT_VS_LS_HISTOGRAM_NAME, TOT_VS_LS_HISTOGRAM_LEGEND_SUFFIX, sectorNumber).c_str(),
            LS_BINS, LS_MIN, LS_MAX, TOT_BINS, TOT_MIN, TOT_MAX);
        
        TrackTimevsLSSectorHistograms[sectorNumber] = sectorDirectories[sectorNumber].make<TH2F>(
            makeSectorHistogramTitle(TRACK_TIME_VS_LS_HISTOGRAM_NAME, sectorNumber).c_str(),
            makeSectorHistogramLegend(TRACK_TIME_VS_LS_HISTOGRAM_NAME, TRACK_TIME_VS_LS_HISTOGRAM_LEGEND_SUFFIX, sectorNumber).c_str(),
            LS_BINS, LS_MIN, LS_MAX, TRACK_TIME_BINS, TRACK_TIME_MIN, TRACK_TIME_MAX
        );

        TrackTimevsBXSectorHistograms[sectorNumber] = sectorDirectories[sectorNumber].make<TH2F>(
            makeSectorHistogramTitle(TRACK_TIME_VS_BX_HISTOGRAM_NAME, sectorNumber).c_str(),
            makeSectorHistogramLegend(TRACK_TIME_VS_BX_HISTOGRAM_NAME, TRACK_TIME_VS_BX_HISTOGRAM_LEGEND_SUFFIX, sectorNumber).c_str(),
            BX_BINS, BX_MIN, BX_MAX, TRACK_TIME_BINS, TRACK_TIME_MIN, TRACK_TIME_MAX
        );
    }
}

std::string LightAnalyzer::makeSectorHistogramTitle(const std::string& titlePrefix, int sectorIndex)
{
    return titlePrefix + " sector " + std::to_string(sectorIndex);
}

std::string LightAnalyzer::makeSectorHistogramLegend(const std::string& legendPrefix, const std::string& legendSuffix, int sectorIndex)
{
    return makeSectorHistogramTitle(legendPrefix, sectorIndex) + legendSuffix;
}

// ------------ method called once each job just after ending the event loop  ------------
void LightAnalyzer::endJob()
{
    makeSectorProfiles();
    makeChannelProfiles();
}

void LightAnalyzer::makeSectorProfiles()
{
    for (int sectorNumber = 0; sectorNumber < SECTORS_NUMBER; sectorNumber++) {
        TOTvsLSSectorProfiles[sectorNumber] = sectorDirectories[sectorNumber].make<TProfile>(*TOTvsLSSectorHistograms[sectorNumber]->ProfileX("_pfx", 1, -1));
        TrackTimevsLSSectorProfiles[sectorNumber] = sectorDirectories[sectorNumber].make<TProfile>(*TrackTimevsLSSectorHistograms[sectorNumber]->ProfileX("_pfx", 1, -1));
        TrackTimevsBXSectorProfiles[sectorNumber] = sectorDirectories[sectorNumber].make<TProfile>(*TrackTimevsBXSectorHistograms[sectorNumber]->ProfileX("_pfx", 1, -1));
    }
}

void LightAnalyzer::makeChannelProfiles()
{
    for (int sectorNumber = 0; sectorNumber < SECTORS_NUMBER; sectorNumber++) {
        for (int planeNumber = 0; planeNumber < PLANES_NUMBER; planeNumber++) {
            for (int channelNumber = 0; channelNumber < CHANNELS_NUMBER; channelNumber++) {
                ChannelKey channelKey(sectorNumber, planeNumber, channelNumber);

                if (channelDirectories.find(channelKey) != channelDirectories.end()) {
                    TOTvsLSChannelProfiles[channelKey] = channelDirectories[channelKey].make<TProfile>(*TOTvsLSChannelHistograms[channelKey]->ProfileX("_pfx", 1, -1));
                }
            }
        }
    }
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void LightAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
 
    //Specify that only 'tracks' is allowed
    //To use, remove the default given above and uncomment below
    //ParameterSetDescription desc;
    //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
    //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(LightAnalyzer);
