#ifndef BOSS__DIAMONDDETECTORCLASS__LIBGUARD
#define BOSS__DIAMONDDETECTORCLASS__LIBGUARD


// system include files
#include <memory> 

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
// user include files
//#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"



#include "DataFormats/CTPPSDigi/interface/CTPPSDiamondDigi.h"
#include "DataFormats/CTPPSDetId/interface/CTPPSDiamondDetId.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondRecHit.h"
#include "DataFormats/CTPPSReco/interface/CTPPSDiamondLocalTrack.h"

#include "TF1.h"
 
#include <map>




struct Correction_Map{
    static std::vector< std::map<std::pair<int,int>,std::vector<double> > > create_correction_mapV() // plane, channel
        {
          std::vector< std::map<std::pair<int,int>,std::vector<double> > > m(2);
		  m[0][std::make_pair(0,0)] = std::vector<double> {0.0,0.0,0.0,0.0,0.0,0.0};
		  m[0][std::make_pair(0,1)] = std::vector<double> {0.0,0.0,0.0,0.0,0.0,0.0};
		  m[0][std::make_pair(0,2)] = std::vector<double> {0.0,0.0,2.006,14.34,1.505,4.63};
		  m[0][std::make_pair(0,3)] = std::vector<double> {0.0,0.0,1.127,11.55,0.6548,5.272};
		  m[0][std::make_pair(0,4)] = std::vector<double> {0.0,0.0,1.191,11.86,0.7645,5.306};
		  m[0][std::make_pair(0,5)] = std::vector<double> {0.0,0.0,1.557,14.28,1.022,4.963};
		  m[0][std::make_pair(0,6)] = std::vector<double> {0.0,0.0,2.067,14.33,1.362,4.674};
		  m[0][std::make_pair(0,7)] = std::vector<double> {0.0,0.0,1.178,12.04,0.6804,5.165};
		  m[0][std::make_pair(0,8)] = std::vector<double> {0.0,0.0,0.8463,11.67,0.5013,5.311};
		  m[0][std::make_pair(0,9)] = std::vector<double> {0.0,0.0,1.027,13.92,0.8086,4.901};
		  m[0][std::make_pair(0,10)] = std::vector<double>{0.0,0.0,1.649,11.59,1.155,5.103};
		  m[0][std::make_pair(0,11)] = std::vector<double>{0.0,0.0,2.308,11.02,0.8602,5.158};
		  m[0][std::make_pair(1,0)] = std::vector<double> {0.0,0.0,1.915,12.3,1.26,5.161};
		  m[0][std::make_pair(1,1)] = std::vector<double> {0.0,0.0,1.03,11.87,0.5981,5.621};
		  m[0][std::make_pair(1,2)] = std::vector<double> {0.0,0.0,0.9698,11.79,0.5261,5.741};
		  m[0][std::make_pair(1,3)] = std::vector<double> {0.0,0.0,1.57,11.72,0.7466,4.77};
		  m[0][std::make_pair(1,4)] = std::vector<double> {0.0,0.0,0.9819,12.21,0.5467,4.93};
		  m[0][std::make_pair(1,5)] = std::vector<double> {0.0,0.0,2.326,11.65,0.8615,5.738};
		  m[0][std::make_pair(1,6)] = std::vector<double> {0.0,0.0,1.1815,12.65,0.6459,5.525};
		  m[0][std::make_pair(1,7)] = std::vector<double> {0.0,0.0,1.523,12.42,0.8805,4.582};
		  m[0][std::make_pair(1,8)] = std::vector<double> {0.0,0.0,2.455,11.17,0.8015,4.774};
		  m[0][std::make_pair(1,9)] = std::vector<double> {0.0,0.0,1.415,11.61,0.7076,5.533};
		  m[0][std::make_pair(1,10)] = std::vector<double>{0.0,0.0,12950,-1.953,1.384,4.817};
		  m[0][std::make_pair(1,11)] = std::vector<double>{0.0,0.0,1.512,11.76,1.039,4.592};
		  m[0][std::make_pair(2,0)] = std::vector<double> {0.0,0.0,2.34,11.23,0.8874,4.048};
		  m[0][std::make_pair(2,1)] = std::vector<double> {0.0,0.0,2.717,10.23,1.356,4.343};
		  m[0][std::make_pair(2,2)] = std::vector<double> {0.0,0.0,4.953,9.694,0.8014,4.372};
		  m[0][std::make_pair(2,3)] = std::vector<double> {0.0,0.0,1.807,11.57,0.6542,4.783};
		  m[0][std::make_pair(2,4)] = std::vector<double> {0.0,0.0,1.823,11.97,1.143,4.638};
		  m[0][std::make_pair(2,5)] = std::vector<double> {0.0,0.0,1.987,11.47,0.6343,4.526};
		  m[0][std::make_pair(2,6)] = std::vector<double> {0.0,0.0,1.555,12.04,0.6252,4.535};
		  m[0][std::make_pair(2,7)] = std::vector<double> {0.0,0.0,3.029,11.03,1.408,4.506};
		  m[0][std::make_pair(2,8)] = std::vector<double> {0.0,0.0,1.282,12.04,0.6962,4.713};
		  m[0][std::make_pair(2,9)] = std::vector<double> {0.0,0.0,2.091,10.8,0.6909,4.295};
		  m[0][std::make_pair(2,10)] = std::vector<double>{0.0,0.0,1.706,11.44,1.209,4.801};
		  m[0][std::make_pair(2,11)] = std::vector<double>{0.0,0.0,2.433,11.73,1.38,4.326};
		  m[0][std::make_pair(3,0)] = std::vector<double> {0.0,0.0,2.612,11.8,1.677,4.24};
		  m[0][std::make_pair(3,1)] = std::vector<double> {0.0,0.0,2.253,11.38,1.792,4.782};
		  m[0][std::make_pair(3,2)] = std::vector<double> {0.0,0.0,1.425,11.3,0.8377,4.982};
		  m[0][std::make_pair(3,3)] = std::vector<double> {0.0,0.0,1.557,11.37,0.6042,4.456};
		  m[0][std::make_pair(3,4)] = std::vector<double> {0.0,0.0,0.9531,11.64,0.6303,4.378};
		  m[0][std::make_pair(3,5)] = std::vector<double> {0.0,0.0,2.481,11.12,0.9857,5.059};
		  m[0][std::make_pair(3,6)] = std::vector<double> {0.0,0.0,2.889,11.6,0.9576,4.846};
		  m[0][std::make_pair(3,7)] = std::vector<double> {0.0,0.0,0.9654,11.7,0.6441,4.266};
		  m[0][std::make_pair(3,8)] = std::vector<double> {0.0,0.0,2.077,11.16,0.805,4.314};
		  m[0][std::make_pair(3,9)] = std::vector<double> {0.0,0.0,1.228,11.38,0.7877,5.022};
		  m[0][std::make_pair(3,10)] = std::vector<double>{0.0,0.0,1.933,11.41,1.387,4.281};
		  m[0][std::make_pair(3,11)] = std::vector<double>{0.0,0.0,2.013,12.17,1.365,3.863};
          
		  m[1][std::make_pair(0,0)] = std::vector<double> {0.0,0.0,1.392,12.19,0.4392,5.645};
		  m[1][std::make_pair(0,1)] = std::vector<double> {0.0,0.0,2.423,11.74,1.435,5.555};
		  m[1][std::make_pair(0,2)] = std::vector<double> {0.0,0.0,1.751,11.89,1.006,5.747};
		  m[1][std::make_pair(0,3)] = std::vector<double> {0.0,0.0,1.14,12.19,0.6755,6.112};
		  m[1][std::make_pair(0,4)] = std::vector<double> {0.0,0.0,1.057,12.43,0.6998,6.249};
		  m[1][std::make_pair(0,5)] = std::vector<double> {0.0,0.0,2.052,11.96,0.9994,5.843};
		  m[1][std::make_pair(0,6)] = std::vector<double> {0.0,0.0,2.763,11.54,1.02,5.777};
		  m[1][std::make_pair(0,7)] = std::vector<double> {0.0,0.0,1.262,12.56,0.7395,6.036};
		  m[1][std::make_pair(0,8)] = std::vector<double> {0.0,0.0,0.9912,12.43,0.5865,6.053};
		  m[1][std::make_pair(0,9)] = std::vector<double> {0.0,0.0,1.115,11.83,0.7117,5.765};
		  m[1][std::make_pair(0,10)] = std::vector<double>{0.0,0.0,1.354,12.06,0.9533,6.269};
		  m[1][std::make_pair(0,11)] = std::vector<double>{0.0,0.0,1.644,12.38,0.6117,6.051};
		  m[1][std::make_pair(1,0)] = std::vector<double> {0.0,0.0,2.511,11.97,1.671,5.887};
		  m[1][std::make_pair(1,1)] = std::vector<double> {0.0,0.0,1.196,12.28,0.8028,6.532};
		  m[1][std::make_pair(1,2)] = std::vector<double> {0.0,0.0,0.9922,12.59,0.5932,6.385};
		  m[1][std::make_pair(1,3)] = std::vector<double> {0.0,0.0,1.399,11.81,0.6517,5.725};
		  m[1][std::make_pair(1,4)] = std::vector<double> {0.0,0.0,1.082,12.1,0.5651,5.861};
		  m[1][std::make_pair(1,5)] = std::vector<double> {0.0,0.0,1.698,13.32,0.593,6.537};
		  m[1][std::make_pair(1,6)] = std::vector<double> {0.0,0.0,1.572,13.49,0.5928,6.291};
		  m[1][std::make_pair(1,7)] = std::vector<double> {0.0,0.0,1.267,12.33,0.7168,5.698};
		  m[1][std::make_pair(1,8)] = std::vector<double> {0.0,0.0,1.375,11.89,0.6036,5.688};
		  m[1][std::make_pair(1,9)] = std::vector<double> {0.0,0.0,1.064,12.61,0.5809,6.294};
		  m[1][std::make_pair(1,10)] = std::vector<double>{0.0,0.0,1.02,11.74,0.6375,5.83};
		  m[1][std::make_pair(1,11)] = std::vector<double>{0.0,0.0,1.465,11.5,0.9603,5.735};
		  m[1][std::make_pair(2,0)] = std::vector<double> {0.0,0.0,2.203,12.73,1.584,3.654};
		  m[1][std::make_pair(2,1)] = std::vector<double> {0.0,0.0,1.028,12.75,0.7025,4.12};
		  m[1][std::make_pair(2,2)] = std::vector<double> {0.0,0.0,1.686,12.08,0.6042,4.085};
		  m[1][std::make_pair(2,3)] = std::vector<double> {0.0,0.0,1.636,11.42,0.878,4.235};
		  m[1][std::make_pair(2,4)] = std::vector<double> {0.0,0.0,1.556,11.9,0.7281,4.336};
		  m[1][std::make_pair(2,5)] = std::vector<double> {0.0,0.0,1.825,12.91,0.8837,4.246};
		  m[1][std::make_pair(2,6)] = std::vector<double> {0.0,0.0,1.528,13.29,0.7223,4.293};
		  m[1][std::make_pair(2,7)] = std::vector<double> {0.0,0.0,1.426,12,0.7137,4.331};
		  m[1][std::make_pair(2,8)] = std::vector<double> {0.0,0.0,1.38,11.77,0.6664,4.22};
		  m[1][std::make_pair(2,9)] = std::vector<double> {0.0,0.0,2.399,11.49,1.2,3.85};
		  m[1][std::make_pair(2,10)] = std::vector<double>{0.0,0.0,1.068,11.95,0.7024,4.264};
		  m[1][std::make_pair(2,11)] = std::vector<double>{0.0,0.0,1.517,11.81,1.075,4.122};
		  m[1][std::make_pair(3,0)] = std::vector<double> {0.0,0.0,0.0,0.0,0.0,0.0};
		  m[1][std::make_pair(3,1)] = std::vector<double> {0.0,0.0,1.355,12.92,1.07,4.287};
		  m[1][std::make_pair(3,2)] = std::vector<double> {0.0,0.0,1.294,12.3,0.7493,4.428};
		  m[1][std::make_pair(3,3)] = std::vector<double> {0.0,0.0,1.037,12.21,0.6056,4.223};
		  m[1][std::make_pair(3,4)] = std::vector<double> {0.0,0.0,1.376,12.38,0.9060,4.241};
		  m[1][std::make_pair(3,5)] = std::vector<double> {0.0,0.0,1.742,12.81,0.8493,4.551};
		  m[1][std::make_pair(3,6)] = std::vector<double> {0.0,0.0,1.131,13.61,0.65,4.537};
		  m[1][std::make_pair(3,7)] = std::vector<double> {0.0,0.0,2.029,11.88,1.15,4.054};
		  m[1][std::make_pair(3,8)] = std::vector<double> {0.0,0.0,1.237,12.42,0.844,4.055};
		  m[1][std::make_pair(3,9)] = std::vector<double> {0.0,0.0,1.163,12.63,0.7625,4.278};
		  m[1][std::make_pair(3,10)] = std::vector<double>{0.0,0.0,1.157,12.66,0.8589,3.944};
		  m[1][std::make_pair(3,11)] = std::vector<double>{0.0,0.0,1.571,12.81,1.208,3.787};
          return m;
        }

};

