#ifndef _COMPRETRADESYSTEMHEADERS_ATMMARKETDATAPLUGININTERFACE_H_
#define _COMPRETRADESYSTEMHEADERS_ATMMARKETDATAPLUGININTERFACE_H_

#include <unordered_map>
#include <atomic>
#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>  
#include <sstream>
#include <string>
#include <boost/thread.hpp>
#include <unordered_map>
#include "AtmPluginInterface.h"

using namespace std;
using namespace boost::property_tree;

class MAtmMarketDataPluginInterface:
	public MAtmPluginInterface
{
public:
	virtual void MDInit(const ptree &) = 0;
};
#endif