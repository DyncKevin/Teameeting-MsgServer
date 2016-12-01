#ifndef __SERVER_CONFIG_H__
#define __SERVER_CONFIG_H__

#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"
#include "rapidjson/reader.h"

namespace gim{

class ServerConfig{
public:
	std::string IP;
	std::string Type;
	std::string ZkUrl;
	std::string NodePath;
	std::string ModulePath;
	std::string ProjectPath;

	ServerConfig();
    ~ServerConfig();

	int init(const std::string& f);

	int parseFromJson(const rapidjson::Document& d);

	std::string toStyledString() const;

private:

};

}

#endif
