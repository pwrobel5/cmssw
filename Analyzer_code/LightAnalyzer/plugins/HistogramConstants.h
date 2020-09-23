#ifndef HISTOGRAM_CONSTANTS_LIGHT_ANALYZER_H
#define HISTOGRAM_CONSTANTS_LIGHT_ANALYZER_H

#include <string>

const char* SECTOR_45_HISTOGRAM_PATH = "sector_45";
const char* SECTOR_56_HISTOGRAM_PATH = "sector_56";

const std::string TOT_VS_LS_HISTOGRAM_NAME = "ToT vs Lumisection";
const std::string TOT_VS_LS_HISTOGRAM_LEGEND_SUFFIX = ";Lumisection;ToT [ns]";

const std::string TRACK_TIME_VS_LS_HISTOGRAM_NAME = "Track time vs Lumisection";
const std::string TRACK_TIME_VS_LS_HISTOGRAM_LEGEND_SUFFIX = ";Lumisection;Track time [ns]";

const std::string TRACK_TIME_VS_BX_HISTOGRAM_NAME = "Track time vs Bunch crossing";
const std::string TRACK_TIME_VS_BX_HISTOGRAM_LEGEND_SUFFIX = ";Bunch crossing;Track time [ns]";

const std::string TRACK_TIME_VS_XANGLE_HISTOGRAM_NAME = "Track time vs Crossing angle";
const std::string TRACK_TIME_VS_XANGLE_HISTOGRAM_LEGEND_SUFFIX = ";Crossing angle [murad];Track time [ns]";

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

#endif