const std::vector<int> Ch_position {0,2,4,6,8,10,11,9,7,5,3,1};

struct  RecHit_info{
		unsigned int plane;
		unsigned int channel;		
		bool operator < (const RecHit_info& y) const {
			return std::tie(this->plane, this->channel) < std::tie(y.plane, y.channel);
}
		bool operator == (const RecHit_info& y) const {
			return std::tie(this->plane, this->channel) == std::tie(y.plane, y.channel);
}
        };


	/*	
		bool operator == (const RecHit_info& x, const RecHit_info& y) {
			return std::tie(x.plane, x.channel) == std::tie(y.plane, y.channel);
}	*/	

		
class DiamondDetectorClass  {
	

	struct CalibrationKey
	{
		int sector, plane, channel;
	
		bool operator < (const CalibrationKey &rhs) const {
			return std::tie(this->sector, this->plane, this->channel) < std::tie(rhs.sector, rhs.plane, rhs.channel);
		}
	
		CalibrationKey(const int sector=-1, const int plane=-1, const int channel=-1)
		: sector(sector), plane(plane), channel(channel)
		{};
	};	
	

	typedef std::map< std::pair<int,int> , std::vector<CTPPSDiamondRecHit> >  RecHit_map;
	typedef std::map< CTPPSDiamondLocalTrack , std::vector<std::pair<RecHit_info,CTPPSDiamondRecHit> > >  LocalTrack_map;
	

	
	
