#include "pch.h"
#include "MCardSetInfo.h"




void InitMCardSetInfo(MCardSetInfo* cardSet) 
{
	cardSet->setID = -1;
	cardSet->totExp = 0;
	cardSet->totPoint = 0;
	cardSet->bonusBadgeMaster = 0;
	InitMCardPrizeInfo_Mqf(cardSet->prizeCompletion);
	cardSet->goodsID = 0;				// 0 �� ��� ��ǰ�� �ƴ� ����ī������� �����Ѵ�..
}
