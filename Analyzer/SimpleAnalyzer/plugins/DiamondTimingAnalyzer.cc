// system include files
#include <memory> 

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
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


#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"
#include "DiamondDetectorClass.h"

#include <map>

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TF1.h"
#include "TProfile.h"
#include "TTree.h"
#include "TBranch.h"

Double_t FermiDirac(Double_t *x, Double_t *par) {

  Double_t value=0.;  
  value=(par[0]/(exp((x[0]-par[1])/par[2])+1))+par[3];
    
  return value;
}

class DiamondTimingAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
	
	typedef std::map< std::pair<int,int> , TH2F* >  TH2F_map;
	typedef std::map< std::pair<int,int> , TH1F* >  TH1F_map;
	
	
   public:
      explicit DiamondTimingAnalyzer(const edm::ParameterSet&);
      ~ DiamondTimingAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
   
   
   
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
	  
      virtual void endJob() override;

      void initHistograms( const CTPPSDiamondDetId&);

      // ---------- constants ---------------------------
      static const int CHANNELS_X_PLANE=12;
      static const int PLANES_X_DETECTOR=4;
      static const int MAX_SECTOR_NUMBER=2;
	  edm::Service<TFileService> fs_;
	  
	enum Sector_id_
	{
		SECTOR_45_ID,
		SECTOR_56_ID
    };
	  
	enum Plane_id_
	{
		PLANE_0_ID,
		PLANE_1_ID,
		PLANE_2_ID,
		PLANE_3_ID
		
    };
	  
	enum Station_id_
	{
		STATION_210_M_ID,
		STATION_TIMING_ID,
		STATION_220_M_ID
		
    };

      // ---------- objects to retrieve ---------------------------
      edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondDigi> > tokenDigi_;
      edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondRecHit> > tokenRecHit_;
      edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondLocalTrack> > tokenLocalTrack_;
      edm::EDGetTokenT< edm::DetSetVector<CTPPSPixelLocalTrack> > tokenPixelLocalTrack_;

      // ---------- directories ---------------------------
      std::map< CTPPSDiamondDetId, TFileDirectory > mainDir_map_;
      std::map< CTPPSDiamondDetId, TFileDirectory > planeDir_map_;
      std::map< CTPPSDiamondDetId, TFileDirectory > channelDir_map_;
	  
	  std::vector<TFileDirectory> dir_cyl_V ;	
	  std::vector<TFileDirectory> dir_cyl_DDTI_V ;
	  std::vector < std::map<std::pair<int,int>, TFileDirectory > > dir_cyl_TPTI_V ;
	  std::vector < std::vector<TFileDirectory> > dir_plane_VV;
	  

      // ---------- global histograms ---------------------------
	  
      std::map< CTPPSDiamondDetId, TH1F*> T_Histo_map_;
      std::map< CTPPSDiamondDetId, TH1F*> TOT_Histo_map_;
      std::map< CTPPSDiamondDetId, TH1F*> ValidT_Histo_map_;
      std::map< CTPPSDiamondDetId, TH1F*> ValidTOT_Histo_map_;
      std::map< CTPPSDiamondDetId, TH2F*> TOTvsT_Histo_map_;
      std::map< CTPPSDiamondDetId, TProfile*> TOTvsT_Profile_map_;
	  
      std::map< CTPPSDiamondDetId, TH1F*> SPCT_Histo_map_;
      std::map< CTPPSDiamondDetId, TH2F*> TOTvsSPCT_Histo_map_;
	  
	  
      // ---------- correlation histograms ---------------------------
	  
      TH1F_map Raw_DT_Hmap_;  //sector, channel
      TH1F_map SPC_DT_Hmap_;  //sector, channel
      TH2F_map ToT_vs_ToT_Hmap_; //plane, channel
	  
      std::vector<std::map< std::pair<RecHit_info,RecHit_info> , TH1F* >> Raw_DT_TrackedPlane_Hmap_;  //[sector] <<recHitInfo1,recHitInfo2>,histogram>
      std::vector<std::map< std::pair<RecHit_info,RecHit_info> , TH1F* >> SPC_DT_TrackedPlane_Hmap_;  //[sector] <<recHitInfo1,recHitInfo2>,histogram>
      std::vector<std::map< std::pair<RecHit_info,RecHit_info> , TH2F* >> ToT_vs_ToT_TrackedPlane_Hmap_; //[sector] <<recHitInfo1,recHitInfo2>,histogram>
	  
      TH2F_map TimingPixel_mux_220_Hmap_; //arm, plane
      TH2F_map TimingPixel_mux_210_Hmap_; //arm, plane
      TH2F_map TimingPixel_Saturated_mux_220_Hmap_; //arm, plane
      TH2F_map TimingPixel_Saturated_mux_210_Hmap_; //arm, plane
	  
      std::map<int, TH2F*>  TimingTrack_Pixel_mux_220_Hmap_; //arm, plane
      std::map<int, TH2F*>  TimingTrack_Pixel_mux_210_Hmap_; //arm, plane
      std::map<int, TH2F*>  TimingTrack_Pixel_Saturated_mux_220_Hmap_; //arm, plane
      std::map<int, TH2F*>  TimingTrack_Pixel_Saturated_mux_210_Hmap_; //arm, plane
	  
      TH2F_map MuxSpread_corr_Hmap_; //arm, plane
	  
      TH2F_map TrackMuxSpread_corr_Hmap_; //arm
	  
	  
      std::vector < TH2F_map > TimingCorrelation_mux_Hmap_V_; //plane 1, plane 2
	  
	  
	  
      TH1F_map Plane_Mux_Hmap_; //arm, plane
      TH1F_map Plane_Mux_validT_Hmap_; //arm, plane
      TH1F_map Plane_Mux_saturated_Hmap_; //arm, plane
      TH1F_map Plane_Mux_saturated_validT_Hmap_; //arm, plane
	  
      TH1F_map Track_Plane_Mux_validT_Hmap_; //arm, plane
      TH1F_map Track_Plane_Mux_saturated_validT_Hmap_; //arm, plane
	  
	  

      std::map<int, TH1F*> PadInTracks_Reco_Hmap_; //arm	  
      std::map<int, TH1F*> TimingTracks_Mux_Hmap_; //arm
      std::map<int, TH1F*> TimingTracks_Mux_saturated_Hmap_; //arm
	  
      TH2F_map Pixel_Hit_Hmap_; //arm, station
      TH2F_map Pixel_Tomography_Hmap_; //arm, station
      std::map<int, TH2F*>  TimingTrack_Hit_Hmap_; //arm
	  
      // ---------- pixel mux map ---------------------------
	   
      std::map< std::pair< int , int >, int> Pixel_Mux_map_; //arm, station
	  
      // ---------- channel graphs ---------------------------
	  
      std::vector <TGraph*> Raw_resolution_DD_V_;
      std::vector <TGraph*> SPC_resolution_DD_V_;
      std::vector <TGraph*> Saturated_percentage_V_;
      TH1F_map ValidToT_mean_Hmap_; // arm,plane
      std::vector <TH2F_map> Raw_resolution_TP_V_; // sector, <pl 1, pl 2>
      std::vector <TH2F_map> SPC_resolution_TP_V_;
      std::vector <TH2F_map> Correlation_TP_V_;
	  
	  std::vector<std::map<RecHit_info,double>> Resolution_L0_store_;
	  
	  TTree  *TCalibration_;
	  int cal_channel_;
	  int cal_plane_;
	  int cal_arm_;
	  double cal_par_0_;
	  double cal_par_1_;
	  double cal_par_2_;
	  double cal_par_3_;
	  double Chi_2_;
	  double resolution_L0_;
	  
	  //external 
	  DiamondDetectorClass DiamondDet;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
