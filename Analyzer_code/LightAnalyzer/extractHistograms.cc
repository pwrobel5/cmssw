#include "extractHistograms.h"
#include "plugins/HistogramConstants.h"

void extractHistograms() {
    std::vector<std::string> directories = getCrabDirectoriesList();
    auto fitResults = processResults(directories);
    saveFitData(fitResults);
}

std::vector<std::string> getCrabDirectoriesList() {
    return listFilesMatchingRegex(BASE_DIRECTORY, TPRegexp("crab_\\w*"));
}

std::vector<std::string> listFilesMatchingRegex(std::string dirName, TPRegexp regex) {
    std::vector<std::string> result;
    
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(dirName.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string fileName = ent->d_name;
            if (regex.MatchB(fileName) == kTRUE) {
                result.push_back(fileName);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error with opening directory";
    }

    return result;
}

std::map<int, std::vector<fitData>> processResults(std::vector<std::string>& directories) {
    std::map<int, std::vector<fitData>> sectorsFitData;
    for (const std::string& directory : directories) {        
        runData runData = initializeRunData(directory);
        loadHistograms(directory, runData);
        createProfiles(runData);
        fillFitData(directory, runData, sectorsFitData);
        saveFile(directory, runData);
        cleanMemory(runData);
    }

    return sectorsFitData;
}

runData initializeRunData(const std::string& directoryName) {
    runData result;

    result.sectorDataMap[SECTOR_45] = initializeSectorData(SECTOR_45);
    result.sectorDataMap[SECTOR_56] = initializeSectorData(SECTOR_56);
    result.globalData = initializeGlobalData();

    return result;
}

sectorData initializeSectorData(int sectorNumber) {
    sectorData result;

    initializeSectorHistograms(sectorNumber, result.histograms);
    for (int i = 0; i < MAX_PLANE_NUMBER; i++) {
        result.planeDataMap[i] = initializePlaneData(sectorNumber, i);
    }

    return result;
}

void initializeSectorHistograms(int sectorNumber, histograms& histograms) {
    histograms.TOTvsLS = new TH2F(
        makeSectorHistogramTitle(TOT_VS_LS_HISTOGRAM_NAME, sectorNumber).c_str(), 
        makeSectorHistogramLegend(TOT_VS_LS_HISTOGRAM_NAME, TOT_VS_LS_HISTOGRAM_LEGEND_SUFFIX, sectorNumber).c_str(), 
        LS_BINS, LS_MIN, LS_MAX, TOT_BINS, TOT_MIN, TOT_MAX
    );
    histograms.TimeRAWvsLS = new TH2F(
        makeSectorHistogramTitle(TRACK_TIME_VS_LS_HISTOGRAM_NAME, sectorNumber).c_str(),
        makeSectorHistogramLegend(TRACK_TIME_VS_LS_HISTOGRAM_NAME, TRACK_TIME_VS_LS_HISTOGRAM_LEGEND_SUFFIX, sectorNumber).c_str(),
        LS_BINS, LS_MIN, LS_MAX, TRACK_TIME_BINS, TRACK_TIME_MIN, TRACK_TIME_MAX
    );
    histograms.TimeRAWvsBX = new TH2F(
        makeSectorHistogramTitle(TRACK_TIME_VS_BX_HISTOGRAM_NAME, sectorNumber).c_str(),
        makeSectorHistogramLegend(TRACK_TIME_VS_BX_HISTOGRAM_NAME, TRACK_TIME_VS_BX_HISTOGRAM_LEGEND_SUFFIX, sectorNumber).c_str(),
        BX_BINS, BX_MIN, BX_MAX, TRACK_TIME_BINS, TRACK_TIME_MIN, TRACK_TIME_MAX
    );
    histograms.TimeRAWvsXAngle = new TH2F(
        makeSectorHistogramTitle(TRACK_TIME_VS_XANGLE_HISTOGRAM_NAME, sectorNumber).c_str(),
        makeSectorHistogramLegend(TRACK_TIME_VS_XANGLE_HISTOGRAM_NAME, TRACK_TIME_VS_XANGLE_HISTOGRAM_LEGEND_SUFFIX, sectorNumber).c_str(),
        XANGLE_BINS, XANGLE_MIN, XANGLE_MAX, TRACK_TIME_BINS, TRACK_TIME_MIN, TRACK_TIME_MAX
    );
    histograms.AvVertexZvsAvTimeRAW = new TH2F(
        makeSectorHistogramTitle(AV_VERTEX_Z_VS_AV_TRACK_TIME_HISTOGRAM_NAME, sectorNumber).c_str(),
        makeSectorHistogramLegend(AV_VERTEX_Z_VS_AV_TRACK_TIME_HISTOGRAM_NAME, AV_VERTEX_Z_VS_AV_TRACK_TIME_HISTOGRAM_LEGEND_SUFFIX, sectorNumber).c_str(),
        TRACK_TIME_BINS, TRACK_TIME_MIN, TRACK_TIME_MAX, VERTEX_Z_BINS, VERTEX_Z_MIN, VERTEX_Z_MAX
    );
}

planeData initializePlaneData(int sectorNumber, int planeNumber) {
    planeData result;

    for (int i = 0; i < MAX_CHANNEL_NUMBER; i++) {
        result.channelDataMap[i] = initializeChannelData(sectorNumber, planeNumber, i);
    }

    return result;
}

channelData initializeChannelData(int sectorNumber, int planeNumber, int channelNumber) {
    channelData result;
    result.TOTvsLS = new TH2F(
        makeChannelHistogramTitle(TOT_VS_LS_HISTOGRAM_NAME, sectorNumber, planeNumber, channelNumber).c_str(),
        makeChannelHistogramLegend(TOT_VS_LS_HISTOGRAM_NAME, TOT_VS_LS_HISTOGRAM_LEGEND_SUFFIX, sectorNumber, planeNumber, channelNumber).c_str(),
        LS_BINS, LS_MIN, LS_MAX, TOT_BINS, TOT_MIN, TOT_MAX
    );
    return result;
}

globalData initializeGlobalData() {
    globalData result;

    result.AvVertexZvsXAngle = new TH2F(
        AV_VERTEX_Z_VS_XANGLE_HISTOGRAM_NAME.c_str(),
        AV_VERTEX_Z_VS_XANGLE_HISTOGRAM_LEGEND.c_str(),
        XANGLE_BINS, XANGLE_MIN, XANGLE_MAX, VERTEX_Z_BINS, VERTEX_Z_MIN, VERTEX_Z_MAX
    );
    result.XAnglevsLS = new TH2F(
        XANGLE_VS_LS_HISTOGRAM_NAME.c_str(),
        XANGLE_VS_LS_HISTOGRAM_LEGEND.c_str(),
        LS_BINS, LS_MIN, LS_MAX, XANGLE_BINS, XANGLE_MIN, XANGLE_MAX
    );
    result.AvVertexZvsLS = new TH2F(
        AV_VERTEX_Z_VS_LS_HISTOGRAM_NAME.c_str(),
        AV_VERTEX_Z_VS_LS_HISTOGRAM_LEGEND.c_str(),
        LS_BINS, LS_MIN, LS_MAX, VERTEX_Z_BINS, VERTEX_Z_MIN, VERTEX_Z_MAX
    );

    return result;
}

void loadHistograms(std::string directory, runData& runData) {
    std::vector<std::string> rootFiles = getRootFilesList(directory);

    for (const std::string& rootFileName : rootFiles) {
        processRootFile(rootFileName, runData);
    }
}

std::vector<std::string> getRootFilesList(std::string directory) {
    std::vector<std::string> result;

    std::string outputDirectory = BASE_DIRECTORY + directory + "/results/";
    std::vector<std::string> outputs = listFilesMatchingRegex(outputDirectory, TPRegexp("\\w*.root"));

    for(const std::string& fileName : outputs) {
        result.push_back(outputDirectory + fileName);
    }

    return result;
}

void processRootFile(const std::string& rootFileName, runData& runData) {
    TFile inputFile(rootFileName.c_str());

    addGlobalData(inputFile, runData.globalData);

    addSectorData(inputFile, DIRECTORIES_PREFIX + SECTOR_45_HISTOGRAM_PATH + "/", runData.sectorDataMap[SECTOR_45], SECTOR_45);
    addSectorData(inputFile, DIRECTORIES_PREFIX + SECTOR_56_HISTOGRAM_PATH + "/", runData.sectorDataMap[SECTOR_56], SECTOR_56);

    inputFile.Close();
}

void addGlobalData(TFile& inputFile, globalData& globalData) {
    addDataFromHistogram(inputFile, DIRECTORIES_PREFIX + GLOBAL_INFO_PATH + "/" + AV_VERTEX_Z_VS_XANGLE_HISTOGRAM_NAME, globalData.AvVertexZvsXAngle);
    addDataFromHistogram(inputFile, DIRECTORIES_PREFIX + GLOBAL_INFO_PATH + "/" + XANGLE_VS_LS_HISTOGRAM_NAME, globalData.XAnglevsLS);
    addDataFromHistogram(inputFile, DIRECTORIES_PREFIX + GLOBAL_INFO_PATH + "/" + AV_VERTEX_Z_VS_LS_HISTOGRAM_NAME, globalData.AvVertexZvsLS);
}

void addSectorData(TFile& inputFile, const std::string& sectorPath, sectorData& sectorData, int sectorNumber) {
    histograms histograms = sectorData.histograms;

    addDataFromHistogram(inputFile, sectorPath + makeSectorHistogramTitle(TOT_VS_LS_HISTOGRAM_NAME, sectorNumber), histograms.TOTvsLS);
    addDataFromHistogram(inputFile, sectorPath + makeSectorHistogramTitle(TRACK_TIME_VS_LS_HISTOGRAM_NAME, sectorNumber), histograms.TimeRAWvsLS);
    addDataFromHistogram(inputFile, sectorPath + makeSectorHistogramTitle(TRACK_TIME_VS_BX_HISTOGRAM_NAME, sectorNumber), histograms.TimeRAWvsBX);
    addDataFromHistogram(inputFile, sectorPath + makeSectorHistogramTitle(TRACK_TIME_VS_XANGLE_HISTOGRAM_NAME, sectorNumber), histograms.TimeRAWvsXAngle);
    addDataFromHistogram(inputFile, sectorPath + makeSectorHistogramTitle(AV_VERTEX_Z_VS_AV_TRACK_TIME_HISTOGRAM_NAME, sectorNumber), histograms.AvVertexZvsAvTimeRAW);

    for (int i = 0; i < MAX_PLANE_NUMBER; i++) {
        //addPlaneData(inputFile, sectorPath + makePlaneDirectoryName(i), sectorData.planeDataMap[i], sectorNumber, i);
    }
}

void addPlaneData(TFile& inputFile, const std::string& planePath, planeData& planeData, int sectorNumber, int planeNumber) {
    if (inputFile.Get(planePath.c_str()) != nullptr) {
        for (int i = 0; i < MAX_CHANNEL_NUMBER; i++) {
            addChannelData(inputFile, planePath + "/" + makeChannelDirectoryName(i), planeData.channelDataMap[i], sectorNumber, planeNumber, i);
        }
    }
}

void addChannelData(TFile& inputFile, const std::string& channelPath, channelData& channelData, int sectorNumber, int planeNumber, int channelNumber) {
    if (inputFile.Get(channelPath.c_str()) != nullptr) {
        addDataFromHistogram(inputFile, channelPath + "/" + makeChannelHistogramTitle(TOT_VS_LS_HISTOGRAM_NAME, sectorNumber, planeNumber, channelNumber), channelData.TOTvsLS);
    }
}

void addDataFromHistogram(TFile& inputFile, const std::string& histogramPath, TH2F* histogram) {
    std::cout << "Trying to read " << histogramPath << std::endl;
    TH2F* readHistogram = (TH2F*) inputFile.Get(histogramPath.c_str());
    histogram->Add(readHistogram);
}

void createProfiles(runData& runData) {
    createGlobalProfiles(runData.globalData);

    for (auto& sectorDataPair : runData.sectorDataMap) {
        createProfilesForSector(sectorDataPair.second);
    }
}

void createGlobalProfiles(globalData& globalData) {
    globalData.AvVertexZvsXAngleProfile = new TProfile(*(globalData.AvVertexZvsXAngle->ProfileX("_pfx", 1, -1)));
    globalData.AvVertexZvsLSProfile = new TProfile(*(globalData.AvVertexZvsLS->ProfileX("_pfx", 1, -1)));
}

void createProfilesForSector(sectorData& sectorData) {
    histograms histograms = sectorData.histograms;

    sectorData.profiles.TOTvsLSProfile = new TProfile(*(histograms.TOTvsLS->ProfileX("_pfx", 1, -1)));
    sectorData.profiles.TimeRAWvsLSProfile = new TProfile(*(histograms.TimeRAWvsLS->ProfileX("_pfx", 1, -1)));
    sectorData.profiles.TimeRAWvsBXProfile = new TProfile(*(histograms.TimeRAWvsBX->ProfileX("_pfx", 1, -1)));
    sectorData.profiles.TimeRAWvsXAngle = new TProfile(*(histograms.TimeRAWvsXAngle->ProfileX("_pfx", 1, -1)));
    sectorData.profiles.AvVertexZvsAvTimeRAWProfile = new TProfile(*(histograms.AvVertexZvsAvTimeRAW->ProfileX("_pfx", 1, -1)));

    sectorData.profiles.TimeRAWvsBXProfile->Fit("1 ++ x");

    for (int i = 0; i < MAX_PLANE_NUMBER; i++) {
        createProfilesForPlane(sectorData.planeDataMap[i]);
    }
}

void createProfilesForPlane(planeData& planeData) {
    for (int i = 0; i < MAX_CHANNEL_NUMBER; i++) {
        createProfilesForChannel(planeData.channelDataMap[i]);
    }
}

void createProfilesForChannel(channelData& channelData) {
    channelData.TOTvsLSProfile = new TProfile(*(channelData.TOTvsLS->ProfileX("_pfx", 1, -1)));
}

void fillFitData(std::string directoryName, runData& runData, std::map<int, std::vector<fitData>>& sectorsFitData) {
    int runNumber = getRunNumberFromDirectory(directoryName);

    sectorsFitData[SECTOR_45].push_back(readSectorFitData(runData.sectorDataMap[SECTOR_45], runNumber));
    sectorsFitData[SECTOR_56].push_back(readSectorFitData(runData.sectorDataMap[SECTOR_56], runNumber));
}

fitData readSectorFitData(sectorData& sectorData, int runNumber) {
    fitData result;

    result.runNumber = runNumber;
    result.intercept = sectorData.profiles.TimeRAWvsBXProfile->GetFunction("1 ++ x")->GetParameter(0);
    result.slope = sectorData.profiles.TimeRAWvsBXProfile->GetFunction("1 ++ x")->GetParameter(1);

    return result;
}

int getRunNumberFromDirectory(const std::string& directoryName) {
    std::string runName = getRunNameFromDirectory(directoryName);
    std::string toRemove = "Run";

    std::string::size_type iterator = runName.find(toRemove);
    if (iterator != std::string::npos)
        runName.erase(iterator, toRemove.length());

    return std::stoi(runName);
}

void saveFile(const std::string& directoryName, runData& runData) {
    std::string outputFileName = getRunNameFromDirectory(directoryName) + ".root";
    TFile outputFile(outputFileName.c_str(), "RECREATE");
    TDirectory* sector45Dir = outputFile.mkdir(SECTOR_45_HISTOGRAM_PATH.c_str());
    TDirectory* sector56Dir = outputFile.mkdir(SECTOR_56_HISTOGRAM_PATH.c_str());
    TDirectory* globalDataDir = outputFile.mkdir(GLOBAL_INFO_PATH.c_str());

    saveSectorHistograms(sector45Dir, runData.sectorDataMap[SECTOR_45]);
    saveSectorHistograms(sector56Dir, runData.sectorDataMap[SECTOR_56]);
    saveGlobalData(globalDataDir, runData.globalData);

    outputFile.Write();
    outputFile.Close();
}

std::string getRunNameFromDirectory(const std::string& directoryName) {
    TObjArray* objArray = TPRegexp("Run(\\d+)").MatchS(directoryName);
    if (objArray->GetLast() < 0) {
        return "";
    } else {
        return ((TObjString*) objArray->At(0))->GetString().Data();
    }
}

void saveSectorHistograms(TDirectory* directory, sectorData& sectorData) {
    directory->cd();

    sectorData.histograms.TOTvsLS->Write();
    sectorData.histograms.TimeRAWvsLS->Write();
    sectorData.histograms.TimeRAWvsBX->Write();
    sectorData.histograms.TimeRAWvsXAngle->Write();
    sectorData.histograms.AvVertexZvsAvTimeRAW->Write();

    sectorData.profiles.TOTvsLSProfile->Write();
    sectorData.profiles.TimeRAWvsLSProfile->Write();
    sectorData.profiles.TimeRAWvsBXProfile->Write();
    sectorData.profiles.TimeRAWvsXAngle->Write();
    sectorData.profiles.AvVertexZvsAvTimeRAWProfile->Write();

    for (int i = 0; i < MAX_PLANE_NUMBER; i++) {
        std::string directoryName = makePlaneDirectoryName(i);
        TDirectory* planeDirectory = directory->mkdir(directoryName.c_str());
        savePlaneHistograms(planeDirectory, sectorData.planeDataMap[i]);
    }
}

void savePlaneHistograms(TDirectory* directory, planeData& planeData) {
    directory->cd();

    for (int i = 0; i < MAX_CHANNEL_NUMBER; i++) {
        std::string directoryName = makeChannelDirectoryName(i);
        TDirectory* channelDirectory = directory->mkdir(directoryName.c_str());
        saveChannelHistograms(channelDirectory, planeData.channelDataMap[i]);
    }
}

void saveChannelHistograms(TDirectory* directory, channelData& channelData) {
    directory->cd();
    channelData.TOTvsLS->Write();
    channelData.TOTvsLSProfile->Write();
}

void saveGlobalData(TDirectory* directory, globalData& globalData) {
    directory->cd();

    globalData.AvVertexZvsXAngle->Write();
    globalData.XAnglevsLS->Write();
    globalData.AvVertexZvsLS->Write();

    globalData.AvVertexZvsXAngleProfile->Write();
    globalData.AvVertexZvsLSProfile->Write();
}

void cleanMemory(runData& runData) {
    cleanGlobalData(runData.globalData);

    cleanSectorData(runData.sectorDataMap[SECTOR_45]);
    cleanSectorData(runData.sectorDataMap[SECTOR_56]);    
}

void cleanGlobalData(globalData& globalData) {
    delete globalData.AvVertexZvsXAngle;
    delete globalData.XAnglevsLS;
    delete globalData.AvVertexZvsLS;

    delete globalData.AvVertexZvsXAngleProfile;
    delete globalData.AvVertexZvsLSProfile;
}

void cleanSectorData(sectorData& sectorData) {
    cleanHistograms(sectorData.histograms);
    cleanProfiles(sectorData.profiles);

    for (int i = 0; i < MAX_PLANE_NUMBER; i++) {
        cleanPlaneData(sectorData.planeDataMap[i]);
    }
}

void cleanHistograms(histograms& histograms) {
    delete histograms.TimeRAWvsBX;
    delete histograms.TimeRAWvsLS;
    delete histograms.TimeRAWvsXAngle;
    delete histograms.TOTvsLS;
    delete histograms.AvVertexZvsAvTimeRAW;
}

void cleanProfiles(profiles& profiles) {
    delete profiles.TimeRAWvsBXProfile;
    delete profiles.TimeRAWvsLSProfile;
    delete profiles.TimeRAWvsXAngle;
    delete profiles.TOTvsLSProfile;
    delete profiles.AvVertexZvsAvTimeRAWProfile;
}

void cleanPlaneData(planeData& planeData) {
    for (int i = 0; i < MAX_CHANNEL_NUMBER; i++) {
        cleanChannelData(planeData.channelDataMap[i]);
    }
}

void cleanChannelData(channelData& channelData) {
    delete channelData.TOTvsLS;
    delete channelData.TOTvsLSProfile;
}

void saveFitData(std::map<int, std::vector<fitData>>& sectorsFitData) {
    TFile fitDataOutputFile("fitData.root", "RECREATE");

    TDirectory* sector45Directory = fitDataOutputFile.mkdir(SECTOR_45_HISTOGRAM_PATH.c_str());
    TDirectory* sector56Directory = fitDataOutputFile.mkdir(SECTOR_56_HISTOGRAM_PATH.c_str());

    saveSectorFitData(sector45Directory, sectorsFitData[SECTOR_45], SECTOR_45);
    saveSectorFitData(sector56Directory, sectorsFitData[SECTOR_56], SECTOR_56);

    fitDataOutputFile.Write();
    fitDataOutputFile.Close();
}

void saveSectorFitData(TDirectory* directory, std::vector<fitData>& sectorFitData, int sectorNumber) {
    directory->cd();

    std::sort(sectorFitData.begin(), sectorFitData.end(), compareFitData);
    Double_t intercepts[sectorFitData.size()];
    Double_t slopes[sectorFitData.size()];
    Double_t runNumbers[sectorFitData.size()];

    for (int i = 0; i < sectorFitData.size(); i++) {
        intercepts[i] = sectorFitData[i].intercept;
        slopes[i] = sectorFitData[i].slope;
        runNumbers[i] = sectorFitData[i].runNumber;
    }

    TGraph* interceptGraph = new TGraph(sectorFitData.size(), runNumbers, intercepts);
    interceptGraph->SetNameTitle(("intercepts sector " + std::to_string(sectorNumber)).c_str(), "Intercepts;Run number;Intercept");
    interceptGraph->Write();

    TGraph* slopeGraph = new TGraph(sectorFitData.size(), runNumbers, slopes);
    slopeGraph->SetNameTitle(("slopes sector " + std::to_string(sectorNumber)).c_str(), "Slopes;Run number;Slope");
    slopeGraph->Write();
}

bool compareFitData(fitData& fitData1, fitData& fitData2) {
    return fitData1.runNumber < fitData2.runNumber;
}
