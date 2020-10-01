#ifndef HISTOGRAM_CONSTANTS_LIGHT_ANALYZER_H
#define HISTOGRAM_CONSTANTS_LIGHT_ANALYZER_H

#include <string>

const std::string DIRECTORIES_PREFIX = "LightAnalyzer/";
const std::string GLOBAL_INFO_PATH = "GlobalInfo";
const std::string SECTOR_45_HISTOGRAM_PATH = "sector_45";
const std::string SECTOR_56_HISTOGRAM_PATH = "sector_56";

const std::string TRACK_TIME_HISTOGRAM_NAME = "Track time distribution";
const std::string TRACK_TIME_HISTOGRAM_LEGEND_SUFFIX = ";Track time [ns]";

const std::string TOT_VS_LS_HISTOGRAM_NAME = "ToT vs Lumisection";
const std::string TOT_VS_LS_HISTOGRAM_LEGEND_SUFFIX = ";Lumisection;ToT [ns]";

const std::string TRACK_TIME_VS_LS_HISTOGRAM_NAME = "Track time vs Lumisection";
const std::string TRACK_TIME_VS_LS_HISTOGRAM_LEGEND_SUFFIX = ";Lumisection;Track time [ns]";

const std::string TRACK_TIME_VS_BX_HISTOGRAM_NAME = "Track time vs Bunch crossing";
const std::string TRACK_TIME_VS_BX_HISTOGRAM_LEGEND_SUFFIX = ";Bunch crossing;Track time [ns]";

const std::string TRACK_TIME_VS_XANGLE_HISTOGRAM_NAME = "Track time vs Crossing angle";
const std::string TRACK_TIME_VS_XANGLE_HISTOGRAM_LEGEND_SUFFIX = ";Crossing angle [murad];Track time [ns]";

const std::string AV_VERTEX_Z_VS_AV_TRACK_TIME_HISTOGRAM_NAME = "Average vertex z position vs Average track time";
const std::string AV_VERTEX_Z_VS_AV_TRACK_TIME_HISTOGRAM_LEGEND_SUFFIX = ";Average track time [ns];Average vertex z position [cm]";

const std::string AV_VERTEX_Z_VS_XANGLE_HISTOGRAM_NAME = "Average vertex z position vs Crossing angle";
const std::string AV_VERTEX_Z_VS_XANGLE_HISTOGRAM_LEGEND = AV_VERTEX_Z_VS_XANGLE_HISTOGRAM_NAME + ";Crossing angle [murad];Average vertex z position [cm]";

const std::string XANGLE_VS_LS_HISTOGRAM_NAME = "Crossing angle vs Lumisection";
const std::string XANGLE_VS_LS_HISTOGRAM_LEGEND = XANGLE_VS_LS_HISTOGRAM_NAME + ";Lumisection;Crossing angle [murad]";

const std::string AV_VERTEX_Z_VS_LS_HISTOGRAM_NAME = "Average vertex z position vs Lumisection";
const std::string AV_VERTEX_Z_VS_LS_HISTOGRAM_LEGEND = AV_VERTEX_Z_VS_LS_HISTOGRAM_NAME + ";Lumisection;Average vertex z position [cm]";

const int LS_BINS = 4000;
const int LS_MIN = 0;
const int LS_MAX = 4000;

const int TOT_BINS = 200;
const int TOT_MIN = 5;
const int TOT_MAX = 25;

const int TRACK_TIME_BINS = 1000;
const int TRACK_TIME_MIN = -10;
const int TRACK_TIME_MAX = 10;

const int BX_BINS = 4000;
const int BX_MIN = 0;
const int BX_MAX = 4000;

const int XANGLE_BINS = 60;
const int XANGLE_MIN = 120;
const int XANGLE_MAX = 180;

const int VERTEX_Z_BINS = 1000;
const int VERTEX_Z_MIN = -10;
const int VERTEX_Z_MAX = 10;

std::string sectorIndexToNumber(int sectorIndex)
{
    switch (sectorIndex) {
        case 0:
            return "45";
        case 1:
            return "56";
        default:
            return "invalid";
    }
}

std::string makeSectorHistogramTitle(const std::string& titlePrefix, int sectorIndex)
{
    return titlePrefix + " sector " + sectorIndexToNumber(sectorIndex);
}

std::string makeSectorHistogramLegend(const std::string& legendPrefix, const std::string& legendSuffix, int sectorIndex)
{
    return makeSectorHistogramTitle(legendPrefix, sectorIndex) + legendSuffix;
}

std::string makeChannelHistogramTitle(const std::string& titlePrefix, int sectorIndex, int planeIndex, int channelIndex)
{
    return titlePrefix + " sector " + sectorIndexToNumber(sectorIndex) + " plane " + std::to_string(planeIndex) + " channel " + std::to_string(channelIndex);
}

std::string makeChannelHistogramLegend(const std::string& legendPrefix, const std::string& legendSuffix, int sectorIndex, int planeIndex, int channelIndex)
{
    return makeChannelHistogramTitle(legendPrefix, sectorIndex, planeIndex, channelIndex) + legendSuffix;
}

std::string makePlaneDirectoryName(int planeIndex) {
    return "plane " + std::to_string(planeIndex);
}

std::string makeChannelDirectoryName(int channelIndex)
{
    return "channel " + std::to_string(channelIndex);
}

#endif