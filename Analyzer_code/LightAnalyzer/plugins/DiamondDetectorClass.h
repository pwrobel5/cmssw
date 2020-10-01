#ifndef BOSS__DIAMONDDETECTORCLASS__LIBGUARD
#define BOSS__DIAMONDDETECTORCLASS__LIBGUARD

// system include files
#include <memory> 

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// user include files
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/CTPPSDigi/interface/CTPPSDiamondDigi.h"
#include "DataFormats/CTPPSDetId/interface/CTPPSDiamondDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondRecHit.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondLocalTrack.h"

#include "TF1.h"
 
#include <map>
#include <math.h>
		
struct ChannelKey
{
	int sector, plane, channel;
	
	bool operator < (const ChannelKey &rhs) const {
		return std::tie(this->sector, this->plane, this->channel) < std::tie(rhs.sector, rhs.plane, rhs.channel);
	}
	
	ChannelKey(const int sector = -1, const int plane = -1, const int channel = -1): 
		sector(sector), 
		plane(plane), 
		channel(channel)
	{};
};

typedef std::map<CTPPSDiamondLocalTrack, std::vector<std::pair<ChannelKey,CTPPSDiamondRecHit>>> LocalTrack_map_type;
		
class DiamondDetectorClass {	
   	public:
		explicit DiamondDetectorClass(const edm::ParameterSet&, edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondRecHit> >, edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondLocalTrack> > );
		~ DiamondDetectorClass();
		
		void ExtractData(const edm::Event&);
			
		inline bool PadActive(int sector, int plane, int channel)
			{ return (RecHit_map.find(ChannelKey(sector,plane,channel)) != RecHit_map.end()); }
			
		inline double GetTime(int sector, int plane, int channel)
			{ return RecHit_map[ChannelKey(sector,plane,channel)].at(0).getT(); }
			
		inline double GetToT(int sector, int plane, int channel)
			{ return RecHit_map[ChannelKey(sector,plane,channel)].at(0).getToT(); }
		
		inline LocalTrack_map_type GetDiamondTrack_map()
			{ return LocalTrack_map; }

   	private:		
		static const int CHANNELS_X_PLANE = 12;
		static const int PLANES_X_DETECTOR = 4;
		static const int MAX_SECTOR_NUMBER = 2;

		// ---------- objects to retrieve ---------------------------
		edm::EDGetTokenT<edm::DetSetVector<CTPPSDiamondRecHit>> tokenRecHit;
		edm::EDGetTokenT<edm::DetSetVector<CTPPSDiamondLocalTrack>> tokenLocalTrack;

		// ---------- data extracted---------------------------

		std::map<ChannelKey, std::vector<CTPPSDiamondRecHit>> RecHit_map;
		LocalTrack_map_type  LocalTrack_map;
		
		// event status flags		
		int valid_OOT;	
};

#endif