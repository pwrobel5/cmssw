import FWCore.ParameterSet.Config as cms

from FWCore.ParameterSet.VarParsing import VarParsing
from RecoCTPPS.TotemRPLocal.ctppsDiamondLocalReconstruction_cff import *
from Configuration.AlCa.GlobalTag import GlobalTag

options = VarParsing("analysis")

options.register("calibFile",
                0,
                VarParsing.multiplicity.singleton,
                VarParsing.varType.string,
                "Calibration input file")

options.register("geometryFile",
                0,
                VarParsing.multiplicity.singleton,
                VarParsing.varType.string,
                "Geometry input file")

options.register("validOOT",
                0,
                VarParsing.multiplicity.singleton,
                VarParsing.varType.int,
                "valid OOT slice")
                
options.parseArguments()


process = cms.Process("LightAnalyzer")

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))
process.verbosity = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag = GlobalTag(process.GlobalTag, "auto:run2_data", "")

process.load(options.geometryFile)
process.load("RecoCTPPS.TotemRPLocal.ctppsDiamondLocalReconstruction_cff")

process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(True))

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "/store/data/Run2018D/ZeroBias/AOD/PromptReco-v2/000/322/332/00000/00302FB3-1CB4-E811-96B2-FA163EC9F152.root",
    ))

process.LightAnalyzer = cms.EDAnalyzer("LightAnalyzer",
    tagRecHit = cms.InputTag("ctppsDiamondRecHits"),
    tagLocalTrack = cms.InputTag("ctppsDiamondLocalTracks"),
    tagCalibrationFile = cms.string(options.calibFile),
    tagValidOOT = cms.int32(options.validOOT)
)
process.Tracer = cms.Service("Tracer")

process.TFileService = cms.Service("TFileService", fileName = cms.string(options.outputFile))

process.path = cms.Path(ctppsDiamondLocalReconstruction * process.LightAnalyzer)
process.schedule = cms.Schedule(process.path)
