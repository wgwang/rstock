/************************************************************************/
/* �ļ����ƣ�StockInfoItem.h
/* ����ʱ�䣺2013-11-08 09:28
/*
/* ��    �������ڴ洢��ֻ��Ʊ����
/************************************************************************/
#ifndef STOCK_INFO_ITEM_H
#define STOCK_INFO_ITEM_H
#include <QtCore>
#include "rsd_global.h"
#include "RStockFunc.h"

class RSDSHARED_EXPORT CStockInfoItem : public QObject
{
	Q_OBJECT
public:
	CStockInfoItem(const QString& code, WORD market);
	CStockInfoItem(const qRcvBaseInfoData& info);
	~CStockInfoItem(void);

public:
	//����Report����
	void setReport(qRcvReportData* p);
	void setReport(RCV_REPORT_STRUCTExV3* p);
	qRcvReportData* getCurrentReport() const;

	//������������
	QList<qRcvHistoryData*> getHistoryList();
	QList<qRcvHistoryData*> getLastHistory(int count);
	void appendHistorys(const QList<qRcvHistoryData*>& list);

	//������ʷ5��������
	QList<RStockData*> get5MinList();

	//�����Ȩ����
	QList<qRcvPowerData*> getPowerList();
	void appendPowers(const QList<qRcvPowerData*>& list);

	//����ֱ�����
	QList<qRcvFenBiData*> getFenBiList();
	void appendFenBis(const QList<qRcvFenBiData*>& list);

	//����F10����
	void setBaseInfo(const qRcvBaseInfoData& info);
	qRcvBaseInfoData* getBaseInfo(){ return &baseInfo; }

	//�������5�յ��ܳɽ���
	void setLast5Volume(float fValue){ fLast5Volume = fValue; }

public:
	/*�������ֶΣ�ֻ��*/
	QString getCode() const;		//��Ʊ����
	WORD getMarket() const;			//��Ʊ�г�
	QString getName() const;		//��Ʊ����
	float getIncrease() const;	//�Ƿ�
	float getVolumeRatio() const;	//����
	float getTurnRatio() const;	//������
	float getLastClose() const;	//��һ�����̼�
	float getOpenPrice() const;	//���տ��̼�
	float getHighPrice() const;	//������߼�
	float getLowPrice() const;	//������ͼ�
	float getNewPrice() const;	//���¼�
	float getTotalVolume() const;	//��ȡ����
	float getTotalAmount() const;	//��ȡ����
	float getNowVolume() const;	//��ȡ����
	float getIncSpeed() const;	//����
	float getPriceFluctuate() const;//�ǵ����۸񲨶�
	float getAmplitude() const;	//���
	float getAvePrice() const;	//����
	float getPERatio() const;		//��ӯ��
	float getLTSZ() const;			//��ͨ��ֵ
	float getZGB() const;			//�ܹɱ�
	float getSellVOL() const;		//������
	float getBuyVOL() const;		//������
	float getBIDVOL() const;		//ί����
	float getASKVOL() const;		//ί����
	float getCommRatio() const;	//ί��
	float getCommSent() const;	//ί��

	float getLast5Volume(); //��ȡ��ȥ5�յĳɽ��������ڼ������ȣ�

	float getMgjzc();		//ÿ�ɾ��ʲ�
	float getMgsy();		//��ȡÿ������

	/*�жϹؼ���_key�Ƿ�ƥ��*/
	bool isMatch(const QString& _key);
protected:
	void updateItemInfo();
	void resetBuySellVOL();		//���¼�������������

signals:
	void stockItemReportChanged(const QString&);	//�������ݸ���
	void stockItemHistoryChanged(const QString&);	//��ʷ���ݸ���
	void stockItemFenBiChanged(const QString&);		//�ֱ����ݸ���

private:
	QString qsCode;					//����
	WORD wMarket;					//�г�����
	QString qsName;					//��Ʊ����
	float fIncrease;				//�Ƿ�
	float fVolumeRatio;				//����
	float fTurnRatio;				//������


	qRcvReportData* pLastReport;	//�����Report
	qRcvReportData* pCurrentReport;	//��ǰ��Report

	float fNowVolume;				//����
	float fIncreaseSpeed;			//�����ٶ�  (NewPrice-OldPrice)/OldPrice
	float fPriceFluctuate;			//�ǵ����۸񲨶�
	float fAmplitude;				//���
	float fAvePrice;				//����

	float fPERatio;					//��ӯ��
	float fLTSZ;					//��ͨ��ֵ
	float fZGB;						//�ܹɱ�
	float fSellVOL;					//������
	float fBuyVOL;					//������

	float fBuyVolume;				//ί����
	float fSellVolume;				//ί����
	float fCommRatio;				//ί��
	float fCommSent;				//ί��

	float fLast5Volume;				//��ȥ5�յĳɽ����������ڼ������ȣ�

	QList<QList<QChar>> shortName;	//��ƴ��

private:
	QMap<time_t,qRcvPowerData*> mapPowers;			//��Ȩ����
	QMultiMap<long,qRcvFenBiData*> mapFenBis;		//�ֱ�����
	QMap<time_t,RStockData*>* pMap5Min;				//���10�����ʷ5��������
	qRcvBaseInfoData baseInfo;
};

#endif	//STOCK_INFO_ITEM_H