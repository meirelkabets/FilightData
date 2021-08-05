#include "stdafx.h"
#include "FlightDataCollection.h"
#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;
///////////////////////////////////////////////////////////////////////////////
CFlightDataCollection::CFlightDataCollection(void)
{
	m_initialized = false;
	m_cppParser   = false;
}

///////////////////////////////////////////////////////////////////////////////
CFlightDataCollection::~CFlightDataCollection(void)
{
	Finalize();
}

///////////////////////////////////////////////////////////////////////////////
bool CFlightDataCollection::Initialize(const char * reportFile)
{
	if (_fullpath(m_azFullFile, reportFile, MAX_FILE_NAME) == NULL)
		return false;

	m_initialized = true;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool CFlightDataCollection::ReadFile()
{
	ifstream       reportFile;
	char           auTimeTag[32] = { 0 };
	vector<string> rvFields;
	T_FLIGHT_DATA* pstFlightData = NULL;
	vector<string> rvSplitted;

	FILE* fp = NULL;
	char   textCSVLineData[1024] = { 0 }; //for C   parsing file code.   
	string textLineCSV;                   //for C++ parsing file code. 

	EraseFlightData();

	if (m_initialized)
	{	
		auto start = chrono::steady_clock::now();
				
		if (m_cppParser)
		{   
			// C++11/14/17  CODE
			reportFile.open(m_azFullFile, ios::in);
			if (reportFile.is_open())
			{
				while (getline(reportFile, textLineCSV))
				{
					if (textLineCSV.empty())
						continue;

					rvSplitted = Split(textLineCSV, ',');
					if (rvSplitted.size() >= 7)
					{
						pstFlightData = new T_FLIGHT_DATA;
						memset(pstFlightData, 0, sizeof(T_FLIGHT_DATA));
						CollectData(pstFlightData, rvSplitted);
					}
				}
				reportFile.close();
			}
		}
		else
		{
			// C CODE faster ...
			fp = fopen(m_azFullFile, "r");
			if (fp != NULL)
			{
				char TimeTag[32] = { 0 };

				while (!feof(fp))
				{
					pstFlightData = new T_FLIGHT_DATA;
					int nItems = fscanf(fp, "%[^,],%hu,%lf,%lf,%hu,%hu,%lf", TimeTag, &pstFlightData->FlightNumber, &pstFlightData->Heading, &pstFlightData->Alt, &pstFlightData->AutoPilotEngage, &pstFlightData->MarkerBeacon, &pstFlightData->CabinLightDimmer);
					if (nItems >= 7)
					{
						pstFlightData->TimeTag = GetTimeFromMidNight(TimeTag);
						CollectData(pstFlightData);
					}
				}

				/*while (fgets(textCSVLineData, sizeof(textCSVLineData), fp) != NULL)
				{
					if (strlen(textCSVLineData) == 0 || (*textCSVLineData) == '\n')
						continue;
					
					rvSplitted = Split(textCSVLineData, ',');					
					if (rvSplitted.size() >= 7)
					{
						pstFlightData = new T_FLIGHT_DATA;
						memset(pstFlightData, 0, sizeof(T_FLIGHT_DATA));
						CollectData(pstFlightData, rvSplitted);
					}					
				}*/
				fclose(fp);
			}
		}
		auto end = chrono::steady_clock::now();
		auto gap = chrono::duration_cast<chrono::milliseconds>(end - start).count();

		int y = 0;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
vector<string> CFlightDataCollection::Split(std::string delimitedString, const char delimiter)
{
	vector<string> rv;
	string itemValue;
	istringstream stream(delimitedString);

	while (getline(stream, itemValue, delimiter))
	{
		rv.push_back(itemValue);
	}
	return rv;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double CFlightDataCollection::GetTimeFromMidNight(const char * auAmpolTime)
{
	double         rv(0);
	vector<string> rvHM;
	vector<string> rvSecMil;

	rvHM = Split(auAmpolTime, ':');

	if (rvHM.size() >= 4)
	{
		rvSecMil = Split(rvHM[2], ' ');

		if (rvSecMil.size() >= 2)
		{
			rv = atof(rvHM[0].c_str())*3600.0 +
				 atof(rvHM[1].c_str())*60.0 +
				 atof(rvSecMil[0].c_str()) +
				 atof(rvSecMil[1].c_str()) / 1000.0 +
				 atof(rvHM[3].c_str()) / 1000000.0;
		}
	}

	return rv;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int64_t CFlightDataCollection::GetTotalLines()
{
	int64_t lines_count = 0;
	ifstream reportFile;
	reportFile.open(m_azFullFile, ios::in);

	if (reportFile.is_open())
	{
		lines_count = std::count(std::istreambuf_iterator<char>(reportFile),
		std::istreambuf_iterator<char>(), '\n');
		reportFile.close();
	}	
	return lines_count;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CFlightDataCollection::CollectData(T_FLIGHT_DATA *& pstFlightData, 
	                                    const std::vector<std::string>& rvSplitted)
{
	pstFlightData->TimeTag          = GetTimeFromMidNight(rvSplitted[0].c_str());
	pstFlightData->FlightNumber     = (unsigned short)atoi(rvSplitted[1].c_str());
	pstFlightData->Heading          = atof(rvSplitted[2].c_str());
	pstFlightData->Alt              = atof(rvSplitted[3].c_str());
	pstFlightData->AutoPilotEngage  = (unsigned short)atoi(rvSplitted[4].c_str());
	pstFlightData->MarkerBeacon     = (unsigned short)atoi(rvSplitted[5].c_str());
	pstFlightData->CabinLightDimmer = atof(rvSplitted[6].c_str());

	m_vctFilghtData.push_back(pstFlightData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CFlightDataCollection::Finalize()
{
	EraseFlightData();
}
