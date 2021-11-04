#include "JSON_Helper.h"

vector<SphereData> JSON_Helper::LoadJsonFile(string file)
{

	Document document;


	std::ifstream fileStream(file);
	if (!fileStream.is_open())
	{
		document.SetNull();
	}
	else
	{
		IStreamWrapper streamWrapper(fileStream);
		document.ParseStream(streamWrapper);
		if (document.HasParseError())
			document.SetNull();
		fileStream.close();
	}

	SphereData objectData;
	std::vector<SphereData> data;

	

	if (!document.IsNull())
	{
		
		// Load from file
		for (Value& Object : document["SphereObjects"].GetArray())
		{
			
			if (CheckDataIsThere<Value>("Position", Object))
			{
				int i = 0;
				for (Value& ob : Object["Position"].GetArray()) {
					objectData.Position[i] = ob.GetFloat();
					i++;
				}
			}
			if (CheckDataIsThere<Value>("Surface_Color", Object))
			{
				int i = 0;
				for (Value& ob : Object["Surface_Color"].GetArray()) {
					objectData.surface_Color[i] = ob.GetFloat();
					i++;
				}
			}

			if (CheckDataIsThere<Value>("emission_Color", Object))
			{
				int i = 0;
				for (Value& ob : Object["emission_Color"].GetArray()) {
					objectData.emission_Color[i] = ob.GetFloat();
					i++;
				}
			}
			if (CheckDataIsThere<Value>("Position_Move_Speed", Object))
			{
				int i = 0;
				for (Value& ob : Object["Position_Move_Speed"].GetArray()) {
					objectData.PositionMove[i] = ob.GetFloat();
					i++;
				}
			}
			if (CheckDataIsThere<Value>("Radius", Object))
			{
				objectData.radius = Object["Radius"].GetFloat();
			}
			if (CheckDataIsThere<Value>("Radius_Change", Object))
			{
				objectData.radiusChange = Object["Radius_Change"].GetFloat();
			}
			if (CheckDataIsThere<Value>("Reflectivity", Object))
			{
				objectData.reflectivity = Object["Reflectivity"].GetFloat();
			}
			if (CheckDataIsThere<Value>("Transparency", Object))
			{
				objectData.transparency = Object["Transparency"].GetFloat();
			}
		
			data.push_back(objectData);
		}
	}

	string ErroLog;
	// Check if data was loaded 
	if (data.size() == 0) {
		ErroLog += "No Data \n";
	}
		
	printf("\n\n Error: \n %s", ErroLog.c_str());


	return data;
}

Document JSON_Helper::ParseFile(const std::string& file)
{
	Document document;

	std::ifstream fileStream(file);
	if (!fileStream.is_open())
	{
		document.SetNull();
	}
	else
	{
		IStreamWrapper streamWrapper(fileStream);
		document.ParseStream(streamWrapper);
		if (document.HasParseError())
			document.SetNull();
		fileStream.close();
	}

	return document;
}