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

//
// constructors and destructor
//
LightAnalyzer::LightAnalyzer(const edm::ParameterSet& iConfig):
    tokenRecHit(consumes<edm::DetSetVector<CTPPSDiamondRecHit>>(iConfig.getParameter<edm::InputTag>("tagRecHit"))),
    tokenLocalTrack(consumes<edm::DetSetVector<CTPPSDiamondLocalTrack>>(iConfig.getParameter<edm::InputTag>("tagLocalTrack"))),
    DiamondDetector(iConfig, tokenRecHit, tokenLocalTrack)
{
}


LightAnalyzer::~LightAnalyzer() {}


//
// member functions
//

// ------------ method called for each event  ------------
void LightAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;

    edm::Handle<edm::DetSetVector<CTPPSDiamondRecHit>> timingRecHit;
    iEvent.getByToken(tokenRecHit, timingRecHit);

    DiamondDetector.ExtractData(iEvent);

    for (const auto& recHits : *timingRecHit) {
       const CTPPSDiamondDetId detId(recHits.detId());
       const ChannelKey recHitKey(detId.arm(), detId.plane(), detId.channel());

       for (const auto& recHit : recHits) {
            if (DiamondDetector.PadActive(detId.arm(), detId.plane(), detId.channel())) { // T and ToT are present
                std::cout << "LS: " << iEvent.luminosityBlock() << ", ToT: " << DiamondDetector.GetToT(detId.arm(), detId.plane(), detId.channel()) << std::endl;
            }
       }
    }
}


// ------------ method called once each job just before starting event loop  ------------
void LightAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void LightAnalyzer::endJob()
{
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
