// -*- C++ -*-
//
// Package:    Filter/TimingMux_Filter
// Class:      TimingMux_Filter
// 
/**\class TimingMux_Filter TimingMux_Filter.cc Filter/TimingMux_Filter/plugins/TimingMux_Filter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Edoardo Bossini
//         Created:  Mon, 11 Feb 2019 13:53:54 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Common/interface/DetSetVector.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/CTPPSDigi/interface/CTPPSDiamondDigi.h"
#include "DataFormats/CTPPSDetId/interface/CTPPSDiamondDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondRecHit.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondLocalTrack.h"
#include "DataFormats/CTPPSDetId/interface/CTPPSDetId.h"

#include <map>


//
// class declaration
//

class TimingMux_Filter : public edm::stream::EDFilter<> {
   public:
      explicit TimingMux_Filter(const edm::ParameterSet&);
      ~TimingMux_Filter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
   
   	enum Sector_id_
	{
		SECTOR_45_ID,
		SECTOR_56_ID
    };
	  
	enum Station_id_
	{
		STATION_210_M_ID,
		STATION_TIMING_ID,
		STATION_220_M_ID
		
    };
	
	
      virtual void beginStream(edm::StreamID) override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endStream() override;
	  
      // ---------- objects to retrieve ---------------------------
      edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondLocalTrack> > tokenTimingTracks_;

      // ----------member data ---------------------------
      std::map< int, int> TTracks_Mux_map_; //arm, station
      std::map< int, std::pair< int , int > > TTracks_cuts_map_; //arm < Lcut,Ucut >
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
TimingMux_Filter::TimingMux_Filter(const edm::ParameterSet& iConfig)
:
 tokenTimingTracks_             ( consumes< edm::DetSetVector<CTPPSDiamondLocalTrack> >      ( iConfig.getParameter<edm::InputTag>( "tagTimingLocalTrack" )))
{
	TTracks_cuts_map_[SECTOR_45_ID] = std::make_pair(iConfig.getParameter< std::vector <int> >( "TTracksMux_Lcuts" )[0],
													iConfig.getParameter< std::vector <int> >( "TTracksMux_Ucuts" )[0]);
	TTracks_cuts_map_[SECTOR_56_ID] = std::make_pair(iConfig.getParameter< std::vector <int> >( "TTracksMux_Lcuts" )[1],
													iConfig.getParameter< std::vector <int> >( "TTracksMux_Ucuts" )[1]);
}


TimingMux_Filter::~TimingMux_Filter()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
TimingMux_Filter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

	edm::Handle< edm::DetSetVector<CTPPSDiamondLocalTrack> > TimingTracks;
	iEvent.getByToken( tokenTimingTracks_, TimingTracks );
	
	
	
	
	TTracks_Mux_map_.clear();

  
	for (const auto& locTracks : *TimingTracks) //rechits = array of hits in one channel
	{
		for (const auto& locTrack : locTracks)
		{
			if (locTrack.getZ0() > 0.0 )   //yes, positive side is 45...
				TTracks_Mux_map_[SECTOR_45_ID]++;
			else
				TTracks_Mux_map_[SECTOR_56_ID]++;
				
			
		}
	} 
	
	for (const auto& TTracks_cuts_iter_ :  TTracks_cuts_map_)
	{
		if ( (TTracks_cuts_iter_.second.first < 0) || (TTracks_cuts_iter_.second.second < 0) ) continue; // don't care condition
		if ( (TTracks_Mux_map_[TTracks_cuts_iter_.first] < TTracks_cuts_iter_.second.first) ||
			 (TTracks_Mux_map_[TTracks_cuts_iter_.first] > TTracks_cuts_iter_.second.second)) return false;		
	}
	
	//iEvent.put(std::make_unique<int>(33));
	
	return true;
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
TimingMux_Filter::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
TimingMux_Filter::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
TimingMux_Filter::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
TimingMux_Filter::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
TimingMux_Filter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
TimingMux_Filter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TimingMux_Filter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(TimingMux_Filter);
