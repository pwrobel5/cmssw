#ifndef HISTOGRAM_CONSTANTS_LIGHT_ANALYZER_H
#define HISTOGRAM_CONSTANTS_LIGHT_ANALYZER_H

#include <string>

const char* SECTOR_45_HISTOGRAM_PATH = "sector_45";
const char* SECTOR_56_HISTOGRAM_PATH = "sector_56";

const std::string TOT_VS_LS_HISTOGRAM_NAME = "ToT vs LS";
const std::string TOT_VS_LS_HISTOGRAM_LEGEND_SUFFIX = ";LS;ToT [ns]";

const int LS_BINS = 4000;
const int LS_MIN = 0;
const int LS_MAX = 4000;

const int TOT_BINS = 200;
const int TOT_MIN = 5;
const int TOT_MAX = 25;

#endif