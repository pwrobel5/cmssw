
#include "DiamondDetectorClass.h"



namespace pt = boost::property_tree;

DiamondDetectorClass::DiamondDetectorClass(const edm::ParameterSet& iConfig, edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondRecHit> > tokenRecHit_input, edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondLocalTrack> > tokenLocalTrack_input)
:

tokenRecHit_          ( tokenRecHit_input),
tokenLocalTrack_          ( tokenLocalTrack_input),
saturationV_ (2),
valid_OOT_ (iConfig.getParameter< int >( "tagValidOOT" ))
{
	CalibFunc = new TF1("myFermi","[0]/(exp((x-[1])/[2])+1)+[3]",0,30);
}

DiamondDetectorClass::~ DiamondDetectorClass()
{
}


void DiamondDetectorClass::ExtractData(const edm::Event& iEvent)
{
	
	
	edm::Handle< edm::DetSetVector<CTPPSDiamondRecHit> > timingRecHit;
	edm::Handle< edm::DetSetVector<CTPPSDiamondLocalTrack> > LocalTracks;
	iEvent.getByToken( tokenRecHit_, timingRecHit );
	iEvent.getByToken( tokenLocalTrack_, LocalTracks );
	
	
	
	RecHit_map_.clear();
	RecHit_map_.clear();
	LocalTrack_map_.clear();
	LocalTrack_map_.clear();
	saturationV_[0]=0;
	saturationV_[1]=0;
	Mux_map_.clear();
	Mux_validT_map_.clear();
	Mux_inTrack_map_.clear();
	
	
	
	
    //extract timing  tracks
	
	for (const auto& locTracks : *LocalTracks) //rechits = array of hits in one channel
	{
		for (const auto& locTrack : locTracks)
		{
			if((valid_OOT_==2) && (locTrack.getOOTIndex() != valid_OOT_) && (locTrack.getOOTIndex() != valid_OOT_+1)) continue;
			if (valid_OOT_!=2 && (locTrack.getOOTIndex() != valid_OOT_) &&  valid_OOT_!=-1 ) continue;
			
			LocalTrack_map_.insert(std::make_pair(locTrack,std::vector<std::pair<ChannelKey,CTPPSDiamondRecHit>>()));
				
			
		}
	}
				//	std::cout <<  " track found in 45 = " << LocalTrack_mapV_[0].size() << std::endl;
  
    // extract reco hit
  
	for (const auto& recHits : *timingRecHit) //rechits = array of hits in one channel
	{
		const CTPPSDiamondDetId detId( recHits.detId() );
		
		// retrieve and order all events in map. 
		
		for (const auto& recHit : recHits ) //rechit
		{
			//std::cout << "Hits in channel " << detId.channel() << ", plane " << detId.plane() << ", rp " << detId.rp()<< ", station " << detId.station()<< ", arm " << detId.arm() << std::endl;
					    
			
			if ((valid_OOT_!=-1) ||  recHit.getMultipleHits()) continue;
			
			
			Mux_map_[std::make_pair(detId.arm(),detId.plane())]++;
			
			//put in hit map ("select hit with valid leading time and TOT")
			
			if(recHit.getT()!=0.0 && recHit.getToT()> 0.0) 
			{
				RecHit_map_[ChannelKey(detId.arm(),detId.plane(),detId.channel())].push_back(recHit);
				Mux_validT_map_[std::make_pair(detId.arm(),detId.plane())]++;
				
				bool counted = false;
			
				for (auto& locTrack_mapIter : LocalTrack_map_) 
				{
					if (locTrack_mapIter.first.containsHit(recHit, 0.1))
					{
						locTrack_mapIter.second.push_back(std::make_pair(ChannelKey(detId.arm(),detId.plane(),detId.channel()),recHit));
						if (!counted)
						{
							Mux_inTrack_map_[std::make_pair(detId.arm(),detId.plane())]++;
							counted=true;
						}
						//std::cout << "hit of sector "<<detId.arm() << " assigned to track with z= " << locTrack_mapIter.first.getZ0() << std::endl;
					}
				}
			
			}	
		
		}
	}
				//	std::cout <<  " track found in 45 = " << LocalTrack_mapV_[0].size() << std::endl;
  
	//check for saturation
  
	for (int sector_id=0; sector_id <2; sector_id++)
	{		
		for  (int ch_number=0; ch_number < CHANNELS_X_PLANE; ch_number++)
		{
			if (RecHit_map_.find(ChannelKey(sector_id,PLANE_2_ID,ch_number)) != RecHit_map_.end() && RecHit_map_.find(ChannelKey(sector_id,PLANE_3_ID,ch_number)) != RecHit_map_.end())
			{	 
				if (RecHit_map_[ChannelKey(sector_id,PLANE_2_ID,ch_number)].at(0).getToT() > 15 && RecHit_map_[ChannelKey(sector_id,PLANE_3_ID,ch_number)].at(0).getToT() > 15) // double saturated
				{	
					saturationV_[sector_id] = 1;
					//std::cout << "saturated!!" << std::endl;
				}
					
			} 
		}
	
	//std::cout << RecHit_mapV_[sector_id].size() << std::endl;
	
	}   
}

int DiamondDetectorClass::GetSpread(int sector, int plane)
{
	
	int min=13, max=-1;
	for (const auto& rechit : RecHit_map_)
	{
		int rh_plane = rechit.first.plane;
		int rh_sector = rechit.first.sector;
		if ((rh_plane == plane) && (rh_sector == sector))
		{
			int rh_channel = Ch_position[rechit.first.channel]; 
			if (min > rh_channel) min = rh_channel;
			if (max < rh_channel) max = rh_channel;
		}
	}
	
	
	if ((max-min)>=0) return max-min+1;
	else return 0;
	
	
}

int DiamondDetectorClass::GetTrackSpread(int sector, int plane)
{
	
	int min=13, max=-1;
	for (const auto& locTrack_mapIter : LocalTrack_map_) //loop on track	
	{	
		for (const auto&  HitInfo_Hit_pair_iter : locTrack_mapIter.second)
		{
			int rh_plane = HitInfo_Hit_pair_iter.first.plane;
			int rh_sector = HitInfo_Hit_pair_iter.first.sector;
			if ((rh_plane == plane) && (rh_sector == sector))
			{
				int rh_channel = Ch_position[HitInfo_Hit_pair_iter.first.channel]; 
				if (min > rh_channel) min = rh_channel;
				if (max < rh_channel) max = rh_channel;
			}
		}
	}
	
	
	if ((max-min)>=0) return max-min+1;
	else return 0;
	
	
}


int DiamondDetectorClass::GetTrackMuxInSector(int sector)
{
	int mux=0;
	for (const auto& locTrack_mapIter : LocalTrack_map_) //loop on track	
	{	
		if (((locTrack_mapIter.first.getZ0() > 0.0 ) && sector==SECTOR_45_ID) ||
		    ((locTrack_mapIter.first.getZ0() < 0.0 ) && sector==SECTOR_56_ID))
			mux++;		
	}

	return mux;	
	
}

void DiamondDetectorClass::DumpEvent()
{
	for (const auto& recHits : RecHit_map_) //rechits = array of hits in one channel
	{
		std::cout << "Hit in sector " << recHits.first.sector << ", plane " << recHits.first.plane << ", channel " << recHits.first.channel << 
					 " : T " << recHits.second.at(0).getT() << ", TOT " << recHits.second.at(0).getToT() << std::endl;
	}
}

