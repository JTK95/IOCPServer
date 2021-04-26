#pragma once
#pragma once


/****************************************************************************
함수명	: loadConfig
설명		: config.xml파일을 파싱한다
리턴값	: bool
매개변수	: xml_t*
*****************************************************************************/
extern bool loadConfig(IN OUT TiXmlDocument* config);
