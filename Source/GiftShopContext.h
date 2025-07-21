#ifndef __GIFT_SHOP_CONTEXT_H
#define __GIFT_SHOP_CONTEXT_H

//struct GIFT_BUY_RANKING;
//struct GIFT_RANKING;

class GiftShopContext : public i3::shared_ginst<GiftShopContext>
{
private:
	enum WEEKLY_RANKING
	{
		LAST_WEEK = 0,	// ������ ��ŷ
		THIS_WEEK,		// �̹��� ��ŷ
		WEEK_MAX,	
	};

	i3::vector<GIFT_RANKING> m_GiftRankList[WEEK_MAX];

protected:
	void			_PushGiftRankData(INT32 i32Type, GIFT_RANKING & data);

	GIFT_RANKING *	_FindGiftRankingDataByIndex(INT32 i32Type, INT32 i32Idx);
	GIFT_RANKING *	_FindGiftRankingDataByRank(INT32 i32Type, INT32 i32Rank);
	GIFT_RANKING *	_FindGiftRankingDataByGiftID(INT32 i32Type, UINT32 GiftID);
	
public:
	enum GIFT_RANK_TYPE
	{
		GRT_INIT = -1,
		GRT_NONE = 0,	// ���� ����
		GRT_NEW,		// �ű� ����
		GRT_UP,			// ���� ���
		GRT_DOWN,	 	// ���� �϶�
	};

	GiftShopContext();
	~GiftShopContext();

	void			BuildGiftRankingData(GIFT_BUY_RANKING* RankInfo);

	UINT32			GetRankGiftID(INT32 i32Rank);
	INT32			GetRankType(INT32 i32Rank);
	INT32			GetRankAmount(INT32 i32Rank);
};


#endif // __GIFT_SHOP_CONTEXT_H