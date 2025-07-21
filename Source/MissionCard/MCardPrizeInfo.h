#pragma once


struct MCardPrizeInfo_Mqf_ItemInfo
{
	UINT32		m_ui32ItemID = 0;				// ������ �������� �ڵ�
	UINT32		m_ui32Arg = 0;					// �� �������� ��� �ٰ��ΰ�
	INT8		m_i8AuthType = 0;				// ������ �������� ���� Ÿ�� ( ������ , �Ⱓ�� )
};

struct MCardPrizeInfo_Mqf
{
	UINT32	point = 0;
	UINT32  exp = 0;
	UINT32  prize = 0;		
	i3::svector<MCardPrizeInfo_Mqf_ItemInfo>	vecPrizeItem;
};

void InitMCardPrizeInfo_Mqf(MCardPrizeInfo_Mqf& prizeInfo);
void ResetMCardPrizeInfo_Mqf(MCardPrizeInfo_Mqf& prizeInfo);
