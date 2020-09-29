#ifndef EXTRACT_HISTOGRAMS_H
#define EXTRACT_HISTOGRAMS_H

extern "C" {
    #include <dirent.h>
}

#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TFile.h"
#include "TPRegexp.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TGraph.h"

enum sectorNumber {
    SECTOR_45,
    SECTOR_56
};

struct histograms {
    TH2F* TOTvsLS;
    TH2F* TimeRAWvsLS;
    TH2F* TimeRAWvsBX;
    TH2F* TimeRAWvsXAngle;
    TH2F* AvVertexZvsAvTimeRAW;
};

struct profiles {
    TProfile* TOTvsLSProfile;
    TProfile* TimeRAWvsLSProfile;
    TProfile* TimeRAWvsBXProfile;
    TProfile* TimeRAWvsXAngle;
    TProfile* AvVertexZvsAvTimeRAWProfile;
};

struct channelData {
    TH2F* TOTvsLS;
    TProfile* TOTvsLSProfile;
};

struct planeData {
    std::map<int, channelData> channelDataMap;
};

struct sectorData {
    histograms histograms;
    profiles profiles;
    std::map<int, planeData> planeDataMap;
};

struct globalData {
    TH2F* AvVertexZvsXAngle;
    TH2F* XAnglevsLS;
    TH2F* AvVertexZvsLS;
    TProfile* AvVertexZvsLSProfile;
    TProfile* AvVertexZvsXAngleProfile;
};

struct runData {
    std::map<sectorNumber, sectorData> sectorDataMap;
    globalData globalData;
};

struct fitData {
    int runNumber;
    double intercept;
    double slope;
};

const std::string BASE_DIRECTORY = "../../../crab_test/test_files/";

// TODO - set to 4 and 12
const int MAX_PLANE_NUMBER = 3;
const int MAX_CHANNEL_NUMBER = 11;


std::vector<std::string> getCrabDirectoriesList();
std::vector<std::string> listFilesMatchingRegex(std::string dirName, TPRegexp regex);

std::map<int, std::vector<fitData>> processResults(std::vector<std::string>& directories);

void loadHistograms(std::string directory, runData& runData);
std::vector<std::string> getRootFilesList(std::string directory);

runData initializeRunData(const std::string& directoryName);
sectorData initializeSectorData(int sectorNumber);
void initializeSectorHistograms(int sectorNumber, histograms& histograms);
planeData initializePlaneData(int sectorNumber, int planeNumber);
channelData initializeChannelData(int sectorNumber, int planeNumber, int channelNumber);
globalData initializeGlobalData();

void processRootFile(const std::string& rootFileName, runData& runData);
void addGlobalData(TFile& inputFile, globalData& globalData);
void addSectorData(TFile& inputFile, const std::string& sectorPath, sectorData& sectorData, int sectorNumber);
void addPlaneData(TFile& inputFile, const std::string& planePath, planeData& planeData, int sectorNumber, int planeNumber);
void addChannelData(TFile& inputFile, const std::string& channelPath, channelData& channelData, int sectorNumber, int planeNumber, int channelNumber);
void addDataFromHistogram(TFile& inputFile, const std::string& histogramPath, TH2F* histogram);

void createProfiles(runData& runData);
void createGlobalProfiles(globalData& globalData);
void createProfilesForSector(sectorData& sectorData);
void createProfilesForPlane(planeData& planeData);
void createProfilesForChannel(channelData& channelData);

void fillFitData(std::string directoryName, runData& runData, std::map<int, std::vector<fitData>>& sectorsFitData);
fitData readSectorFitData(sectorData& sectorData, int runNumber);
int getRunNumberFromDirectory(const std::string& directoryName);

void saveFile(const std::string& directoryName, runData& runData);
std::string getRunNameFromDirectory(const std::string& directoryName);
void saveSectorHistograms(TDirectory* directory, sectorData& sectorData);
void savePlaneHistograms(TDirectory* directory, planeData& planeData);
void saveChannelHistograms(TDirectory* directory, channelData& channelData);
void saveGlobalData(TDirectory* directory, globalData& globalData);

void cleanMemory(runData& runData);
void cleanGlobalData(globalData& globalData);
void cleanSectorData(sectorData& sectorData);
void cleanHistograms(histograms& histograms);
void cleanProfiles(profiles& profiles);
void cleanPlaneData(planeData& planeData);
void cleanChannelData(channelData& channelData);

void saveFitData(std::map<int, std::vector<fitData>>& sectorsFitData);
void saveSectorFitData(TDirectory* directory, std::vector<fitData>& sectorFitData, int sectorNumber);
bool compareFitData(fitData& fitData1, fitData& fitData2);

#endif
