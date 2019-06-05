// -*- C++ -*-
//
// Package:    Filter/PixelMux_Filter
// Class:      PixelMux_Filter
// 
/**\class PixelMux_Filter PixelMux_Filter.cc Filter/PixelMux_Filter/plugins/PixelMux_Filter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Edoardo Bossini
//         Created:  Wed, 31 Oct 2018 08:35:17 GMT
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


#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"
#include "DataFormats/CTPPSDetId/interface/CTPPSDetId.h"

#include <map>

//
// class declaration
//

class PixelMux_Filter : public edm::stream::EDFilter<> {
   public:
      explicit PixelMux_Filter(const edm::ParameterSet&);
      ~PixelMux_Filter();

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
      edm::EDGetTokenT< edm::DetSetVector<CTPPSPixelLocalTrack> > tokenPixelLocalTrack_;

      // ----------member data ---------------------------
      std::map< std::pair< int , int >, int> Pixel_Mux_map_; //arm, station
      std::map< std::pair< int , int >, std::pair< int , int > > Ntracks_cuts_map_; //arm, station ,, Lcut,Ucut
	  
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
PixelMux_Filter::PixelMux_Filter(const edm::ParameterSet& iConfig)
:
 tokenPixelLocalTrack_      ( consumes< edm::DetSetVector<CTPPSPixelLocalTrack> >      ( iConfig.getParameter<edm::InputTag>( "tagPixelLocalTrack" )))
{
	Ntracks_cuts_map_[std::make_pair(SECTOR_45_ID,STATION_210_M_ID)] = std::make_pair(iConfig.getParameter< std::vector <int> >( "Ntracks_Lcuts" )[0],
																					 iConfig.getParameter< std::vector <int> >( "Ntracks_Ucuts" )[0]);
	Ntracks_cuts_map_[std::make_pair(SECTOR_45_ID,STATION_220_M_ID)] = std::make_pair(iConfig.getParameter< std::vector <int> >( "Ntracks_Lcuts" )[1],
																					 iConfig.getParameter< std::vector <int> >( "Ntracks_Ucuts" )[1]);
	Ntracks_cuts_map_[std::make_pair(SECTOR_56_ID,STATION_210_M_ID)] = std::make_pair(iConfig.getParameter< std::vector <int> >( "Ntracks_Lcuts" )[2],
																					 iConfig.getParameter< std::vector <int> >( "Ntracks_Ucuts" )[2]);
	Ntracks_cuts_map_[std::make_pair(SECTOR_56_ID,STATION_220_M_ID)] = std::make_pair(iConfig.getParameter< std::vector <int> >( "Ntracks_Lcuts" )[3],
																					 iConfig.getParameter< std::vector <int> >( "Ntracks_Ucuts" )[3]);
}


PixelMux_Filter::~PixelMux_Filter()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
PixelMux_Filter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
//#ifdef THIS_IS_AN_EVENT_EXAMPLE
//   Handle<ExampleData> pIn;
//   iEvent.getByLabel("example",pIn);
//#endif
//
//#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
//   ESHandle<SetupData> pSetup;
//   iSetup.get<SetupRecord>().get(pSetup);
//#endif

  edm::Handle< edm::DetSetVector<CTPPSPixelLocalTrack> > pixelLocalTrack;
  iEvent.getByToken( tokenPixelLocalTrack_, pixelLocalTrack );

  Pixel_Mux_map_.clear();

  
 
   for (const auto& RP_trks : *pixelLocalTrack) //array of tracks
  {
      const CTPPSDetId detId( RP_trks.detId() );
	  
		//std::cout << "Tracks in arm " << detId.arm() << ", station " << detId.station() << ", rp " << detId.rp() << std::endl;
      
	  for ( const auto& trk : RP_trks ) 
	  {
		if ( trk.isValid() )
			Pixel_Mux_map_[ std::make_pair(detId.arm(), detId.station()) ]++;
      }
	} 
	
	for (const auto& Ntracks_cuts_iter_ :  Ntracks_cuts_map_)
	{
		if ( (Ntracks_cuts_iter_.second.first < 0) || (Ntracks_cuts_iter_.second.second < 0) ) continue; // don't care condition
		if ( (Pixel_Mux_map_[Ntracks_cuts_iter_.first] < Ntracks_cuts_iter_.second.first) ||
			 (Pixel_Mux_map_[Ntracks_cuts_iter_.first] > Ntracks_cuts_iter_.second.second)) return false;		
	}
	
	//iEvent.put(std::make_unique<int>(33));
	
	return true;
		

  
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
PixelMux_Filter::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
PixelMux_Filter::endStream() {
}

// ------------ method called when starting to processes a run  ------------
/*
void
PixelMux_Filter::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
void
PixelMux_Filter::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
void
PixelMux_Filter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
PixelMux_Filter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
PixelMux_Filter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(PixelMux_Filter);
