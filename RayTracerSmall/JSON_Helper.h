#pragma once
#include<string>
#include<vector>
#include<fstream>
#include<rapidjson.h>
#include <writer.h>
#include <document.h>
#include <istreamwrapper.h>
#include <ostreamwrapper.h>


//Json pars from:https://github.com/Tencent/rapidjson/ 

using namespace std;
using namespace rapidjson;
//position, radius, surface color, reflectivity, transparency, emission color and change vars
struct SphereData
{
	float Position[3];
	float PositionMove[3];
	float surface_Color[3];
	float emission_Color[3];
	float radius;
	float reflectivity;
	float transparency;
	float radiusChange;
};

// Check node is there
template <typename DataFormat>
bool CheckDataIsThere(const std::string& objectName, const DataFormat& document)
{
	return document.HasMember(objectName.c_str());;
}

/// <summary>
/// Load shphere data form json file
/// </summary>
class JSON_Helper
{
public:
	static vector<SphereData> LoadJsonFile(string file);


private:
	static Document ParseFile(const std::string& file);
};

