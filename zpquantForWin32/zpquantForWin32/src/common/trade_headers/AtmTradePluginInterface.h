#ifndef _COMPRETRADESYSTEMHEADERS_ATMTRADEPLUGININTERFACE_H_
#define _COMPRETRADESYSTEMHEADERS_ATMTRADEPLUGININTERFACE_H_

#ifndef BOOST_SPIRIT_THREADSAFE
#define BOOST_SPIRIT_THREADSAFE
#endif
#include <boost/property_tree/ptree.hpp>
#include "TradePluginContextInterface.h"
#include <unordered_map>
#include "AtmPluginInterface.h"

using namespace std;
using namespace boost::property_tree;

class MAtmTradePluginInterface:
	public MAtmPluginInterface
{
public:
	virtual void TDInit(const ptree &, MTradePluginContextInterface*, unsigned int AccountNumber) = 0;
};
#endif