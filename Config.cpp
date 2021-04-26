#include "pch.h"

#include "Config.h"

bool loadConfig(IN OUT TiXmlDocument* config)
{
	if (!config->LoadFile("./config.xml"))
	{
		printf_s("! not exist config file.");
		return false;
	}
	return true;
}