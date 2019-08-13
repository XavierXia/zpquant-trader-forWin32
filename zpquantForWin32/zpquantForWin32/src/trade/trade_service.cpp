//#include "stdafx.h"
#include <fstream>
#include <string>
#include <iostream>
#include <memory>
#include <exception>
#include <sstream>
#include <algorithm>
#include "public.h"

#ifndef WIN32
#include <unistd.h>
#endif

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 

using namespace boost::property_tree;
using namespace std;

#include "trade_service.h"

string GetNodeData(string name, const ptree & root)
{
    auto Node = root.find(name);
    if (Node == root.not_found())
    {
        char buf[128];
        snprintf(buf, sizeof(buf), "can not find <%s>", name.c_str());
        throw std::runtime_error(buf);
    }
    else
        return Node->second.data();
}

extern char ProcessName[256];

CTradeService::CTradeService(std::string configFile, unsigned int sysnum):m_strConfigFile(configFile)
{
    m_uSystemNumber = sysnum;
    m_vecAllTradeSource.first.resize(_MaxAccountNumber + 1);
}

CTradeService::~CTradeService()
{
    if (m_pApi)
    {
        m_pApi->Release();
        m_pApi = nullptr;
    }
}

void CTradeService::MakeError(ptree & out, const char * fmt, ...)
{
	out.put("type", "error");
	char buf[STRATEGY_MESSAGE_MAXLEN];
	va_list arg;
	va_start(arg, fmt);
	auto n = vsnprintf(buf, STRATEGY_MESSAGE_MAXLEN, fmt, arg);
	va_end(arg);
	if (n > -1 && n < STRATEGY_MESSAGE_MAXLEN)
		out.put("errormsg", buf);
	else
		out.put("errormsg", "!<buffer is too short to put this message>");
}

void CTradeService::Start()
{
    auto temp=MCommuModForServInterface::CreateApi(GetAddress().c_str(),GetListenPort(), this, GetNetHandlerThreadCount());
    m_pApi = temp;
    if (m_pApi)
        m_pApi->StartListen();
}

void CTradeService::Join()
{
#ifdef WIN32
    Sleep(INFINITE);
#else
    sleep(0);
#endif
}

unsigned short CTradeService::GetListenPort()
{
    ptree g_Config;
    boost::property_tree::read_json(m_strConfigFile, g_Config);
    

    if (g_Config.find("basic") != g_Config.not_found())
    {
        auto Basic = g_Config.find("basic");
        if (Basic->second.find("listenport") != Basic->second.not_found())
        {
            unsigned short _ListenPort = atoi(Basic->second.find("listenport")->second.data().c_str());
            if (0 == _ListenPort)
                throw std::runtime_error("[error]invalid 'basic.listenport' value.");
            else
                return _ListenPort;
        }
        else
            throw std::runtime_error("[error]could not find 'basic.listenport' node.");
    }
    else
        throw std::runtime_error("[error]could not find 'basic' node.");

}

size_t CTradeService::GetNetHandlerThreadCount()
{
    ptree g_Config;
    boost::property_tree::read_json(m_strConfigFile, g_Config);


    if (g_Config.find("basic") != g_Config.not_found())
    {
        auto Basic = g_Config.find("basic");
        if (Basic->second.find("nethandlerthreadcount") != Basic->second.not_found())
        {
            unsigned short _NetHandlerThreadCount = atoi(Basic->second.find("nethandlerthreadcount")->second.data().c_str());
            if (0 == _NetHandlerThreadCount)
                throw std::runtime_error("[error]invalid 'basic.nethandlerthreadcount' value.");
            else
                return _NetHandlerThreadCount;
        }
        else
            throw std::runtime_error("[error]could not find 'basic.nethandlerthreadcount' node.");
    }
    else
        throw std::runtime_error("[error]could not find 'basic' node.");

}

void CTradeService::OnCommunicate(const ptree & in, ptree & out)
{
    if (in.find("type") != in.not_found())
    {
        auto Type = in.find("type")->second.data();
        auto PackageHandler = m_mapString2PackageHandlerType.find(Type);
        if(PackageHandler == m_mapString2PackageHandlerType.end())
            MakeError(out, "invalid <type>");
        else
        {
            try {
                PackageHandler->second(in, out);    
            }
            catch (std::exception & err)
            {
                out.clear();
                MakeError(out, err.what());
            }
        }
    }
    else
        MakeError(out, "Can not find <type>.");
    
}