   public:
      explicit DiamondDetectorClass(const edm::ParameterSet&, edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondRecHit> >, edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondLocalTrack> > );
      ~ DiamondDetectorClass();
	  
	  void ExtractData(const edm::Event&);
	  
	  void ExtractCalibrations(const boost::property_tree::ptree);
	  
	  inline int GetMux(int sector, int plane) 			{return Mux_map_[std::make_pair(sector,plane)];}
	  inline int GetMuxValidT(int sector, int plane) 	{return Mux_validT_map_[std::make_pair(sector,plane)];}
	  inline int GetMuxInTrack(int sector, int plane) 	{return Mux_inTrack_map_[std::make_pair(sector,plane)];}
	  inline bool SecIsSaturated(int sector) 			{return saturationV_[sector]!=0;}
	  
	  inline bool PairActive(int sector, int planeA, int channelA, int planeB, int channelB)   			
		{ return (RecHit_mapV_[sector].find( std::make_pair(planeA,channelA)) != RecHit_mapV_[sector].end() && RecHit_mapV_[sector].find( std::make_pair(planeB,channelB)) != RecHit_mapV_[sector].end());}
		
	  inline bool PadActive(int sector, int plane, int channel)   			
		{ return (RecHit_mapV_[sector].find( std::make_pair(plane,channel)) != RecHit_mapV_[sector].end());}
		
