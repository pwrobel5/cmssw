# LightAnalyzer

Tool developed to extract important information from 2018 experimental data in UL mode.

# Produced histograms

1. Global data:

* Average vertex z position vs crossing angle + profile
* Average vertex z position vs lumisection + profile
* Crossing angle vs lumisection

2. For every sector:

* Track time vs lumisection + profile
* ToT vs lumisection + profile
* Track time vs bunch crossing + profile with linear fit
* Track time vs crossing angle + profile
* Average vertex z position vs average track time + profile

3. For every channel:

* ToT vs lumisection + profile for every channel

# Compiling and running

1. Compiling
```
scram b
```

2. Running test example
```
cmsRun Analyzer_code/LightAnalyzer/python/test.py geometryFile=Geometry.VeryForwardGeometry.geometryRPFromDD_2018_cfi validOOT=-1 outputFile=test.root
```

3. Test script parameters:

* outputFile - name of the output containing all produced histograms
* geometryFile - name of module with detector geometry, default Geometry.VeryForwardGeometry.geometryRPFromDD_2018_cfi
* validOOT - OOT parameter, default -1
* calibFile - path to calibration JSON file (used if needed), default to blank string
* minBX - minimal value of bunch crossing saved in histograms, default 0
* maxBX - maximal value of bunch crossing saved in histograms, default 4000
* useJSONCalibration - if set to "y", then timing data is calibrated using file set in calibFile parameter, default "n"

# Contained files

* directory plugins:
    * DiamondDetectorClass.cc, DiamondDetectorClass.h - auxiliary class extracting i.e. ToT from data file
    * LightAnalyzer.cc, LightAnalyzer.h - main module file
    * HistogramConstants.h - header containg constants used in creation of histograms, separate from LightAnalyzer.h to use it in script for CRAB output extraction (described below)
* directory python:
    * test.py - script used to run analyzer job
* extractHistograms.cc, extractHistograms.h - ROOT script for extracting data from CRAB jobs output files and merging results within the scope of a single run, the path to the directory from which CRAB jobs have been submitted has to be set in the header file in parameter BASE_DIRECTORY. For every run output file named Run[run_number].root is created. In addition there is also produced fitData.root file which contains dependencies on run number for parameters of linear fit for time track vs bunch crossing profiles with respect to sector numbers