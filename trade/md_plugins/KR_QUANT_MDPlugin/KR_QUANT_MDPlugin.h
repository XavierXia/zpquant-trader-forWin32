#ifndef _QFCOMPRETRADESYSTEM_ATMMARKETDATAPLUGINS_KR_QUANT_MDPLUGIN_H_
#define _QFCOMPRETRADESYSTEM_ATMMARKETDATAPLUGINS_KR_QUANT_MDPLUGIN_H_
#include <string>
#include <boost/thread.hpp>
#include <thread>                // std::thread
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>    // std::condition_variable
#include <atomic>
#include <boost/asio.hpp>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <future>
#include <tuple>
#include <boost/log/common.hpp>

extern "C" {
#include "mds_api/mds_api.h"
#include "mds_api/parser/mds_protocol_parser.h"
#include "mds_api/parser/json_parser/mds_json_parser.h"
#include "sutil/logger/spk_log.h"
#include "sutil/time/spk_times.h"
}

#include "SeverityLevel.h"
#include "StockTick.h"
#include <vector>

#include "AtmPluginInterface.h"
#include "AtmMarketDataPluginInterface.h"
#include "nn.hpp"
#include <nanomsg/pair.h>

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;

//std::function<int32(MdsApiSessionInfoT *,SMsgHeadT *,void *,void *)> MarketDataCallBack; 

class CKrQuantMDPluginImp:
	public MAtmMarketDataPluginInterface
{

//日志属性
	boost::log::sources::severity_logger< severity_levels > m_Logger;

//定时器属性
	io_service  m_IOservice;
	deadline_timer m_StartAndStopCtrlTimer;
	std::future<bool> m_futTimerThreadFuture;

	string m_strUsername;
	string m_strPassword;

//账号线程关键属性
	unsigned int m_uRequestID = 0;
	bool m_boolIsOnline = false;

//登录登出同步
	std::mutex m_mtxLoginSignal;
	condition_variable m_cvLoginSignalCV;

//观察者管理
	boost::shared_mutex m_mapObserverStructProtector;
	unordered_map<string, pair<CStockTick,list< tuple < MStrategy*, TMarketDataIdType, boost::shared_mutex*> > > > m_mapInsid2Strategys;
	unordered_map< MStrategy*, list<string> > m_mapStrategy2Insids;

	//MdsApiClientEnvT    cliEnv;

public:
	static const string s_strAccountKeyword;
	CKrQuantMDPluginImp();
	~CKrQuantMDPluginImp();
	int m_intRefCount = 0;
	atomic_bool m_abIsPending;
	bool IsPedding();
	nn::socket nnsocket;

	virtual bool IsOnline();
	virtual void IncreaseRefCount();
	virtual void DescreaseRefCount();
	virtual int GetRefCount();
	virtual void CheckSymbolValidity(const unordered_map<string, string> &);
	virtual string GetCurrentKeyword();
	virtual string GetProspectiveKeyword(const ptree &);
	virtual void GetState(ptree & out);
	virtual void MDInit(const ptree &);
	virtual void MDUnload();
	atomic<bool> m_adbIsPauseed;
	virtual void MDAttachStrategy(
		MStrategy *, 
		TMarketDataIdType,
		const unordered_map<string, string> &,
		boost::shared_mutex &,
		atomic_uint_least64_t *);
	virtual void MDDetachStrategy(MStrategy*/*IN*/);

	virtual void MDHotUpdate(const ptree &);
	virtual void Pause();
	virtual void Continue();
public:
	void MDDestoryAll();
	BOOL MDResubscribeByCodePrefix(MdsApiSessionInfoT *pTcpChannel,
        const char *pCodeListString,eMdsSubscribeModeT subMode);
	void OnWaitOnMsg();
    void ShowMessage(severity_levels,const char * fmt, ...);

private:
	bool Start();
	void Stop();
	void TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err);
	void OnError();

private:
    /* 内部的回调处理函数，可以考虑不定义在类中 */
    static void *       MdThreadMain(void *pParams);
};
#endif