DiamondTimingAnalyzer:: DiamondTimingAnalyzer(const edm::ParameterSet& iConfig)
 : 
 tokenDigi_            ( consumes< edm::DetSetVector<CTPPSDiamondDigi> >      ( iConfig.getParameter<edm::InputTag>( "tagDigi" ) ) ),
 tokenRecHit_          ( consumes< edm::DetSetVector<CTPPSDiamondRecHit> >      ( iConfig.getParameter<edm::InputTag>( "tagRecHit" ))),
 tokenLocalTrack_          ( consumes< edm::DetSetVector<CTPPSDiamondLocalTrack> >      ( iConfig.getParameter<edm::InputTag>( "tagLocalTrack" ))),
 tokenPixelLocalTrack_      ( consumes< edm::DetSetVector<CTPPSPixelLocalTrack> >      ( iConfig.getParameter<edm::InputTag>( "tagPixelLocalTrack" ))),
 Raw_DT_TrackedPlane_Hmap_(2),
 SPC_DT_TrackedPlane_Hmap_(2),
 ToT_vs_ToT_TrackedPlane_Hmap_(2),
 TimingCorrelation_mux_Hmap_V_(2),
 Raw_resolution_DD_V_(2),
 SPC_resolution_DD_V_(2),
 Saturated_percentage_V_(2),
 Raw_resolution_TP_V_(2),
 SPC_resolution_TP_V_(2),
 Correlation_TP_V_(2),
 Resolution_L0_store_(2),
 DiamondDet(iConfig,tokenRecHit_,tokenLocalTrack_)
{
  usesResource("TFileService");
}


DiamondTimingAnalyzer::~ DiamondTimingAnalyzer()
{}


//
// member functions
//

void
DiamondTimingAnalyzer::initHistograms(const CTPPSDiamondDetId& detId)
{
  //now do what ever initialization is needed
  if (mainDir_map_.find(detId.getRPId()) == mainDir_map_.end())
  {
	std::cout << "RP is new, creating directory and histos" << std::endl;
    std::string dirName;
    detId.rpName(dirName, CTPPSDiamondDetId::nPath);
    std::string rpName;
    detId.rpName(rpName, CTPPSDiamondDetId::nFull);

    // create directory for the detector, if not already done
    mainDir_map_[ detId.getRPId() ] = fs_->mkdir( dirName );
	


   }

   if (planeDir_map_.find( detId.getPlaneId() ) == planeDir_map_.end())
   {
	  std::cout << "Plane is new, creating directory and histos" << std::endl;
      std::string dirName;
      detId.planeName(dirName, CTPPSDiamondDetId::nPath);
      std::string plName;
      detId.planeName(plName, CTPPSDiamondDetId::nFull);

      // create directory for the plane, if not already done
      planeDir_map_[ detId.getPlaneId() ] = fs_->mkdir( dirName );
   } 
    //create channels histograms

   if (channelDir_map_.find( detId ) == channelDir_map_.end())
   {
	  std::cout << "Channels is new, creating directory and histos" << std::endl;
      std::string dirName;
      detId.channelName(dirName, CTPPSDiamondDetId::nPath);
      std::string chName;
      detId.channelName(chName, CTPPSDiamondDetId::nFull);

      // create directory for the channel, if not already done
      channelDir_map_[ detId ] = fs_->mkdir( dirName );
	  
	  
	  
	 // create all histograms
    std::string T_Histo_name(chName);
    T_Histo_name.insert(0, "RAW_T_Distribution_");
    T_Histo_map_[ detId ] = channelDir_map_[ detId ].make<TH1F>(T_Histo_name.c_str(), T_Histo_name.c_str(), 1200, -60, 60 );
    std::string ValidT_Histo_name(chName);
    ValidT_Histo_name.insert(0, "VALID_RAW_T_Distribution_");
    ValidT_Histo_map_[ detId ] = channelDir_map_[ detId ].make<TH1F>(ValidT_Histo_name.c_str(), ValidT_Histo_name.c_str(), 1200, -60, 60 );
    
	std::string SPCT_Histo_name(chName);
    SPCT_Histo_name.insert(0, "SinglePadCorr_T_Distribution_");
    SPCT_Histo_map_[ detId ] = channelDir_map_[ detId ].make<TH1F>(SPCT_Histo_name.c_str(), SPCT_Histo_name.c_str(), 1200, -60, 60 );

    std::string TOT_Histo_name(chName);
    TOT_Histo_name.insert(0, "TOT_Distribution_");
    TOT_Histo_map_[ detId ] = channelDir_map_[ detId ].make<TH1F>(TOT_Histo_name.c_str(), TOT_Histo_name.c_str(), 100, -20, 20 );
    std::string ValidTOT_Histo_name(chName);
    ValidTOT_Histo_name.insert(0, "ValidTOT_Distribution_");
    ValidTOT_Histo_map_[ detId ] = channelDir_map_[ detId ].make<TH1F>(ValidTOT_Histo_name.c_str(), ValidTOT_Histo_name.c_str(), 100, -20, 20 );

    std::string TOTvsT_Histo_name(chName);
    TOTvsT_Histo_name.insert(0, "TvsTOT_Distribution_");
    TOTvsT_Histo_map_[ detId ] = channelDir_map_[ detId ].make<TH2F>(TOTvsT_Histo_name.c_str(), TOTvsT_Histo_name.c_str(), 240, 0, 60, 400, -20, 20 );

    std::string TOTvsSPCT_Histo_name(chName);
    TOTvsSPCT_Histo_name.insert(0, "TOTvsSPCT_Distribution_");
    TOTvsSPCT_Histo_map_[ detId ] = channelDir_map_[ detId ].make<TH2F>(TOTvsSPCT_Histo_name.c_str(), TOTvsSPCT_Histo_name.c_str(), 240, 0, 60, 400, -20, 20 );


   } 

}

