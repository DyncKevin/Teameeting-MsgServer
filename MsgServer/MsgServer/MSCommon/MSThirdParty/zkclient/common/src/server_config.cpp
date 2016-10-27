#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "server_config.h"
#include "base/ef_log.h"

namespace gim{


using namespace std;
using namespace ef;

ServerConfig::ServerConfig()
    :IP("")
     ,Type("")
     ,ZkUrl("")
     ,NodePath("")
     ,ModulePath("")
     ,ProjectPath("")
     {
     }

ServerConfig::~ServerConfig(){
}

int ServerConfig::init(const std::string& f){

	int ret = -1;
    printf("ServerConfig::init file name:%s\n", f.c_str());
    FILE* pf = fopen(f.c_str(), "r");
    if (pf) {
        rapidjson::FileStream fs(pf);

        rapidjson::Document		jsonReqDoc;
        if (jsonReqDoc.ParseStream<0>(fs).HasParseError()) {
            fprintf(stderr, "jsonReqDoc.ParseStream error");
            fclose(pf);
            return -2;
        }
        ret = parseFromJson(jsonReqDoc);
        fclose(pf);
    }
	return ret;
}

int ServerConfig::parseFromJson(const rapidjson::Document& root){

	int ret = 0;

    if(!(root.HasMember("IP") && root["IP"].IsString()))
    {
        return -1;
    }
    IP = root["IP"].GetString();

    if(!(root.HasMember("Type") && root["Type"].IsString()))
    {
        return -1;
    }
    Type = root["Type"].GetString();

    if(!(root.HasMember("ZkUrl") && root["ZkUrl"].IsString()))
    {
        return -1;
    }
    ZkUrl = root["ZkUrl"].GetString();

    if(!(root.HasMember("NodePath") && root["NodePath"].IsString()))
    {
        return -1;
    }
    NodePath = root["NodePath"].GetString();

    if(!(root.HasMember("ModulePath") && root["ModulePath"].IsString()))
    {
        return -1;
    }
    ModulePath = root["ModulePath"].GetString();

    if(!(root.HasMember("ProjectPath") && root["ProjectPath"].IsString()))
    {
        return -1;
    }
    ProjectPath = root["ProjectPath"].GetString();

    printf("Ip:%s, type:%s, zkurl:%s, nodepath:%s, module:%s, project:%s\n", IP.c_str(), Type.c_str(), ZkUrl.c_str(), NodePath.c_str()
            , ModulePath.c_str(), ProjectPath.c_str());

	return 0;
}

string ServerConfig::toStyledString() const{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("IP", IP.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("Type", Type.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("ZkUrl", ZkUrl.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("NodePath", NodePath.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("ModulePath", ModulePath.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("ProjectPath", ProjectPath.c_str(), jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();

	return s;
}

}