string CTradeService::GetAddress()
{
    ptree g_Config;
    boost::property_tree::read_json(m_strConfigFile, g_Config);


    if (g_Config.find("basic") != g_Config.not_found())
    {
        auto Basic = g_Config.find("basic");
        if (Basic->second.find("address") != Basic->second.not_found())
        {
            string _Address = Basic->second.find("address")->second.data().c_str();
            if (_Address.empty())
                throw std::runtime_error("[error]invalid 'basic.address' value.");
            else
                return _Address;
        }
        else
            throw std::runtime_error("[error]could not find 'basic.address' node.");
    }
    else
        throw std::runtime_error("[error]could not find 'basic' node.");

}

/*
./thunder-trader|0: ...ReqAddSource, ptree, in: {
    "type": "reqaddtradesource",
    "sourcetype": "ctp",
    "brokerid": "9999",
    "maxcancelperday": "",
    "password": "808502",
    "serveraddress": "tcp:\/\/192.168.0.2",
    "username": "123456"
}
*/
void CTradeService::ReqAddSource(PackageHandlerParamType param, const ptree & in, ptree &out)
{
	cout << "...ReqAddSource, ptree: " << endl;
	const unordered_map<string, pair<TPluginFactory, string> > * tarFactoryMap = nullptr;
	pair<vector<PluginPtrType>, boost::shared_mutex> * tarContainer = nullptr;
	if (PackageHandlerParamType::MarketData == param)
	{
		tarFactoryMap = &m_mapAMarketDataPFactories;
		tarContainer = &m_vecAllMarketDataSource;
	}
	else
	{
		tarFactoryMap = &m_mapATradePFactories;
		tarContainer = &m_vecAllTradeSource;
	}

	//添加打印信息
	std::stringstream in_print_str;
	boost::property_tree::write_json(in_print_str, in);
	cout << "...ReqAddSource, ptree, in: "<< in_print_str.str().c_str() << endl;

	if (in.find("sourcetype") != in.not_found())
	{
		auto SourceType = in.find("sourcetype")->second.data();
		auto SourceTypeItr = tarFactoryMap->find(SourceType);
		if (SourceTypeItr != tarFactoryMap->end())
		{
			auto ObjectPlugin = SourceTypeItr->second.first();
			boost::unique_lock<boost::shared_mutex> lock(tarContainer->second);
			auto FindResult = find_if(
				tarContainer->first.begin(),
				tarContainer->first.end(),
				[ObjectPlugin, in](PluginPtrType CurrentPlugin) {
				return CurrentPlugin
					&&
					(CurrentPlugin->GetCurrentKeyword() == ObjectPlugin->GetProspectiveKeyword(in)); }
			);
			if (FindResult != tarContainer->first.end())
			{
				if (PackageHandlerParamType::MarketData == param)
				{
				}
				else
				{
				}
			}
			else
			{
				if (PackageHandlerParamType::MarketData == param)
				{
					MAtmMarketDataPluginInterface * mdObjectPlugin
						= dynamic_cast<MAtmMarketDataPluginInterface*>(ObjectPlugin.get());
					mdObjectPlugin->MDInit(in);
					tarContainer->first.push_back(ObjectPlugin);
					vector<PluginPtrType>(tarContainer->first).swap(tarContainer->first);
					out.put("type", "rspaddmarketdatasource");
					out.put("result", "market data source init succeed.");
				}
				else
				{
					unsigned int NewAccountNumber = 0;
					for (; NewAccountNumber < tarContainer->first.size(); NewAccountNumber++)
					{
						if (tarContainer->first[NewAccountNumber] == nullptr)
							break;
					}
					if (NewAccountNumber >= tarContainer->first.size())
						throw std::runtime_error("too much account exists in this process,maxmun 32");
					MAtmTradePluginInterface * tdObjectPlugin
						= dynamic_cast<MAtmTradePluginInterface*>(ObjectPlugin.get());
					tdObjectPlugin->TDInit(in, this, NewAccountNumber);//
					tarContainer->first[NewAccountNumber] = ObjectPlugin;
					out.put("type", "rspaddtradesource");
					out.put("result", "trade source init succeed.");

				}

			}

		}
		else
		{
			string exp = "the sourcetype " + SourceType + " does not support";
			throw std::runtime_error(exp.c_str());
		}
	}
	else
		throw std::runtime_error("can not find <sourcetype>");
}


