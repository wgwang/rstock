#include "StdAfx.h"
#include "CoordXBaseWidget.h"
#include "DataEngine.h"
extern lua_State* g_Lua;
extern QString g_native;
extern luaL_Reg* g_pFuncs;


//计算分时数据的横坐标时间
int getTimeMapByMin(QMap<time_t,int>& mapTimes,time_t& tmBegin, time_t& tmEnd, int iSize = 60/*second*/)
{
	if(tmBegin>tmEnd)
		return 0;

	time_t tmCur = tmEnd;
	while(tmCur>=tmBegin)
	{
		//		QString qsTime = QDateTime::fromTime_t(tmCur).toString();
		mapTimes.insert(tmCur,mapTimes.size());
		tmCur = tmCur-iSize;
	}
	return 1;
}

int getDayMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	time_t tmNow = (_tmEnd+3600*24);			//需多计算一个周期
	while(tmNow>=_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		tmNow -= 3600*24;
	}

	return 1;
}

int getWeekMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addDays(8-dtNow.dayOfWeek());	//多计算一个周期
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		tmNow -= (3600*24*7);
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getMonthMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addMonths(1);					//多计算一个周期
	dtNow = QDate(dtNow.year(),dtNow.month(),1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addMonths(-1);
		dtNow = QDate(dtNow.year(),dtNow.month(),1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getMonth3MapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();
	dtNow = dtNow.addMonths(3);					//多计算一个周期
	dtNow = QDate(dtNow.year(),(dtNow.month()-1)/3*3,1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addMonths(-3);
		dtNow = QDate(dtNow.year(),(dtNow.month()-1)/3*3,1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

int getYearMapByHistory(QMap<time_t,int>& mapTimes,time_t& _tmBegin, time_t& _tmEnd)
{
	QDate dtNow = QDateTime::fromTime_t(_tmEnd).date();

	dtNow = dtNow.addYears(1);					//多计算一个周期
	dtNow = QDate(dtNow.year(),1,1);
	time_t tmNow = QDateTime(dtNow).toTime_t();

	while(tmNow>_tmBegin)
	{
		mapTimes.insert(tmNow,mapTimes.size());
		dtNow = dtNow.addYears(-1);
		dtNow = QDate(dtNow.year(),1,1);
		tmNow = QDateTime(dtNow).toTime_t();
	}

	mapTimes.insert(tmNow,mapTimes.size());
	return 1;
}

/*通过分钟数据获取指定周期内的数据*/
RStockData* getColorBlockItemByMins(const QList<qRcvFenBiData*>& list, qRcvFenBiData* pLastFenbi)
{
	if(list.size()<1)
		return NULL;

	RStockData* pData = new RStockData;
	//	qRcvFenBiData* pBegin = list.first();
	qRcvFenBiData* pLast = list.last();
	qRcvFenBiData* pFirst = list.first();
	pData->tmTime = pFirst->tmTime;
	pData->fOpen = pFirst->fPrice;
	pData->fClose = pLast->fPrice;
	pData->fHigh = pFirst->fPrice;
	pData->fLow = pFirst->fPrice;

	if(pLastFenbi)
	{
		pData->fAmount = pLast->fAmount-pLastFenbi->fAmount;
		pData->fVolume = pLast->fVolume-pLastFenbi->fVolume;
	}
	else
	{
		pData->fAmount = pLast->fAmount;
		pData->fVolume = pLast->fVolume;
	}

	foreach(qRcvFenBiData* p,list)
	{
		if(pData->fLow>p->fPrice)
			pData->fLow = p->fPrice;
		if(pData->fHigh<p->fPrice)
			pData->fHigh = p->fPrice;
	}
	return pData;
}

/*通过多天数据获取一个周期内的数据*/
RStockData* getColorBlockItemByDays(const QList<qRcvHistoryData*>& list)
{
	if(list.size()<1)
		return NULL;
	RStockData* pData = new RStockData();

	qRcvHistoryData* pFirst = list.first();
	qRcvHistoryData* pLast = list.last();
	pData->tmTime = pFirst->time;
	pData->fOpen = pFirst->fOpen;
	pData->fClose = pLast->fClose;


	pData->fLow = pFirst->fLow;
	pData->fHigh = pFirst->fHigh;
	pData->fAmount = 0;
	pData->fVolume = 0;
	foreach(qRcvHistoryData* p,list)
	{
		if(pData->fLow>p->fLow)
			pData->fLow = p->fLow;
		if(pData->fHigh<p->fHigh)
			pData->fHigh = p->fHigh;
		pData->fAmount+=p->fAmount;
		pData->fVolume+=p->fVolume;
	}
	return pData;
}

/*将分笔数据按照时间轴排序*/
QMap<time_t,RStockData*>* getColorBlockItems(QMap<time_t,int>& mapTimes, const QList<qRcvFenBiData*>& minutes)
{
	QMap<time_t,RStockData*>* pMap = new QMap<time_t,RStockData*>();
	if(mapTimes.size()<1)
	{
		return pMap;
	}

	QMap<time_t,int>::iterator iter = mapTimes.begin();

	time_t tmBegin = iter.key();
	if(mapTimes.size()<2)
	{
		QList<qRcvFenBiData*> listFenBi;
		foreach(qRcvFenBiData* pFenBi,minutes)
		{
			if(pFenBi->tmTime>tmBegin)
				listFenBi.push_back(pFenBi);
		}
		pMap->insert(tmBegin,getColorBlockItemByMins(listFenBi,NULL));
		return pMap;
	}

	time_t tmEnd = (iter+1).key();

	QList<qRcvFenBiData*> listPer;
	qRcvFenBiData* pLastFenBi = NULL;
	foreach(qRcvFenBiData* pFenBi,minutes)
	{
		if(pFenBi->tmTime<tmBegin)
		{
			pLastFenBi = pFenBi;
			continue;
		}
		else if(pFenBi->tmTime>=tmBegin&&pFenBi->tmTime<=tmEnd)
		{
			listPer.push_back(pFenBi);
		}
		else
		{
			if(listPer.size()>0)
				pLastFenBi = listPer.last();
			pMap->insert(tmBegin,getColorBlockItemByMins(listPer,NULL));
			listPer.clear();

			++iter;
			while (iter!=mapTimes.end())
			{
				tmBegin = iter.key();
				if(iter!=mapTimes.end())
					tmEnd = (iter+1).key();
				else
					tmEnd = tmBegin+3600*24*1000;		//加1000天的时间

				if(pFenBi->tmTime>=tmBegin&&pFenBi->tmTime<=tmEnd)
				{
					listPer.push_back(pFenBi);
					break;
				}
				else
				{
					pMap->insert(tmBegin,NULL);
				}
				++iter;
			}

			if(iter==mapTimes.end())
				break;
		}
	}

	//获取最后一个数据块，并进行计算
	RStockData* pLastData = getColorBlockItemByMins(listPer,pLastFenBi);
	while(iter!=mapTimes.end())
	{
		tmBegin = iter.key();
		if(pLastData && pLastData->tmTime>=tmBegin)
		{
			pMap->insert(tmBegin,pLastData);
			pLastData = NULL;
		}
		else
		{
			pMap->insert(tmBegin,NULL);
		}
		++iter;
	}
	delete pLastData;

	if(pMap->size()!=mapTimes.size())
	{
		qDebug()<<"FenBi Map is not enouph";
	}

	return pMap;
}

/*将日线按照时间轴排序*/
QMap<time_t,RStockData*>* getColorBlockItems(QMap<time_t,int>& mapTimes, const QList<qRcvHistoryData*>& minutes)
{
	QMap<time_t,RStockData*>* pMap = new QMap<time_t,RStockData*>();
	if(mapTimes.size()<1)
		return pMap;

	QMap<time_t,int>::iterator iter = mapTimes.begin();

	time_t tmBegin = iter.key();
	if(mapTimes.size()<2)
	{
		QList<qRcvHistoryData*> listFenBi;
		foreach(qRcvHistoryData* pFenBi,minutes)
		{
			if(pFenBi->time>tmBegin)
				listFenBi.push_back(pFenBi);
		}
		pMap->insert(tmBegin,getColorBlockItemByDays(listFenBi));
		return pMap;
	}

	time_t tmEnd = (iter+1).key();

	QList<qRcvHistoryData*> listPer;
	qRcvHistoryData* pLastFenBi = NULL;
	foreach(qRcvHistoryData* pFenBi,minutes)
	{
		if(pFenBi->time<tmBegin)
		{
			pLastFenBi = pFenBi;
			continue;
		}
		else if(pFenBi->time>=tmBegin&&pFenBi->time<=tmEnd)
			listPer.push_back(pFenBi);
		else
		{
			if(listPer.size()>0)
				pLastFenBi = listPer.last();
			pMap->insert(tmBegin,getColorBlockItemByDays(listPer));
			listPer.clear();

			++iter;
			while (iter!=mapTimes.end())
			{
				tmBegin = iter.key();
				if(iter!=mapTimes.end())
					tmEnd = (iter+1).key();
				else
					tmEnd = tmBegin+3600*24*1000;		//加1000天的时间

				if(pFenBi->time>=tmBegin&&pFenBi->time<=tmEnd)
				{
					listPer.push_back(pFenBi);
					break;
				}
				else
				{
					pMap->insert(tmBegin,NULL);
				}
				++iter;
			}

			if(iter==mapTimes.end())
				break;
		}
	}

	//获取最后一个数据块，并进行计算
	RStockData* pLastData = getColorBlockItemByDays(listPer);
	while(iter!=mapTimes.end())
	{
		tmBegin = iter.key();
		if(pLastData && pLastData->tmTime>=tmBegin)
		{
			pMap->insert(tmBegin,pLastData);
			pLastData = NULL;
		}
		else
		{
			pMap->insert(tmBegin,NULL);
		}
		++iter;
	}
	delete pLastData;

	if(pMap->size()!=mapTimes.size())
	{
		qDebug()<<"Day Map is not enouph";
	}
	return pMap;
}


CCoordXBaseWidget::CCoordXBaseWidget(CBaseWidget* parent /*= 0*/, RWidgetType type /*= CBaseWidget::Basic*/)
	: CBaseWidget(parent,type)
	, m_typeCircle(Min1)
	, m_pMenuCustom(0)

{
	{
		m_pL = luaL_newstate();
		luaL_openlibs(m_pL);
		luaL_dostring(m_pL,g_native.toLocal8Bit());
		if(lua_type(m_pL,-1)==LUA_TSTRING)
		{
			const char* aa = lua_tostring(m_pL,-1);
			QString ss = QString::fromLocal8Bit(aa);
			qDebug()<<"Load native.lua error:"<<ss;
		}
		lua_pushglobaltable(m_pL);
		luaL_setfuncs(m_pL, g_pFuncs,0);
		lua_pop(m_pL,1);
	}
	m_pMenuCustom = new QMenu(tr("色块图菜单"));
	{
		//设置当前K线图的显示周期
		m_pMenuCircle = m_pMenuCustom->addMenu(tr("周期设置"));
		m_pMenuCircle->addAction(tr("10秒分时图"),this,SLOT(onSetCircle()))->setData(Sec10);
		m_pMenuCircle->addAction(tr("30秒分时图"),this,SLOT(onSetCircle()))->setData(Sec30);
		m_pMenuCircle->addAction(tr("1分钟分时图"),this,SLOT(onSetCircle()))->setData(Min1);
		m_pMenuCircle->addAction(tr("5分钟分时图"),this,SLOT(onSetCircle()))->setData(Min5);
		m_pMenuCircle->addAction(tr("15分钟分时图"),this,SLOT(onSetCircle()))->setData(Min15);
		m_pMenuCircle->addAction(tr("30分钟分时图"),this,SLOT(onSetCircle()))->setData(Min30);
		m_pMenuCircle->addAction(tr("60分钟分时图"),this,SLOT(onSetCircle()))->setData(Min60);

		m_pMenuCircle->addAction(tr("日线图"),this,SLOT(onSetCircle()))->setData(Day);
		m_pMenuCircle->addAction(tr("周线图"),this,SLOT(onSetCircle()))->setData(Week);
		m_pMenuCircle->addAction(tr("月线图"),this,SLOT(onSetCircle()))->setData(Month);
		m_pMenuCircle->addAction(tr("季线图"),this,SLOT(onSetCircle()))->setData(Month3);
		m_pMenuCircle->addAction(tr("年线图"),this,SLOT(onSetCircle()))->setData(Year);
	}
	//m_pL = lua_newthread(g_Lua);
}


CCoordXBaseWidget::~CCoordXBaseWidget(void)
{
}

bool CCoordXBaseWidget::loadPanelInfo( const QDomElement& eleWidget )
{
	if(!CBaseWidget::loadPanelInfo(eleWidget))
		return false;

	//当前显示的周期
	if(eleWidget.hasAttribute("circle"))
	{
		m_typeCircle = static_cast<RStockCircle>(eleWidget.attribute("circle").toInt());
	}

	return true;
}

bool CCoordXBaseWidget::savePanelInfo( QDomDocument& doc,QDomElement& eleWidget )
{
	if(!CBaseWidget::savePanelInfo(doc,eleWidget))
		return false;

	//显示的周期
	eleWidget.setAttribute("circle",m_typeCircle);

	return true;
}

void CCoordXBaseWidget::updateData()
{
	updateTimesH();
}

void CCoordXBaseWidget::updateTimesH()
{
	//更新当前的横坐标数据，从后向前计算时间
	m_mapTimes.clear();

	//	int iCount = 1024;				//计算1024个时间
	if(m_typeCircle<Day)
	{
		time_t tmCur = CDataEngine::getCurrentTime();
		time_t tmLast = ((tmCur/(3600*24))*3600*24)+3600*(9-8)+60*25;	//9：25开盘
		time_t tmCurrent = (tmCur+m_typeCircle*2)/m_typeCircle*m_typeCircle;//向上对分钟取整
		time_t tmNoon1 = ((tmCur/(3600*24))*3600*24)+3600*(11-8)+60*30;
		time_t tmNoon2 = ((tmCur/(3600*24))*3600*24)+3600*(13-8);


		if((tmCurrent%(3600*24))>3600*7)
		{
			tmCurrent = (tmCurrent/(3600*24))*3600*24 + 3600*7 + m_typeCircle;		//3点收盘(多加一个周期)
		}
		/*需向上和向下多计算一个周期*/
		if(tmCurrent>tmNoon2)
		{
			time_t tmBegin = tmNoon2-m_typeCircle;
			//time_t tmEnd = tmCurrent+m_typeCircle*2;
			getTimeMapByMin(m_mapTimes,tmBegin,tmCurrent,m_typeCircle);
		}

		if(tmCurrent>tmNoon1)
		{
			time_t tmBegin = tmLast-m_typeCircle;
			time_t tmEnd = tmNoon1+m_typeCircle;
			getTimeMapByMin(m_mapTimes,tmBegin,tmEnd,m_typeCircle);
		}
		else if(tmCurrent>tmLast)
		{
			time_t tmBegin = tmLast-m_typeCircle;
			time_t tmEnd = tmCurrent;
			getTimeMapByMin(m_mapTimes,tmBegin,tmEnd,m_typeCircle);
		}
	}
	else
	{
		time_t tmBegin = QDateTime(QDate(2000,1,1)).toTime_t();
		time_t tmEnd = (CDataEngine::getCurrentTime()/(3600*24))*3600*24 - 8*3600;

		if(m_typeCircle == Day)
		{
			getDayMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == DayN)
		{
			//目前未使用
		}
		else if(m_typeCircle == Week)
		{
			getWeekMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == Month)
		{
			getMonthMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == Month3)
		{
			getMonth3MapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
		else if(m_typeCircle == Year)
		{
			getYearMapByHistory(m_mapTimes,tmBegin,tmEnd);
		}
	}
}

void CCoordXBaseWidget::updateShowTimes( const QRectF& rtCoordX,float fItemWidth )
{
	m_mapShowTimes.clear();
	//从右向左绘制横坐标
	float fBeginX = rtCoordX.right();
	float fEndX = rtCoordX.left();
	float fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;

	QList<time_t> listTimes = m_mapTimes.keys();
	float fCurX = fBeginX-fEndX;
	int iCount = listTimes.size()-1;

	while(fCurX>fEndX && iCount>=0)
	{
		m_mapShowTimes[listTimes[iCount]] = fCurX;

		--iCount;
		fCurX = fCurX- fItemWidth;
	}
	return;
}

void CCoordXBaseWidget::drawCoordX(QPainter& p,const QRectF& rtCoordX,float fItemWidth)	//绘制X坐标轴
{
	//从右向左绘制横坐标
	float fBeginX = rtCoordX.right();
	float fEndX = rtCoordX.left();
	float fCBWidth = fBeginX-fEndX;
	if(fCBWidth<0)
		return;

	QList<time_t> listTimes = m_mapTimes.keys();
	float fCurX = fBeginX-fItemWidth;
	float fLastX = fBeginX;
	int iCount = listTimes.size()-1;

	int iTimeCount = 0;				//只是用来区分时间的颜色（隔开颜色，便于查看）
	while(fCurX>fEndX && iCount>=0)
	{
		if(m_typeCircle<Day)
		{
			if((fLastX-fCurX)>30)
			{
				p.setPen( iTimeCount%2 ? QColor(255,0,0) : QColor(0,255,255));
				p.drawLine(fCurX,rtCoordX.top(),fCurX,rtCoordX.top()+2);
				p.drawText(fCurX-14,rtCoordX.top()+2,30,rtCoordX.height()-2,
					Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("hh:mm"));
				fLastX = fCurX;
				++iTimeCount;
			}
		}
		else
		{
			if((fLastX-fCurX)>80)
			{
				p.setPen( iTimeCount%2 ? QColor(255,0,0) : QColor(0,255,255));
				p.drawLine(fCurX,rtCoordX.top(),fCurX,rtCoordX.top()+2);
				switch(m_typeCircle)
				{
				case Week:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QString("%1 %2").arg(dtmTmp.date().year()).arg(dtmTmp.date().weekNumber()));
					}
					break;
				case Month:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy/MM"));
					}
					break;
				case Month3:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy/MM"));
					}
					break;
				case Year:
					{
						QDateTime dtmTmp = QDateTime::fromTime_t(listTimes[iCount]);
						p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
							Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy"));
					}
					break;
				default:
					p.drawText(fCurX-40,rtCoordX.top()+2,80,rtCoordX.height()-2,
						Qt::AlignCenter,QDateTime::fromTime_t(listTimes[iCount]).toString("yyyy/MM/dd"));
					break;
				}
				fLastX = fCurX;
				++iTimeCount;
			}
		}

		--iCount;
		fCurX = fCurX- fItemWidth;
	}
	return;
}

void CCoordXBaseWidget::onSetCircle()
{
	//设置当前的显示周期
	QAction* pAct = reinterpret_cast<QAction*>(sender());
	m_typeCircle = static_cast<RStockCircle>(pAct->data().toInt());
	updateData();
}

QMap<time_t,RStockData*>* CCoordXBaseWidget::getColorBlockMap( CStockInfoItem* pItem )
{
	if(!pItem)
		return new QMap<time_t,RStockData*>();
	QMap<time_t,RStockData*>* pMap = NULL;
	if(m_typeCircle < Day)
	{
		//获取分钟数据，进行计算
		QList<qRcvFenBiData*> FenBis = pItem->getFenBiList();

		pMap = getColorBlockItems(m_mapTimes,FenBis);
	}
	else
	{
		//获取日线数据
		QList<qRcvHistoryData*> historys = pItem->getHistoryList();
		qRcvReportData* pLastReport = pItem->getCurrentReport();
		bool bAppendLast = true;
		if(historys.size()>0 && pLastReport)
		{
			qRcvHistoryData* pLastHistory = historys.last();
			if(QDateTime::fromTime_t(pLastHistory->time).date() == QDateTime::fromTime_t(pLastReport->tmTime).date())
				bAppendLast = false;
		}
		if(pLastReport&&bAppendLast)
		{
			historys.push_back(new qRcvHistoryData(pLastReport));
		}
		pMap = getColorBlockItems(m_mapTimes,historys);
		{
			//清除获取的日线数据
			foreach(qRcvHistoryData* p,historys)
				delete p;
			historys.clear();
		}
	}

	return pMap;
}
