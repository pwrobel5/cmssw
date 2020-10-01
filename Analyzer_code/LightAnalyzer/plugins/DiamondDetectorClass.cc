#include "DiamondDetectorClass.h"

namespace pt = boost::property_tree;

DiamondDetectorClass::DiamondDetectorClass(const edm::ParameterSet& iConfig, edm::EDGetTokenT<edm::DetSetVector<CTPPSDiamondRecHit>> tokenRecHit_input, edm::EDGetTokenT<edm::DetSetVector<CTPPSDiamondLocalTrack>> tokenLocalTrack_input):
	tokenRecHit(tokenRecHit_input),
	tokenLocalTrack(tokenLocalTrack_input),
	valid_OOT(iConfig.getParameter<int>("tagValidOOT"))
{
}

DiamondDetectorClass::~DiamondDetectorClass() {}

void DiamondDetectorClass::ExtractData(const edm::Event& iEvent)
{
	edm::Handle<edm::DetSetVector<CTPPSDiamondRecHit>> timingRecHit;
	edm::Handle<edm::DetSetVector<CTPPSDiamondLocalTrack>> LocalTracks;
	iEvent.getByToken(tokenRecHit, timingRecHit);
	iEvent.getByToken(tokenLocalTrack, LocalTracks);
	
	RecHit_map.clear();
	LocalTrack_map.clear();
	
    //extract timing  tracks	
	for (const auto& locTracks : *LocalTracks) //rechits = array of hits in one channel
	{
		for (const auto& locTrack : locTracks)
		{
			if ((valid_OOT == 2) && (locTrack.getOOTIndex() != valid_OOT) && (locTrack.getOOTIndex() != valid_OOT + 1)) continue;
			if (valid_OOT != 2 && (locTrack.getOOTIndex() != valid_OOT) && valid_OOT != -1) continue;
			
			LocalTrack_map.insert(std::make_pair(locTrack,std::vector<std::pair<ChannelKey,CTPPSDiamondRecHit>>()));			
		}
	}
  
    // extract reco hit  
	for (const auto& recHits : *timingRecHit) //rechits = array of hits in one channel
	{
		const CTPPSDiamondDetId detId(recHits.detId());
		
		// retrieve and order all events in map.
		for (const auto& recHit : recHits) //rechit
		{   
			if ((valid_OOT != -1) || recHit.getMultipleHits()) continue;
			
			//put in hit map ("select hit with valid leading time and TOT")			
			if (recHit.getT() != 0.0 && recHit.getToT() > 0.0) 
			{
				RecHit_map[ChannelKey(detId.arm(), detId.plane(), detId.channel())].push_back(recHit);
			
				for (auto& locTrack_mapIter : LocalTrack_map) 
				{
					if (locTrack_mapIter.first.containsHit(recHit, 0.1))
					{
						locTrack_mapIter.second.push_back(std::make_pair(ChannelKey(detId.arm(), detId.plane(), detId.channel()), recHit));
					}
				}			
			}
		}
	}   
}