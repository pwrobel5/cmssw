import FWCore.ParameterSet.Config as cms

UFSDAnlzr = cms.EDAnalyzer('UFSDAnlzr',
     tagDigi = cms.InputTag("totemTimingRawToDigi","TotemTiming"),
     tagRecHit = cms.InputTag("totemTimingRecHits"),
)
