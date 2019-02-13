
#include "DiamondDetectorClass.h"



namespace pt = boost::property_tree;

DiamondDetectorClass::DiamondDetectorClass(const edm::ParameterSet& iConfig, edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondRecHit> > tokenRecHit_input, edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondLocalTrack> > tokenLocalTrack_input)
:
SPC_mapV_(Correction_Map:: create_correction_mapV()),
tokenRecHit_          ( tokenRecHit_input),
tokenLocalTrack_          ( tokenLocalTrack_input),
RecHit_mapV_ (2),
LocalTrack_mapV_ (2),
saturationV_ (2),
Calib_input_path_ (iConfig.getParameter< edm::FileInPath >( "tagCalibrationFile" ))
{
	CalibFunc = new TF1("myFermi","[0]/(exp((x-[1])/[2])+1)+[3]",0,30);
	

	// Create a root
	pt::ptree root;
	// Load the json file in this ptree
	pt::read_json(Calib_input_path_.fullPath().c_str(), root);	
	this-> ExtractCalibrations(root);

}

void DiamondDetectorClass::ExtractCalibrations(pt::ptree root)
{



	

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
	
	
	
	RecHit_mapV_[SECTOR_45_ID].clear();
	RecHit_mapV_[SECTOR_56_ID].clear();
	LocalTrack_mapV_[SECTOR_45_ID].clear();
	LocalTrack_mapV_[SECTOR_56_ID].clear();
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
			if (locTrack.getZ0() > 0.0 )   //yes, positive side is 45...
				LocalTrack_mapV_[SECTOR_45_ID].insert(std::make_pair(locTrack,std::vector<std::pair<RecHit_info,CTPPSDiamondRecHit>>()));
			else
				LocalTrack_mapV_[SECTOR_56_ID].insert(std::make_pair(locTrack,std::vector<std::pair<RecHit_info,CTPPSDiamondRecHit>>()));
				
			
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
		
			Mux_map_[std::make_pair(detId.arm(),detId.plane())]++;
			
			//put in hit map ("select hit with valid leading time and TOT")
			
			if(recHit.getT()!=0.0 && recHit.getToT()!= 0.0) 
			{
				RecHit_mapV_[detId.arm()][std::make_pair(detId.plane(),detId.channel())].push_back(recHit);
				Mux_validT_map_[std::make_pair(detId.arm(),detId.plane())]++;
				
				bool counted = false;
			
				for (auto& locTrack_mapIter : LocalTrack_mapV_[detId.arm()]) 
				{
					if (locTrack_mapIter.first.containsHit(recHit, 0.1))
					{
						locTrack_mapIter.second.push_back(std::make_pair((RecHit_info) {detId.plane(),detId.channel()},recHit));
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
			if (RecHit_mapV_[sector_id].find( std::make_pair(PLANE_2_ID,ch_number)) != RecHit_mapV_[sector_id].end() && RecHit_mapV_[sector_id].find( std::make_pair(PLANE_3_ID,ch_number)) != RecHit_mapV_[sector_id].end())
			{	 
				if (RecHit_mapV_[sector_id][std::make_pair(PLANE_2_ID,ch_number)].at(0).getToT() > 15 && RecHit_mapV_[sector_id][std::make_pair(PLANE_3_ID,ch_number)].at(0).getToT() > 15) // double saturated
				{	
					saturationV_[sector_id] = 1;
					//std::cout << "saturated!!" << std::endl;
				}
					
			} 
		}
	
	//std::cout << RecHit_mapV_[sector_id].size() << std::endl;
	
	}
}

double DiamondDetectorClass::GetTime_SPC(int sector, int plane, int channel)
{
	double Time_raw = RecHit_mapV_[sector][std::make_pair(plane, channel)].at(0).getT();
	double ToT = RecHit_mapV_[sector][std::make_pair(plane, channel)].at(0).getToT();
	
	//TF1 *myfermi = new TF1("myFermi","[0]/(exp((x-[1])/[2])+1)+[3]",0,30);
	
	double P0 = SPC_mapV_[sector][std::make_pair(plane,channel)].at(2);
	double P1 = SPC_mapV_[sector][std::make_pair(plane,channel)].at(3);
	double P2 = SPC_mapV_[sector][std::make_pair(plane,channel)].at(4);
	double P3 = SPC_mapV_[sector][std::make_pair(plane,channel)].at(5);
	
	CalibFunc -> SetParameters(P0,P1,P2,P3);
	
	double Time_SPC = Time_raw - ( CalibFunc-> Eval(ToT) ); 
	
	//double Time_SPC = Time_raw - (C0 + C1*ToT + C2*ToT*ToT + C3*ToT*ToT*ToT);
 
	return Time_SPC;
	
	
}

int DiamondDetectorClass::GetSpread(int sector, int plane)
{
	
	int min=13, max=-1;
	for (const auto& rechit : RecHit_mapV_[sector])
	{
		int rh_plane = rechit.first.first;
		if (rh_plane == plane)
		{
			int rh_channel = Ch_position[rechit.first.second]; 
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
	for (const auto& locTrack_mapIter : LocalTrack_mapV_[sector]) //loop on track	
	{	
		for (const auto&  HitInfo_Hit_pair_iter : locTrack_mapIter.second)
		{
			int rh_plane = HitInfo_Hit_pair_iter.first.plane;
			if (rh_plane == plane)
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








