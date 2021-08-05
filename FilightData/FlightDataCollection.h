#pragma once

#include <vector>

constexpr auto MAX_FILE_NAME = (512);

#pragma pack(push,1)
typedef struct T_FLIGHT_DATA
{
	double         TimeTag;
	unsigned short FlightNumber;
	double Heading;
	double Alt;
	unsigned short AutoPilotEngage;
	unsigned short MarkerBeacon;
	double         CabinLightDimmer;
}T_FILGHT_DATA;
#pragma pack(pop,1)

class CFlightDataCollection
{
public:
	CFlightDataCollection(void);
	~CFlightDataCollection(void);

public:
	bool Initialize(const char* reportFile);
	bool ReadFile();

private:
	std::vector<std::string> CFlightDataCollection::Split(std::string delimitedString, const char delimiter);
	double GetTimeFromMidNight(const char* auAmpolTime);
	int64_t GetTotalLines();

	inline void EraseFlightData()
	{
		for (size_t i = 0; m_vctFilghtData.size(); i++)
		{
			delete m_vctFilghtData[i];
		}
		m_vctFilghtData.clear();
	}

	inline void CollectData(T_FLIGHT_DATA*& pstFlightData, const std::vector<std::string>& rvSplitted)
	{		
		pstFlightData->TimeTag = GetTimeFromMidNight(rvSplitted[0].c_str());
		pstFlightData->FlightNumber = (unsigned short)atoi(rvSplitted[1].c_str());
		pstFlightData->Heading = atof(rvSplitted[2].c_str());
		pstFlightData->Alt = atof(rvSplitted[3].c_str());
		pstFlightData->AutoPilotEngage = (unsigned short)atoi(rvSplitted[4].c_str());
		pstFlightData->MarkerBeacon = (unsigned short)atoi(rvSplitted[5].c_str());
		pstFlightData->CabinLightDimmer = atof(rvSplitted[6].c_str());

		m_vctFilghtData.push_back(pstFlightData);		
	}

	void Finalize();

private:
	bool m_initialized;
	bool m_cppParser;

private:
	char m_azFullFile[MAX_FILE_NAME] = { 0 };
	std::vector<T_FLIGHT_DATA*> m_vctFilghtData;

};

