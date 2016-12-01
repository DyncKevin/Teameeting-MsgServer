#include "ZKClt.h"
#include "RTZKClient.hpp"
#include "RTProcessInfo.h"

/////////////////////////////////////////////////////////////
///////////////////////file global///////////////////////////
/////////////////////////////////////////////////////////////

bool CheckNodeExist(const std::string& nodePath)
{
    return true;
}

void UpdateNodeData(const std::string& nodePath, const std::string& nodeData)
{

}


void UpdateProcessStatus(std::string& status)
{
    RTProcessInfo info;
    pid_t pid = getpid();
    float cpu = info.GetPcpu(pid);
    float mem = info.GetPmem(pid);
    int online = 1;
    std::string node = RTZKClient::Instance().GetServerConfig().NodePath;

    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    rapidjson::Document jDocNode;
    rapidjson::StringBuffer sbNode;
    rapidjson::Writer<rapidjson::StringBuffer> writerNode(sbNode);
    jDocNode.SetObject();
    jDocNode.AddMember("node", node.c_str(), jDocNode.GetAllocator());
    jDocNode.Accept(writerNode);

    rapidjson::Document jDocSyst;
    rapidjson::StringBuffer sbSyst;
    rapidjson::Writer<rapidjson::StringBuffer> writerSyst(sbSyst);
    jDocSyst.SetObject();
    jDocSyst.AddMember("cpu", cpu, jDocSyst.GetAllocator());
    jDocSyst.AddMember("mem", mem, jDocSyst.GetAllocator());
    jDocSyst.Accept(writerSyst);

    rapidjson::Document jDocProc;
    rapidjson::StringBuffer sbProc;
    rapidjson::Writer<rapidjson::StringBuffer> writerProc(sbProc);
    jDocProc.SetObject();
    jDocProc.AddMember("online", online, jDocProc.GetAllocator());
    jDocProc.Accept(writerProc);

    jDoc.SetObject();
    jDoc.AddMember("node", sbNode.GetString(), jDoc.GetAllocator());
    jDoc.AddMember("syst", sbSyst.GetString(), jDoc.GetAllocator());
    jDoc.AddMember("proc", sbProc.GetString(), jDoc.GetAllocator());

    jDoc.Accept(writer);
    status = sb.GetString();
}

ZKClt::ZKClt()
{

}

ZKClt::~ZKClt()
{

}

bool ZKClt::Init(const std::string& config)
{
    mConfPath = config;
    if (mConfPath.c_str()==nullptr || mConfPath.length()==0)
    {
        return false;
    }
    if (RTZKClient::Instance().InitZKClient(mConfPath)!=0)
    {
        return false;
    } else {
        mNodePath = RTZKClient::Instance().GetServerConfig().NodePath;
        if (mNodePath.c_str()==nullptr || mNodePath.length()==0)
        {
            return false;
        } else {
            return true;
        }
    }
    return true;
}

bool ZKClt::Unin()
{
    RTZKClient::Instance().Unin();
    return true;
}

void ZKClt::SetCallback(NodeCallback addCb, NodeCallback delCb)
{
    RTZKClient::Instance().SetNodeCallback(addCb, delCb);
}

bool ZKClt::DoUpload()
{
    std::string st("");
    UpdateProcessStatus(st);
    if (RTZKClient::Instance().CheckNodeExists(mNodePath))
    {
        RTZKClient::Instance().SetNodeData(mNodePath, st);
        return true;
    } else {
        return false;
    }
    return true;
}


