#pragma once

#include "MCardPrizeInfo.h"

struct MCardInfo;
typedef i3::scoped_ptr<MCardInfo>	MCardInfoPtr;

struct MCardSetInfo : public i3::class_object_pool<MCardSetInfo>
{
	INT32			setID = -1;
	i3::rc_wstring	wstrName;
	i3::rc_wstring	wstrTitle;
	i3::rc_wstring	wstrFileName;
	i3::rc_wstring  wstrUITemplateName;
	INT32			UITemplateIndex = -1;

	INT32			totExp = 0;
	INT32			totPoint = 0;
	INT32			bonusBadgeMaster = 0;
	i3::rc_wstring	wstrEventPeriod;				// �̺�Ʈ �Ⱓ..�̺�Ʈ �����ۿ��� �ش�ɰ���..
	
	MCardPrizeInfo_Mqf	prizeCompletion;			// ��� ī�� �Ϸ�� ���� ����... (vecBonusItem ��ü�뵵)
	UINT32			goodsID = 0;						// �̰��� �����κ��� �α�� �ý�����Ŷ�� �����ڿ���� ������ �ȴ�.

	MCardInfoPtr mCardInfo[ MAX_CARD_PER_CARDSET ];
};

void InitMCardSetInfo(MCardSetInfo* cardSet);