// ------------ method called for each event  ------------
void
DiamondTimingAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  //edm::Handle< edm::DetSetVector<CTPPSDiamondDigi> > timingDigi;
  edm::Handle< edm::DetSetVector<CTPPSDiamondRecHit> > timingRecHit;
  edm::Handle< edm::DetSetVector<CTPPSPixelLocalTrack> > pixelLocalTrack;
  //iEvent.getByToken( tokenDigi_, timingDigi );
  iEvent.getByToken( tokenRecHit_, timingRecHit );
  iEvent.getByToken( tokenPixelLocalTrack_, pixelLocalTrack );
  

  Pixel_Mux_map_.clear();
  DiamondDet.ExtractData(iEvent);
  
 // if ((DiamondDet.GetTrackMux(0)!=1) || (DiamondDet.GetMux(0,2)!=1) || (DiamondDet.GetMux(0,3)!=1)) return;
  
  for (const auto& recHits : *timingRecHit) //rechits = array of hits in one channel
  {
    const CTPPSDiamondDetId detId( recHits.detId() );
	
    if (channelDir_map_.find(detId) == channelDir_map_.end() && recHits.size() > 0)
    {  
		std::cout << "Found new channel with data! Creating Histograms..." << std::endl;
		initHistograms( detId );
	}
	
	// Perform channel histogram
	
    for (const auto& recHit : recHits ) //rechit
    {
		//std::cout << "Hits in channel " << detId.channel() << ", plane " << detId.plane() << ", rp " << detId.rp()<< ", station " << detId.station()<< 
	//", arm " << detId.arm() << std::endl;
	
		// Do stuff on recHits
		
		T_Histo_map_[ detId ]-> Fill( recHit.getT() );
		TOT_Histo_map_[ detId ]-> Fill( recHit.getToT() );
		
		
		if (DiamondDet.PadActive(detId.arm(), detId.plane(),detId.channel())) // T and Tot present
		{
			ValidT_Histo_map_[ detId ]-> Fill( recHit.getT() );
			ValidTOT_Histo_map_[ detId ]-> Fill( recHit.getToT() );
			SPCT_Histo_map_[ detId ]-> Fill( DiamondDet.GetTime_SPC(detId.arm(), detId.plane(),detId.channel()));
			TOTvsSPCT_Histo_map_[ detId ]-> Fill(recHit.getToT(), DiamondDet.GetTime_SPC(detId.arm(),detId.plane(),detId.channel()));
			TOTvsT_Histo_map_[ detId ]-> Fill(recHit.getToT(), DiamondDet.GetTime(detId.arm(), detId.plane(),detId.channel()));
		}
    }
  }
  
  

 ////////////////////////////////////////////////////////////////
//
//		EXTRACT PIXELS TRACK NUMBER
//
///////////////////////////////////////////////////////////////// 
  
  
  
  
   for (const auto& RP_trks : *pixelLocalTrack) //array of tracks
  {
      const CTPPSDetId detId( RP_trks.detId() );
	  
	//	std::cout << "Tracks in arm " << detId.arm() << ", station " << detId.station() << ", rp " << detId.rp() << std::endl;
      
	  for ( const auto& trk : RP_trks ) 
	  {
		if ( !trk.isValid() ) continue;
		//pixelsppslist.push_back( std::pair<const CTPPSPixelLocalTrack*, const CTPPSDetId>(&trk, det_id) );
		//std::cout << "track found in " << trk.getX0() << " , "<< trk.getY0() << std::endl;
		
		Pixel_Hit_Hmap_[ std::make_pair(detId.arm(), detId.station()) ] -> Fill(trk.getX0(),trk.getY0());
		Pixel_Mux_map_[ std::make_pair(detId.arm(), detId.station()) ]++;
      }

	  for ( const auto& trk : RP_trks ) 
	  {
		if ( !trk.isValid() ) continue;
		if ((Pixel_Mux_map_[ std::make_pair(detId.arm(), detId.station()) ]==1) && (DiamondDet.GetTrackMux(detId.arm()) > 0))  
				Pixel_Tomography_Hmap_[ std::make_pair(detId.arm(), detId.station()) ] -> Fill(trk.getX0(),trk.getY0());
		  
      }	  
	  
} 



////////////////////////////////////////////////////////////////
//
//		Multiplicity correlation for all events
//
///////////////////////////////////////////////////////////////// 



   // plane multiplicity correlation
