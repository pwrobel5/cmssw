import FWCore.ParameterSet.Config as cms

from FWCore.ParameterSet.VarParsing import VarParsing
from RecoCTPPS.TotemRPLocal.ctppsDiamondLocalReconstruction_cff import *
from Configuration.AlCa.GlobalTag import GlobalTag
from CondCore.CondDB.CondDB_cfi import *

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

options.register("minBX",
                0,
                VarParsing.multiplicity.singleton,
                VarParsing.varType.int,
                "minimal bunch crossing value taken into histograms")

options.register("maxBX",
                4000,
                VarParsing.multiplicity.singleton,
                VarParsing.varType.int,
                "maximal bunch crossing value taken into histograms")

options.register("useJSONCalibration",
                "n",
                VarParsing.multiplicity.singleton,
                VarParsing.varType.string,
                "determine if it is needed to use calibration from JSON file")
                
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

process.CondDBOptics = CondDB.clone(connect="frontier://FrontierProd/CMS_CONDITIONS")
process.PoolDBESSourceOptics = cms.ESSource("PoolDBESSource",
	process.CondDBOptics,
	DumpStat=cms.untracked.bool(False),
	toGet=cms.VPSet(cms.PSet(
		record=cms.string("CTPPSOpticsRcd"),
		tag=cms.string("PPSOpticalFunctions_offline_v5")
	    )
    ),
)
process.esPreferDBFileOptics = cms.ESPrefer("PoolDBESSource","PoolDBESSourceOptics")

process.CondDBAlignment = CondDB.clone(connect="frontier://FrontierProd/CMS_CONDITIONS")
process.PoolDBESSourceAlignment = cms.ESSource("PoolDBESSource",
   process.CondDBAlignment,
   toGet = cms.VPSet(cms.PSet(
        record = cms.string("RPRealAlignmentRecord"),
        tag = cms.string("CTPPSRPAlignment_real_offline_v4") # always check on https://twiki.cern.ch/twiki/bin/view/CMS/TaggedProtonsRecommendations
        )
    )
)

process.esPreferDBFileAlignment = cms.ESPrefer("PoolDBESSource", "PoolDBESSourceAlignment")

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "/store/data/Run2018D/ZeroBias/AOD/12Nov2019_UL2018_rsb-v1/120000/27E825BA-972B-E04C-86BC-F38C53041087.root",
    ))

process.LightAnalyzer = cms.EDAnalyzer("LightAnalyzer",
    tagRecHit = cms.InputTag("ctppsDiamondRecHits"),
    tagLocalTrack = cms.InputTag("ctppsDiamondLocalTracks"),
    tagPixelLocalTrack = cms.InputTag("ctppsPixelLocalTracks"),
    lhcInfoLabel = cms.string(''),
    vertexTag = cms.InputTag("offlinePrimaryVerticesWithBS"),
    tagCalibrationFile = cms.string(options.calibFile),
    tagValidOOT = cms.int32(options.validOOT),
    minBX = cms.int32(options.minBX),
    maxBX = cms.int32(options.maxBX),
    Ntracks_Lcuts = cms.vint32([-1,1,-1,1]),
    Ntracks_Ucuts = cms.vint32([-1,6,-1,6])
)

process.TFileService = cms.Service("TFileService", fileName = cms.string(options.outputFile))

if options.useJSONCalibration == "y":
    # TODO - load calibration from JSON file to ctppsDiamondLocalReconstruction
    process.load("CondFormats.PPSObjects.ppsTimingCalibrationESSource_cfi")
    process.ppsTimingCalibrationESSource.calibrationFile = cms.FileInPath(options.calibFile)
    process.path = cms.Path(ctppsDiamondLocalReconstruction * process.LightAnalyzer)
else:
    process.path = cms.Path(process.LightAnalyzer)
process.schedule = cms.Schedule(process.path)
