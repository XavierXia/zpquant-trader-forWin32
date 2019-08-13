#ifndef _COMPRETRADESYSTEMHEADERS_MATMPLUGINMANAGEINTERFACE_H_
#define _COMPRETRADESYSTEMHEADERS_MATMPLUGINMANAGEINTERFACE_H_
#include <string>
#include <unordered_map>

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;


using namespace std;
class MAtmPluginInterface
{
public:
	virtual bool IsOnline() = 0;
	virtual void CheckSymbolValidity(const unordered_map<string, string> &) = 0;
	virtual string GetCurrentKeyword() = 0;
	virtual string GetProspectiveKeyword(const ptree &) = 0;
};
#endif
