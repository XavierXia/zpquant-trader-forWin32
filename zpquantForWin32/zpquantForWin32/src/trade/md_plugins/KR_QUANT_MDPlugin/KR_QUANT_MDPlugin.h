#ifndef _QFCOMPRETRADESYSTEM_ATMMARKETDATAPLUGINS_KR_QUANT_MDPLUGIN_H_
#define _QFCOMPRETRADESYSTEM_ATMMARKETDATAPLUGINS_KR_QUANT_MDPLUGIN_H_
#include <string>
#include <boost/thread.hpp>
#include <thread>                // std::thread
#include <mutex>                // std::mutex, std::unique_lock
#include <atomic>
#include <boost/asio.hpp>
#include <memory>
#include <tuple>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <future>

extern "C" {
#include "mds_api/mds_api.h"
#include "mds_api/parser/mds_protocol_parser.h"
#include "mds_api/parser/json_parser/mds_json_parser.h"
#include "sutil/logger/spk_log.h"
#include "sutil/time/spk_times.h"
}

#include "SeverityLevel.h"
#include <vector>

#include "AtmPluginInterface.h"
#include "AtmMarketDataPluginInterface.h"
#include "nn.hpp"
#include <nanomsg/pair.h>

using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;

//std::function<int32(MdsApiSessionInfoT *,SMsgHeadT *,void *,void *)> MarketDataCallBack; 

class CKrQuantMDPluginImp:
	public MAtmMarketDataPluginInterface
{

	io_service  m_IOservice;
	deadline_timer m_StartAndStopCtrlTimer;
	std::future<bool> m_futTimerThreadFuture;

	string m_strUsername;
	string m_strPassword;

//账号线程关键属性
	unsigned int m_uRequestID = 0;
	bool m_boolIsOnline = false;

public:
	static const string s_strAccountKeyword;
	CKrQuantMDPluginImp();
	~CKrQuantMDPluginImp();

	nn::socket nnsocket;
	map<string, ClientListMdInfoT> mclient;

	virtual bool IsOnline();
	virtual void CheckSymbolValidity(const unordered_map<string, string> &);
	virtual string GetCurrentKeyword();
	virtual string GetProspectiveKeyword(const ptree &);
	virtual void GetState(ptree & out);
	virtual void MDInit(const ptree &, ptree &, string &);
	virtual void MDAddClientUser(const ptree &, ptree &, string &);
public:
	void MDDestoryAll();
	BOOL MDResubscribeByCodePrefix(MdsApiSessionInfoT *pTcpChannel,
        const char *pCodeListString, int32 MdsSubscribeDataType,eMdsSubscribeModeT subMode);
	BOOL MDResubscribeByCodePostfix(MdsApiSessionInfoT *pTcpChannel,
		const char *pCodeListString, eMdsSubscribeModeT subMode);
	void OnWaitOnMsg();

private:
	bool Start();
	void Stop();
	void OnError();
	void TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err);

private:
    /* 内部的回调处理函数 */
    static void *       MdThreadMain(void *pParams);
};
#endif

