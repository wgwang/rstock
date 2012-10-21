#pragma once
#include <QtCore>

struct qRcvHistoryData
{
	time_t	time;				//UCT
	float	fOpen;			//开盘
	float	fHigh;			//最高
	float	fLow;				//最低
	float	fClose;			//收盘
	float	fVolume;			//量
	float	fAmount;			//额
	WORD	wAdvance;			//涨数,仅大盘有效
	WORD	wDecline;			//跌数,仅大盘有效

	qRcvHistoryData(RCV_HISTORY_STRUCTEx* p)
	{
		memcpy(&time,&p->m_time,sizeof(qRcvHistoryData));
	}
};

struct qRcvReportData
{
	QDateTime	tmTime;			//成交时间
	DWORD		wMarket;		//股票市场类型
	QString		qsCode;			//股票代码
	QString		qsName;			//股票名称

	float	fLastClose;			//昨收
	float	fOpen;				//今开
	float	fHigh;				//最高
	float	fLow;				//最低
	float	fNewPrice;			//最新
	float	fVolume;			//成交量
	float	fAmount;			//成交额
	float	fBuyPrice[3];		//申买价1,2,3
	float	fBuyVolume[3];		//申买量1,2,3
	float	fSellPrice[3];		//申卖价1,2,3
	float	fSellVolume[3];		//申卖量1,2,3

	float	fBuyPrice4;			//申买价4
	float	fBuyVolume4;		//申买量4
	float	fSellPrice4;		//申卖价4
	float	fSellVolume4;		//申卖量4

	float	fBuyPrice5;			//申买价5
	float	fBuyVolume5;		//申买量5
	float	fSellPrice5;		//申卖价5
	float	fSellVolume5;		//申卖量5

	QMap<time_t,qRcvHistoryData> mapHistorys;

	qRcvReportData(RCV_REPORT_STRUCTExV3* p)
	{
		tmTime = QDateTime::fromTime_t(p->m_time);
		wMarket = p->m_wMarket;
		qsCode = QString::fromLocal8Bit(p->m_szLabel);
		qsName = QString::fromLocal8Bit(p->m_szName);

		//直接拷贝剩余的所有float数据
		memcpy(&fLastClose,&p->m_fLastClose,sizeof(float)*27);
/*		fLastClose = p->m_fLastClose;
		fOpen = p->m_fOpen;				//今开
		fHigh = p->m_fHigh;				//最高
		fLow = p->m_fLow;				//最低
		fNewPrice = p->m_fNewPrice;		//最新
		fVolume = p->m_fVolume;			//成交量
		fAmount = p->m_fAmount;			//成交额

		memcpy(fBuyPrice,p->m_fBuyPrice,sizeof(float)*3);	//申买价1,2,3
		memcpy(fBuyVolume,p->m_fBuyVolume,sizeof(float)*3);	//申买量1,2,3
		memcpy(fSellPrice,p->m_fSellPrice,sizeof(float)*3);	//申卖价1,2,3
		memcpy(fSellVolume,p->m_fSellVolume,sizeof(float)*3);	//申卖量1,2,3

		fBuyPrice4;			//申买价4
		fBuyVolume4;		//申买量4
		fSellPrice4;		//申卖价4
		fSellVolume4;		//申卖量4

		fBuyPrice5;			//申买价5
		fBuyVolume5;		//申买量5
		fSellPrice5;		//申卖价5
		fSellVolume5;		//申卖量5
		*/
	}

	void resetItem(RCV_REPORT_STRUCTExV3* p)
	{
		tmTime = QDateTime::fromTime_t(p->m_time);
		wMarket = p->m_wMarket;
		qsCode = QString::fromLocal8Bit(p->m_szLabel);
		qsName = QString::fromLocal8Bit(p->m_szName);

		//直接拷贝剩余的所有float数据
		memcpy(&fLastClose,&p->m_fLastClose,sizeof(float)*27);
	}
};

class CDataEngine : public QObject
{
	Q_OBJECT
public:
	CDataEngine(void);
	~CDataEngine(void);
	static CDataEngine* getDataEngine()
	{
		if(m_pDataEngine == NULL)
			m_pDataEngine = new CDataEngine;
		return m_pDataEngine;
	}

public:
	//重新初始化所有股票数据
	void initAllReport();

	//获取基本行情数据
	QList<qRcvReportData*> getBaseMarket()
	{
		return m_mapBaseMarket.values();
	}
	qRcvReportData* getBaseMarket(const QString& qsCode)
	{
		if(m_mapBaseMarket.contains(qsCode))
		{
			return m_mapBaseMarket[qsCode];
		}
		return NULL;
	}
	void setBaseMarket(qRcvReportData* p)
	{
		m_mapBaseMarket[p->qsCode] = p;
	}

	//补充日线数据
	bool appendHistory(const QString& code, const qRcvHistoryData& p)
	{
		if(!m_mapBaseMarket.contains(code))
			return false;
		qRcvReportData* pData = m_mapBaseMarket[code];
		pData->mapHistorys.insert(p.time,p);

		return true;
	}

private:
	QMap<QString,qRcvReportData*> m_mapBaseMarket;

private:
	static CDataEngine* m_pDataEngine;
};

