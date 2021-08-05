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
			// C CODE
			fp = fopen(m_azFullFile, "r");
			if (fp != NULL)
			{
				while (fgets(textCSVLineData, sizeof(textCSVLineData), fp) != NULL)
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
				}
				fclose(fp);
			}
		}
		auto end = chrono::steady_clock::now();
		auto gap = chrono::duration_cast<chrono::seconds>(end - start).count();
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
	vector<string> rvHM     = Split(auAmpolTime, ':');
	vector<string> rvSecMil = Split(rvHM[2], ' ');

	double rv = atof(rvHM[0].c_str())*3600.0       +
                atof(rvHM[1].c_str())*60.0         +               
		        atof(rvSecMil[0].c_str())          +
                atof(rvSecMil[1].c_str()) / 1000.0 +
                atof(rvHM[3].c_str()) / 1000000.0;

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
void CFlightDataCollection::Finalize()
{
	EraseFlightData();
}