for (int sec_number=0; sec_number < MAX_SECTOR_NUMBER; sec_number++)
{

  	for  (int pl_number=0; pl_number < PLANES_X_DETECTOR; pl_number++)
	{
		for  (int pl_number_2 = pl_number+1; pl_number_2 < PLANES_X_DETECTOR; pl_number_2++)
			
		{
			//multiplicity corralation between planes (all hits)
			TimingCorrelation_mux_Hmap_V_[sec_number][ std::make_pair(pl_number,pl_number_2) ]->Fill(DiamondDet.GetMux(sec_number,pl_number),DiamondDet.GetMux(sec_number,pl_number_2));
		}
		// multiplicity distribution (all hits)
		Plane_Mux_Hmap_[ std::make_pair(sec_number,pl_number) ] -> Fill(DiamondDet.GetMux(sec_number,pl_number));
		// multiplicity distribution (valid time hits)
		Plane_Mux_validT_Hmap_[ std::make_pair(sec_number,pl_number) ] -> Fill(DiamondDet.GetMuxValidT(sec_number,pl_number));
		// hits multiplicity (valid T) Vs Spread (valid T)
		MuxSpread_corr_Hmap_[ std::make_pair(sec_number,pl_number) ] -> Fill(DiamondDet.GetMuxValidT(sec_number,pl_number),DiamondDet.GetSpread(sec_number,pl_number));
		// hits multiplicty (valid T) Vs 220 pixel tracks multiplicity
		TimingPixel_mux_220_Hmap_[std::make_pair(sec_number,pl_number) ]-> Fill(DiamondDet.GetMuxValidT(sec_number,pl_number),Pixel_Mux_map_[std::make_pair(sec_number,STATION_220_M_ID)]); 
		// hits multiplicty (valid T) Vs 210 pixel tracks multiplicity
		TimingPixel_mux_210_Hmap_[std::make_pair(sec_number,pl_number) ]-> Fill(DiamondDet.GetMuxValidT(sec_number,pl_number),Pixel_Mux_map_[std::make_pair(sec_number,STATION_210_M_ID)]);
        // hits multiplicty (in tracks) Vs Spread (in tracks) 		
		TrackMuxSpread_corr_Hmap_[ std::make_pair(sec_number,pl_number) ] -> Fill(DiamondDet.GetMuxInTrack(sec_number,pl_number),DiamondDet.GetTrackSpread(sec_number,pl_number));
        // multiplicity distribution (hits contained in tracks)		
		Track_Plane_Mux_validT_Hmap_[ std::make_pair(sec_number,pl_number) ] -> Fill(DiamondDet.GetMuxInTrack(sec_number,pl_number));
			
	}	
	
	
	for (const auto& LocalTrack_mapIter : DiamondDet.GetDiamondTrack_map(sec_number))
	{
		PadInTracks_Reco_Hmap_[sec_number]->Fill(LocalTrack_mapIter.second.size());
		TimingTrack_Hit_Hmap_[ sec_number ]-> Fill (LocalTrack_mapIter.first.getX0(),LocalTrack_mapIter.first.getY0());

	}	
	TimingTracks_Mux_Hmap_[sec_number]-> Fill(DiamondDet.GetTrackMux(sec_number));
	TimingTrack_Pixel_mux_220_Hmap_[sec_number]-> Fill(DiamondDet.GetTrackMux(sec_number),Pixel_Mux_map_[std::make_pair(sec_number,STATION_220_M_ID)]); 
	TimingTrack_Pixel_mux_210_Hmap_[sec_number]-> Fill(DiamondDet.GetTrackMux(sec_number),Pixel_Mux_map_[std::make_pair(sec_number,STATION_210_M_ID)]); 
	
	
	
	if (DiamondDet.SecIsSaturated(sec_number))
	{
		for  (int pl_number=0; pl_number < PLANES_X_DETECTOR; pl_number++)
			{	
		
				Plane_Mux_saturated_Hmap_[ std::make_pair(sec_number,pl_number) ] -> Fill(DiamondDet.GetMux(sec_number,pl_number));
				Plane_Mux_saturated_validT_Hmap_[ std::make_pair(sec_number,pl_number) ] -> Fill(DiamondDet.GetMuxValidT(sec_number,pl_number));
				Track_Plane_Mux_saturated_validT_Hmap_[ std::make_pair(sec_number,pl_number) ] -> Fill(DiamondDet.GetMuxInTrack(sec_number,pl_number));
				TimingPixel_Saturated_mux_220_Hmap_[std::make_pair(sec_number,pl_number) ]-> Fill(DiamondDet.GetMuxValidT(sec_number,pl_number),Pixel_Mux_map_[std::make_pair(sec_number,STATION_220_M_ID)]); 
				TimingPixel_Saturated_mux_210_Hmap_[std::make_pair(sec_number,pl_number) ]-> Fill(DiamondDet.GetMuxValidT(sec_number,pl_number),Pixel_Mux_map_[std::make_pair(sec_number,STATION_210_M_ID)]); 
					
			}	
			
		TimingTracks_Mux_saturated_Hmap_[sec_number]-> Fill(DiamondDet.GetTrackMux(sec_number));
		TimingTrack_Pixel_Saturated_mux_220_Hmap_[sec_number]-> Fill(DiamondDet.GetTrackMux(sec_number),Pixel_Mux_map_[std::make_pair(sec_number,STATION_220_M_ID)]); 
		TimingTrack_Pixel_Saturated_mux_210_Hmap_[sec_number]-> Fill(DiamondDet.GetTrackMux(sec_number),Pixel_Mux_map_[std::make_pair(sec_number,STATION_210_M_ID)]);
	}
}


////////////////////////////////////////////////////////////////
//
//		TIMING STUDIES
//
///////////////////////////////////////////////////////////////// 

 
	// double diamond analzsis method
	
for (int sec_number=0; sec_number < MAX_SECTOR_NUMBER; sec_number++)
{		
	for  (int ch_number=0; ch_number < CHANNELS_X_PLANE; ch_number++)
	{
		if (DiamondDet.PairActive(sec_number, PLANE_2_ID,ch_number,PLANE_3_ID,ch_number))
		{	 
			Raw_DT_Hmap_[std::make_pair(sec_number,ch_number)]->Fill(DiamondDet.GetTime(sec_number,PLANE_2_ID,ch_number)-DiamondDet.GetTime(sec_number,PLANE_3_ID,ch_number));
			
			
			
			double TimeA_corr=DiamondDet.GetTime_SPC(sec_number, PLANE_2_ID,ch_number);
			double TimeB_corr=DiamondDet.GetTime_SPC(sec_number, PLANE_3_ID,ch_number);
			
			SPC_DT_Hmap_[std::make_pair(sec_number,ch_number)]->Fill(TimeA_corr-TimeB_corr);
			ToT_vs_ToT_Hmap_[std::make_pair(sec_number,ch_number)]->Fill(DiamondDet.GetToT(sec_number, PLANE_2_ID,ch_number),DiamondDet.GetToT(sec_number,PLANE_3_ID,ch_number));
	
		}
	}
	
	
	//typedef std::map< CTPPSDiamondLocalTrack , std::vector<std::pair<RecHit_info,CTPPSDiamondRecHit> > >  LocalTrack_map;
	//std::vector<TH1F_map> Raw_DT_TrackedPlane_Hmap_;  //[sector] <<recHit_info1,recHit_info2>,histogram>
	
	// loop on all tracks --> generation of time resolution timing track based
	for (const auto& LocalTrack_mapIter : DiamondDet.GetDiamondTrack_map(sec_number)) // loop on predigested tracks
	{
		std::map<RecHit_info,CTPPSDiamondRecHit> Hits_in_Track_map;  // plane, <channel, hits>

		std::vector<std::pair<RecHit_info,CTPPSDiamondRecHit>>::const_iterator  hitA_iter;
		std::vector<std::pair<RecHit_info,CTPPSDiamondRecHit>>::const_iterator  hitB_iter;
 		
	
	//std::cout << "Before hit loop: #hit " << LocalTrack_mapIter.second.size()  << std::endl;
		if	(LocalTrack_mapIter.second.size() < 2) continue;
		for (hitA_iter=LocalTrack_mapIter.second.begin();hitA_iter < LocalTrack_mapIter.second.end()-1; hitA_iter++ ) // hits in track loop LocalTrack_mapIter.second = std::vector<std::pair<RecHit_info,CTPPSDiamondRecHit> > 
		{
			
			for (hitB_iter=hitA_iter+1; hitB_iter < LocalTrack_mapIter.second.end(); hitB_iter++ ) // hits in track loop LocalTrack_mapIter.second = std::vector<std::pair<RecHit_info,CTPPSDiamondRecHit> > 
			{
				if ((*hitA_iter).first.plane == (*hitB_iter).first.plane) continue;
				
				if (Raw_DT_TrackedPlane_Hmap_[sec_number].find(std::make_pair((*hitA_iter).first, (*hitB_iter).first))==Raw_DT_TrackedPlane_Hmap_[sec_number].end()) // creta pair histos if do not exist
						{
								std::string Histo_name = "Raw_resolution_TP_sec_"+std::to_string(sec_number)+"_planes_"+std::to_string((*hitA_iter).first.plane)+"_"+std::to_string((*hitB_iter).first.plane)+
									"_channels_"+std::to_string((*hitA_iter).first.channel)+"_"+std::to_string((*hitB_iter).first.channel);
								Raw_DT_TrackedPlane_Hmap_[sec_number][std::make_pair((*hitA_iter).first, (*hitB_iter).first)] = 
									dir_cyl_TPTI_V[sec_number][std::make_pair((*hitA_iter).first.plane,(*hitB_iter).first.plane)].make<TH1F>(Histo_name.c_str(), Histo_name.c_str(), 1200, -60, 60 );
									
								Histo_name = "SPC_resolution_TP_sec_"+std::to_string(sec_number)+"_planes_"+std::to_string((*hitA_iter).first.plane)+"_"+std::to_string((*hitB_iter).first.plane)+
									"_channels_"+std::to_string((*hitA_iter).first.channel)+"_"+std::to_string((*hitB_iter).first.channel);
								SPC_DT_TrackedPlane_Hmap_[sec_number][std::make_pair((*hitA_iter).first, (*hitB_iter).first)] = 
									dir_cyl_TPTI_V[sec_number][std::make_pair((*hitA_iter).first.plane,(*hitB_iter).first.plane)].make<TH1F>(Histo_name.c_str(), Histo_name.c_str(), 1200, -60, 60 );
									
								Histo_name = "TOT_vs_TOT_TP_sector_"+std::to_string(sec_number)+"_planes_"+std::to_string((*hitA_iter).first.plane)+"_"+std::to_string((*hitB_iter).first.plane)+
									"_channels_"+std::to_string((*hitA_iter).first.channel)+"_"+std::to_string((*hitB_iter).first.channel);
								ToT_vs_ToT_TrackedPlane_Hmap_[sec_number][std::make_pair((*hitA_iter).first, (*hitB_iter).first)] = 
									dir_cyl_TPTI_V[sec_number][std::make_pair((*hitA_iter).first.plane,(*hitB_iter).first.plane)].make<TH2F>(Histo_name.c_str(), Histo_name.c_str(), 100, 5, 25, 100, 5, 25 );
								
						}	
						
						
						Raw_DT_TrackedPlane_Hmap_[sec_number][std::make_pair((*hitA_iter).first, (*hitB_iter).first)]->
							Fill(DiamondDet.GetTime(sec_number,(*hitA_iter).first.plane,(*hitA_iter).first.channel)-DiamondDet.GetTime(sec_number,(*hitB_iter).first.plane,(*hitB_iter).first.channel));
		
						double TimeA_corr=DiamondDet.GetTime_SPC(sec_number,(*hitA_iter).first.plane,(*hitA_iter).first.channel);
						double TimeB_corr=DiamondDet.GetTime_SPC(sec_number,(*hitB_iter).first.plane,(*hitB_iter).first.channel);
					
						SPC_DT_TrackedPlane_Hmap_[sec_number][std::make_pair((*hitA_iter).first, (*hitB_iter).first)]->Fill(TimeA_corr-TimeB_corr);
						ToT_vs_ToT_TrackedPlane_Hmap_[sec_number][std::make_pair((*hitA_iter).first, (*hitB_iter).first)]->
							Fill(DiamondDet.GetToT(sec_number,(*hitA_iter).first.plane,(*hitA_iter).first.channel),DiamondDet.GetToT(sec_number,(*hitB_iter).first.plane,(*hitB_iter).first.channel));	
						Correlation_TP_V_[sec_number][std::make_pair((*hitA_iter).first.plane,(*hitB_iter).first.plane)] ->Fill((*hitA_iter).first.channel, (*hitB_iter).first.channel);

			}	
				
		}
	
	}	
	
	//std::cout << "After track loop" << std::endl;
	
}