	  inline double GetTime(int sector, int plane, int channel)
		{return RecHit_mapV_[sector][std::make_pair(plane, channel)].at(0).getT();}	
		
	  inline double GetToT(int sector, int plane, int channel)
		{return RecHit_mapV_[sector][std::make_pair(plane, channel)].at(0).getToT();}	
		
	  inline std::vector<CTPPSDiamondRecHit>  GetPad(int sector, int plane, int channel) {return RecHit_mapV_[sector][std::make_pair(plane, channel)];};
		
	  double GetTime_SPC(int, int , int );
	  
	  int GetSpread(int, int);
	  int GetTrackSpread(int, int);
	  
	  inline LocalTrack_map GetDiamondTrack_map (int sector){return LocalTrack_mapV_[sector];}	  
	  inline std::vector<LocalTrack_map> GetDiamondTrack_mapV() {return LocalTrack_mapV_;}
	  inline int  GetTrackMux (int sector){return LocalTrack_mapV_[sector].size();}


   private:
   
   	enum Sector_id_
	{
		SECTOR_45_ID,
		SECTOR_56_ID
    };
	  
	enum Plane_id_
	{
		PLANE_0_ID,
		PLANE_1_ID,
		PLANE_2_ID,
		PLANE_3_ID
		
    };
	
    static const int CHANNELS_X_PLANE=12;
    static const int PLANES_X_DETECTOR=4;
    static const int MAX_SECTOR_NUMBER=2;
	
    std::vector<std::map<std::pair<int,int>,std::vector<double> > > SPC_mapV_;

	// ---------- objects to retrieve ---------------------------
	edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondRecHit> > tokenRecHit_;
	edm::EDGetTokenT< edm::DetSetVector<CTPPSDiamondLocalTrack> > tokenLocalTrack_;

	// ---------- data extracted---------------------------
	
	std::vector<RecHit_map> RecHit_mapV_;
	std::vector<LocalTrack_map> LocalTrack_mapV_;
	
	// ---------- mux map ---------------------------
	
	std::map< std::pair< int , int >, int> Mux_map_;   //arm, plane
	std::map< std::pair< int , int >, int> Mux_validT_map_;  //arm, plane
	std::map< std::pair< int , int >, int> Mux_inTrack_map_;  //arm, plane
	
	//  event status flags
	  
	std::vector<int> saturationV_;
	
	TF1 *CalibFunc;
	edm::FileInPath Calib_input_path_;
	
};





#endif
