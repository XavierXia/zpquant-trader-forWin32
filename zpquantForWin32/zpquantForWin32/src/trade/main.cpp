#include <string>
#include <sys/stat.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "public.h"

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;

#include "trade_service.h"
#include "CommuModForServInterface.h"
using namespace std;
// g++ -D BOOST_SPIRIT_THREADSAFE

char ProcessName[256];

void InitLog(std::string configFile)
{
    ptree g_Config;
    boost::property_tree::read_json(configFile, g_Config);
    

    auto LogConfig = g_Config.find("logconfig");
    if (LogConfig != g_Config.not_found())
    {
        auto LogtypeNode = LogConfig->second.find("logtype");
        if(LogtypeNode== LogConfig->second.not_found())
            throw std::runtime_error("[error]Can not find 'LogConfig.Logtype' Value.");
        string LogTypeString = LogtypeNode->second.data();
        if (LogTypeString.find("syslog")!= std::string::npos)
        {
            string _ServerAddress="";
            unsigned short _Port = 0;
            if (LogConfig->second.find("syslog") != LogConfig->second.not_found())
            {
                auto Syslog = LogConfig->second.find("syslog");
                if (Syslog->second.find("serveraddress") != Syslog->second.not_found())
                    _ServerAddress = Syslog->second.find("serveraddress")->second.data();
                else
                    throw std::runtime_error("[error]invalid 'logconfig.syslog.serveraddress' value.");
                if (Syslog->second.find("port") != Syslog->second.not_found())
                    _Port = atoi(Syslog->second.find("port")->second.data().c_str());
                else
                    throw std::runtime_error("[error]invalid 'logconfig.syslog.port' value.");
            }
            else
                throw std::runtime_error("[error]could not find 'logconfig.syslog' node.");
            
        }

        if (LogTypeString.find("file") != std::string::npos)
        {
        }

        if (LogTypeString.find("console") != std::string::npos)
        {
        }
    }
    else
        throw std::runtime_error("[error]could not find 'logconfig' node.");

}

// ./thunder-trader thunder-trader.conf.default 1
int main(int argc,char *argv[])
{
    if (argc < 3)
    {
        cout<<"Usage:ATM.exe %ConfigFileName%.json sysNumber [daemon]"<<endl;
        return 0;
    }
    try
    {
        strncpy(ProcessName, argv[0], sizeof(ProcessName));
        if(atoi(argv[2])>_MaxAccountNumber)
            throw std::runtime_error("sysNumber error");
        InitLog(argv[1]);
        
        CTradeService service(argv[1],atoi(argv[2]));
        service.Start();
        service.Join();
    }
    catch (std::exception & exp)
    {
        std::cout << exp.what() << endl;
        return 1;
    }
    return 0;
}