//for (const auto& LocalTrack_mapIter : DiamondDet.GetDiamondTrack_map(sec_number))
//{
//
//}	
		

}


// ------------ method called once each job just before starting event loop  ------------
void
DiamondTimingAnalyzer::beginJob()
{
   // edm::Service<TFileService> fs_;
	
	dir_cyl_V.push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr" ));
	dir_cyl_V.push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr" ));
	
	dir_cyl_DDTI_V.push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/DDTimeInfo" ));
	dir_cyl_DDTI_V.push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/DDTimeInfo" ));

	dir_cyl_TPTI_V.resize(2);	
	dir_cyl_TPTI_V[0][std::make_pair(0,1)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/TPTimeInfo/plane0_1" );
	dir_cyl_TPTI_V[0][std::make_pair(0,2)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/TPTimeInfo/plane0_2" );
	dir_cyl_TPTI_V[0][std::make_pair(0,3)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/TPTimeInfo/plane0_3" );
	dir_cyl_TPTI_V[0][std::make_pair(1,2)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/TPTimeInfo/plane1_2" );
	dir_cyl_TPTI_V[0][std::make_pair(1,3)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/TPTimeInfo/plane1_3" );
	dir_cyl_TPTI_V[0][std::make_pair(2,3)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/TPTimeInfo/plane2_3" );
	dir_cyl_TPTI_V[1][std::make_pair(0,1)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/TPTimeInfo/plane0_1" );
	dir_cyl_TPTI_V[1][std::make_pair(0,2)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/TPTimeInfo/plane0_2" );
	dir_cyl_TPTI_V[1][std::make_pair(0,3)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/TPTimeInfo/plane0_3" );
	dir_cyl_TPTI_V[1][std::make_pair(1,2)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/TPTimeInfo/plane1_2" );
	dir_cyl_TPTI_V[1][std::make_pair(1,3)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/TPTimeInfo/plane1_3" );
	dir_cyl_TPTI_V[1][std::make_pair(2,3)]=fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/TPTimeInfo/plane2_3" );
	
	
	dir_plane_VV.push_back(std::vector<TFileDirectory>());
	dir_plane_VV.push_back(std::vector<TFileDirectory>());
	dir_plane_VV[0].push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/plane 0" ));
	dir_plane_VV[0].push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/plane 1" ));
	dir_plane_VV[0].push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/plane 2" ));
	dir_plane_VV[0].push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 45/station 220cyl/cyl_hr/plane 3" ));
	dir_plane_VV[1].push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/plane 0" ));
	dir_plane_VV[1].push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/plane 1" ));
	dir_plane_VV[1].push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/plane 2" ));
	dir_plane_VV[1].push_back(fs_->mkdir( "CTPPS/TimingDiamond/sector 56/station 220cyl/cyl_hr/plane 3" ));

	for (int sec_number=0; sec_number < MAX_SECTOR_NUMBER; sec_number++)
	{		 	
		for  (int ch_number=0; ch_number < CHANNELS_X_PLANE; ch_number++)
		{
			
	
			std::string Raw_DT_name("RAW_DeltaT_DD_sector_"+std::to_string(sec_number)+"_channel_"+std::to_string(ch_number));
			std::string SPC_DT_name("SPC_DeltaT_DD_sector_"+std::to_string(sec_number)+"_channel_"+std::to_string(ch_number));
			std::string ToT_vs_ToT_name("TOT_vs_TOT_DD_sector_"+std::to_string(sec_number)+"_channel_"+std::to_string(ch_number));
			
			Raw_DT_Hmap_[ std::make_pair(sec_number,ch_number) ] = dir_cyl_DDTI_V[sec_number].make<TH1F>(Raw_DT_name.c_str(), Raw_DT_name.c_str(), 1200, -60, 60 );
			SPC_DT_Hmap_[ std::make_pair(sec_number,ch_number) ] = dir_cyl_DDTI_V[sec_number].make<TH1F>(SPC_DT_name.c_str(), SPC_DT_name.c_str(), 1200, -60, 60 );
			ToT_vs_ToT_Hmap_[ std::make_pair(sec_number,ch_number) ] = dir_cyl_DDTI_V[sec_number].make<TH2F>(ToT_vs_ToT_name.c_str(), ToT_vs_ToT_name.c_str(), 100, 5, 25, 100, 5, 25 );	
			
		}
	}	
	
	for (int sec_number=0; sec_number < MAX_SECTOR_NUMBER; sec_number++)
	{	
	
		for  (int pl_number=0; pl_number < PLANES_X_DETECTOR; pl_number++)
		{
		 
			std::string Plane_Mux_Hname("PlaneMux_sSector_"+std::to_string(sec_number)+"_plane_"+std::to_string(pl_number));
			Plane_Mux_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH1F>(Plane_Mux_Hname.c_str(), Plane_Mux_Hname.c_str(), 16, 0, 16 );
			Plane_Mux_Hname = "PlaneMuxValidT_sector_"+std::to_string(sec_number)+"_plane_"+std::to_string(pl_number);
			Plane_Mux_validT_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH1F>(Plane_Mux_Hname.c_str(), Plane_Mux_Hname.c_str(), 16, 0, 16 );
			Plane_Mux_Hname = "PlaneMuxSaturated_sector_"+std::to_string(sec_number)+"_plane_"+std::to_string(pl_number);
			Plane_Mux_saturated_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH1F>(Plane_Mux_Hname.c_str(), Plane_Mux_Hname.c_str(), 16, 0, 16 );
			Plane_Mux_Hname = "PlaneMuxSaturatedValidT_sector_"+std::to_string(sec_number)+"_plane_"+std::to_string(pl_number);
			Plane_Mux_saturated_validT_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH1F>(Plane_Mux_Hname.c_str(), Plane_Mux_Hname.c_str(), 16, 0, 16 );
			
			
			Plane_Mux_Hname = "Track_PlaneMuxValidT_sector_"+std::to_string(sec_number)+"_plane_"+std::to_string(pl_number);
			Track_Plane_Mux_validT_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH1F>(Plane_Mux_Hname.c_str(), Plane_Mux_Hname.c_str(), 16, 0, 16 );
			Plane_Mux_Hname = "Track_PlaneMuxSaturatedValidT_sector_"+std::to_string(sec_number)+"_plane_"+std::to_string(pl_number);
			Track_Plane_Mux_saturated_validT_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH1F>(Plane_Mux_Hname.c_str(), Plane_Mux_Hname.c_str(), 16, 0, 16 );
			
			
			std::string TP_Mux_Hname("Diamond-Pixel 220 mux correlation sector "+std::to_string(sec_number)+" plane "+std::to_string(pl_number));
			TimingPixel_mux_220_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH2F>(TP_Mux_Hname.c_str(), TP_Mux_Hname.c_str(), 12, 0, 12, 20, 0, 20 ); 
			TP_Mux_Hname = "Diamond-Pixel 210 mux correlation sector "+std::to_string(sec_number)+" plane "+std::to_string(pl_number); 
			TimingPixel_mux_210_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH2F>(TP_Mux_Hname.c_str(), TP_Mux_Hname.c_str(), 12, 0, 12, 20, 0, 20 );
			TP_Mux_Hname = "Diamond-Pixel 220 Saturated mux correlation sector "+std::to_string(sec_number)+" plane "+std::to_string(pl_number);
			TimingPixel_Saturated_mux_220_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH2F>(TP_Mux_Hname.c_str(), TP_Mux_Hname.c_str(), 12, 0, 12, 20, 0, 20 ); 
			TP_Mux_Hname = "Diamond-Pixel 210 Saturated mux correlation sector "+std::to_string(sec_number)+" plane "+std::to_string(pl_number);
			TimingPixel_Saturated_mux_210_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH2F>(TP_Mux_Hname.c_str(), TP_Mux_Hname.c_str(), 12, 0, 12, 20, 0, 20 ); 
			
		
			std::string MS_Corr_Hname = "Mux-Spread correlation sector "+std::to_string(sec_number)+" plane "+std::to_string(pl_number);
			MuxSpread_corr_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH2F>(MS_Corr_Hname.c_str(), MS_Corr_Hname.c_str(), 12, 0, 12, 12, 0, 12 ); 
			MS_Corr_Hname="Tracks Mux-Spread correlation sector" + std::to_string(sec_number)+" plane "+std::to_string(pl_number);		
			TrackMuxSpread_corr_Hmap_[ std::make_pair(sec_number,pl_number) ] = dir_plane_VV[sec_number][pl_number].make<TH2F>(MS_Corr_Hname.c_str(), MS_Corr_Hname.c_str(), 12, 0, 12, 13, 0, 12 ); 	
			
			std::string Hname = "Valid TOT mean sector "+std::to_string(sec_number)+" plane "+std::to_string(pl_number);
			ValidToT_mean_Hmap_[ std::make_pair(sec_number,pl_number)] = dir_plane_VV[sec_number][pl_number].make<TH1F>(Hname.c_str(), Hname.c_str(), 12, 0, 12);
		
			
			for (int pl_number_2 = pl_number+1; pl_number_2 < PLANES_X_DETECTOR; pl_number_2++)
			{
			
				Hname = "Diamond plane mux correlation sector "+std::to_string(sec_number)+" plane "+std::to_string(pl_number)+ "_" + std::to_string(pl_number_2);
				TimingCorrelation_mux_Hmap_V_[sec_number][ std::make_pair(pl_number,pl_number_2) ] = dir_cyl_V[sec_number].make<TH2F>(Hname.c_str(), Hname.c_str(), 12, 0, 12, 12, 0, 12 ); 
				

				Hname = "Raw_resolution_TP_sec_"+std::to_string(sec_number)+"_plane_"+std::to_string(pl_number)+ "_" + std::to_string(pl_number_2);	
				Raw_resolution_TP_V_[sec_number][std::make_pair(pl_number,pl_number_2)] = dir_cyl_TPTI_V[sec_number][std::make_pair(pl_number,pl_number_2)].make<TH2F>(Hname.c_str(), Hname.c_str(), 12, 0, 12,12, 0, 12);
				
				Hname = "SPC_resolution_TP_sec_"+std::to_string(sec_number)+"_plane_"+std::to_string(pl_number)+ "_" + std::to_string(pl_number_2);	
				SPC_resolution_TP_V_[sec_number][std::make_pair(pl_number,pl_number_2)] = dir_cyl_TPTI_V[sec_number][std::make_pair(pl_number,pl_number_2)].make<TH2F>(Hname.c_str(), Hname.c_str(), 12, 0, 12,12, 0, 12);

				Hname = "Occupancy_TP_sec_"+std::to_string(sec_number)+"_plane_"+std::to_string(pl_number)+ "_" + std::to_string(pl_number_2);	
				Correlation_TP_V_[sec_number][std::make_pair(pl_number,pl_number_2)] = dir_cyl_TPTI_V[sec_number][std::make_pair(pl_number,pl_number_2)].make<TH2F>(Hname.c_str(), Hname.c_str(), 12, 0, 12,12, 0, 12);		
				
			}
				
		}
	}	
	
	for (int sec_number=0; sec_number < MAX_SECTOR_NUMBER; sec_number++)
	{
	
	    std::string name="Pixel hit map sector "+std::to_string(sec_number)+" station 210";
		Pixel_Hit_Hmap_[ std::make_pair(sec_number, STATION_210_M_ID) ] = dir_cyl_V[sec_number].make<TH2F>(name.c_str(), name.c_str(), 1000, -100, 100, 1000, -100, 100);
	    name="Pixel hit map sector "+std::to_string(sec_number)+" station 220";
		Pixel_Hit_Hmap_[ std::make_pair(sec_number, STATION_220_M_ID) ] = dir_cyl_V[sec_number].make<TH2F>(name.c_str(), name.c_str(), 1000, -100, 100, 1000, -100, 100);
		
		
	
	    name="Tomography sector "+std::to_string(sec_number)+" station 210";
		Pixel_Tomography_Hmap_[ std::make_pair(sec_number, STATION_210_M_ID) ] = dir_cyl_V[sec_number].make<TH2F>(name.c_str(), name.c_str(), 1000, -100, 100, 1000, -100, 100);
	    name="Tomography sector "+std::to_string(sec_number)+" station 220";
		Pixel_Tomography_Hmap_[ std::make_pair(sec_number, STATION_220_M_ID) ] = dir_cyl_V[sec_number].make<TH2F>(name.c_str(), name.c_str(), 1000, -100, 100, 1000, -100, 100);
		
		
		
		Raw_resolution_DD_V_[sec_number] = dir_cyl_DDTI_V[sec_number].make<TGraph>(12);
		name="Raw_resolution_DD_sec_"+std::to_string(sec_number);
		Raw_resolution_DD_V_[sec_number] -> SetNameTitle(name.c_str(),name.c_str());
		
		SPC_resolution_DD_V_[sec_number] = dir_cyl_DDTI_V[sec_number].make<TGraph>(12);
		name="SPC_resolution_DD_sec_"+std::to_string(sec_number);
		SPC_resolution_DD_V_[sec_number] -> SetNameTitle(name.c_str(),name.c_str());
		
		Saturated_percentage_V_[sec_number] = dir_cyl_V[sec_number].make<TGraph>(12);
		name="Saturated_percentage_sec_"+std::to_string(sec_number);
		Saturated_percentage_V_[sec_number] -> SetNameTitle(name.c_str(),name.c_str());

	    name=" Reconstructed Constituent Pad Mux sector" + std::to_string(sec_number);		
		PadInTracks_Reco_Hmap_[sec_number] = dir_cyl_V[sec_number].make<TH1F>(name.c_str(), name.c_str(), 12, 0, 12);
	    name="Timing tracks mux sector" + std::to_string(sec_number);		
		TimingTracks_Mux_Hmap_[sec_number] = dir_cyl_V[sec_number].make<TH1F>(name.c_str(), name.c_str(), 12, 0, 12);
	    name="Timing tracks saturated mux sector" + std::to_string(sec_number);		
		TimingTracks_Mux_saturated_Hmap_[sec_number] = dir_cyl_V[sec_number].make<TH1F>(name.c_str(), name.c_str(), 12, 0, 12);  


			
		name="DiamondTracks-Pixel 220 mux correlation sector "+std::to_string(sec_number);
		TimingTrack_Pixel_mux_220_Hmap_[sec_number] = dir_cyl_V[sec_number].make<TH2F>(name.c_str(), name.c_str(), 12, 0, 12, 20, 0, 20 ); 
		name="DiamondTracks-Pixel 210 mux correlation sector "+std::to_string(sec_number); 
		TimingTrack_Pixel_mux_210_Hmap_[sec_number] = dir_cyl_V[sec_number].make<TH2F>(name.c_str(), name.c_str(), 12, 0, 12, 20, 0, 20 );
		name="DiamondTracks-Pixel 220 Saturated mux correlation sector "+std::to_string(sec_number);
		TimingTrack_Pixel_Saturated_mux_220_Hmap_[sec_number] = dir_cyl_V[sec_number].make<TH2F>(name.c_str(), name.c_str(), 12, 0, 12, 20, 0, 20 ); 
		name="DiamondTracks-Pixel 210 Saturated mux correlation sector "+std::to_string(sec_number);
		TimingTrack_Pixel_Saturated_mux_210_Hmap_[sec_number] = dir_cyl_V[sec_number].make<TH2F>(name.c_str(), name.c_str(), 12, 0, 12, 20, 0, 20 );	
		
	    name="Timing track hit map sector "+std::to_string(sec_number);
		TimingTrack_Hit_Hmap_[ sec_number ] = dir_cyl_V[sec_number].make<TH2F>(name.c_str(), name.c_str(), 100, 0, 20, 10, -1.05, 0.95);	
		
		
	  				
		
	}
	
	
	
	TFileDirectory  BaseDir = fs_->mkdir( "Calib/" );
	
	TCalibration_ = BaseDir.make<TTree>("CalibTree","rootuple");
	
	TCalibration_ -> Branch("Arm"  		, &(this->cal_arm_), "Arm/I");
	TCalibration_ -> Branch("Plane"		, &(this->cal_plane_), "Plane/I");
	TCalibration_ -> Branch("Channel"  	, &(this->cal_channel_), "Channel/I");
	TCalibration_ -> Branch("Par_0"		, &(this->cal_par_0_), "Par_0/D" );
	TCalibration_ -> Branch("Par_1"		, &(this->cal_par_1_), "Par_1/D" );
	TCalibration_ -> Branch("Par_2"		, &(this->cal_par_2_), "Par_2/D" );
	TCalibration_ -> Branch("Par_3"		, &(this->cal_par_3_), "Par_3/D" );
	TCalibration_ -> Branch("Chi_2"		, &(this->Chi_2_), "Chi_2/D" );
	
}

// ------------ method called once each job just after ending the event loop  ------------
void
DiamondTimingAnalyzer::endJob()
{
	
	int X_sat_start = ToT_vs_ToT_Hmap_[std::make_pair(0,0)]-> GetXaxis()->FindBin(15);
	int X_sat_stop = ToT_vs_ToT_Hmap_[std::make_pair(0,0)]-> GetNbinsX();
	int Y_sat_start = ToT_vs_ToT_Hmap_[std::make_pair(0,0)]-> GetYaxis()->FindBin(15);
	int Y_sat_stop = ToT_vs_ToT_Hmap_[std::make_pair(0,0)]-> GetNbinsY();
	
	
	for (int sec_number=0; sec_number < MAX_SECTOR_NUMBER; sec_number++)
	{
		for  (int ch_A=0; ch_A < CHANNELS_X_PLANE; ch_A++)
		{
			if (Raw_DT_Hmap_[std::make_pair(sec_number,ch_A)]->GetEntries() >100)
			{	
		
				Raw_DT_Hmap_[std::make_pair(sec_number,ch_A)]->Fit("gaus","+","",-3,3);
				Raw_resolution_DD_V_[sec_number]->SetPoint(ch_A+1, ch_A, Raw_DT_Hmap_[std::make_pair(sec_number,ch_A)]->GetFunction("gaus")->GetParameter(2)/sqrt(2));
				Saturated_percentage_V_[sec_number] ->SetPoint(ch_A+1, ch_A, (ToT_vs_ToT_Hmap_[std::make_pair(sec_number,ch_A)]->Integral(X_sat_start,X_sat_stop,Y_sat_start,Y_sat_stop))/(ToT_vs_ToT_Hmap_[std::make_pair(sec_number,ch_A)]->Integral()));
			
				SPC_DT_Hmap_[std::make_pair(sec_number,ch_A)]->Fit("gaus","+","",-10,10);		
				double SPC_mean = SPC_DT_Hmap_[std::make_pair(sec_number,ch_A)]->GetFunction("gaus")->GetParameter(1);
				double SPC_sigma = SPC_DT_Hmap_[std::make_pair(sec_number,ch_A)]->GetFunction("gaus")->GetParameter(2);
				SPC_DT_Hmap_[std::make_pair(sec_number,ch_A)]->Fit("gaus","","",SPC_mean-(2.2*SPC_sigma),SPC_mean+(2.2*SPC_sigma));				
				SPC_resolution_DD_V_[sec_number]->SetPoint(ch_A+1, ch_A, SPC_DT_Hmap_[std::make_pair(sec_number,ch_A)]->GetFunction("gaus")->GetParameter(2)/sqrt(2));
			}
			else
			{
				Raw_resolution_DD_V_[sec_number]->SetPoint(ch_A+1, ch_A, 0);	
				SPC_resolution_DD_V_[sec_number]->SetPoint(ch_A+1, ch_A, 0);	
			}
		}
			
		for (const auto& Raw_histo_iter	: Raw_DT_TrackedPlane_Hmap_[sec_number]) // <<recHit_info1,recHit_info2>,histogram>
		{
			std::pair<RecHit_info,RecHit_info> histo_key = Raw_histo_iter.first;
			int plane_1 = histo_key.first.plane;
			int plane_2 = histo_key.second.plane;
			int chann_1 = histo_key.first.channel;
			int chann_2 = histo_key.second.channel;
		
			if (Raw_DT_TrackedPlane_Hmap_[sec_number][histo_key]->GetEntries()>100) // controlla se l'histo ha abbastanya  hit
			{
				Raw_DT_TrackedPlane_Hmap_[sec_number][histo_key]->Fit("gaus","+","",-3,3);
				Raw_resolution_TP_V_[sec_number][std::make_pair(plane_1,plane_2)]->SetBinContent(chann_1+1, chann_2+1, Raw_DT_TrackedPlane_Hmap_[sec_number][histo_key]->GetFunction("gaus")->GetParameter(2)/sqrt(2));
	
				SPC_DT_TrackedPlane_Hmap_[sec_number][histo_key]->Fit("gaus","+","",-10,10);		
				double SPC_mean = SPC_DT_TrackedPlane_Hmap_[sec_number][histo_key]->GetFunction("gaus")->GetParameter(1);
				double SPC_sigma = SPC_DT_TrackedPlane_Hmap_[sec_number][histo_key]->GetFunction("gaus")->GetParameter(2);
				SPC_DT_TrackedPlane_Hmap_[sec_number][histo_key]->Fit("gaus","","",SPC_mean-(2.2*SPC_sigma),SPC_mean+(2.2*SPC_sigma));				
				SPC_resolution_TP_V_[sec_number][std::make_pair(plane_1,plane_2)]->SetBinContent(chann_1+1, chann_2+1, SPC_DT_TrackedPlane_Hmap_[sec_number][histo_key]->GetFunction("gaus")->GetParameter(2)/sqrt(2));						
			}
		}
		
		
	}
	
	
	

    for ( auto& Histo_handle : TOTvsT_Histo_map_ ) //rechit
    {
		
		//TOT average
		ValidToT_mean_Hmap_[std::pair(Histo_handle.first.arm(),Histo_handle.first.plane())]->SetBinContent(Histo_handle.first.channel()+1,ValidTOT_Histo_map_[Histo_handle.first]->GetMean());
		
		
		std::cout << "profiling" << std::endl;
		//&(Histo_handle.second) = &(TvsTOT_Histo_map_[Histo_handle.first]->ProfileX("_pfx",0,100));
		 
		//std::string TvsTOT_Profile_name(chName);
		//TvsTOT_Profile_name.insert(0, "TvsTOT_Profile_");
		
		std::cout << "histo entry" << Histo_handle.second->GetEntries() << std::endl;
		
		TOTvsT_Profile_map_[ Histo_handle.first ] = channelDir_map_[ Histo_handle.first ].make<TProfile>(*Histo_handle.second->ProfileX("_pfx",1,-1));
		if (Histo_handle.second->GetEntries() > 100 )
		{
			double Fit_UpRange = ValidTOT_Histo_map_[Histo_handle.first]->GetMean()+2;
			
			
			TF1 *myfermi = new TF1("MYfit","[0]/(exp((x-[1])/[2])+1)+[3]",5,20);
			myfermi-> SetParameters(1.5,12.5,1,4);
			TOTvsT_Profile_map_[ Histo_handle.first ]->Fit(myfermi,"+","",5,Fit_UpRange);
			
			cal_arm_    = Histo_handle.first.arm();
			cal_plane_  = Histo_handle.first.plane();
			cal_channel_= Histo_handle.first.channel();
			cal_par_0_  = myfermi-> GetParameter(0);
			cal_par_1_  = myfermi-> GetParameter(1);
			cal_par_2_  = myfermi-> GetParameter(2);
			cal_par_3_  = myfermi-> GetParameter(3);
			Chi_2_  = myfermi-> GetChisquare();
			
			TCalibration_ ->Fill();
			
		}
	}	
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DiamondTimingAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
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
DEFINE_FWK_MODULE(DiamondTimingAnalyzer);
