#if !defined( __QPI_H)
#define __QPI_H

extern "C"
{
	// ���� Quest�� Argument Value�� ���´�.
	// idx : 0~5
	INT32				QPI_GetValue( INT32 idx);

	// ���� Quest�� Count Value�� �����Ѵ�.
	// idx : 0~5
	void				QPI_SetValue( INT32 idx, UINT8 value);

	// ���� ������ ��� �ִ� ������ ī�װ��� ��ȯ�Ѵ�.
	WEAPON_CLASS_TYPE	QPI_GetWeaponClassType(void);

	// ���� ����ڰ� ��� �ִ� ������ ������ ��ȯ�Ѵ�.
	// WEAPON_CLASS_TYPE�� ���� �Ʒ��� ���� �پ��� ī�װ��� ������� ��ȯ�ȴ�.
	//
	// GetItemIndex("ASSAULT")
	// GetItemIndex("SMG")
	// GetItemIndex("SNIPER")
	// GetItemIndex("HANDGUN")
	// GetItemIndex("SHOTGUN")
	// GetItemIndex("KNIFE")
	// GetItemIndex("THROWING")
	// GetItemIndex("MISSION")
	// GetItemIndex("MG")
	INT32				QPI_GetWeaponType(void);

	//���� ����ڰ� ����ִ� ������ ���� ���������� ��ȯ�Ѵ� ( GetItemIndex("ASSAULT_M4A1_SCOPE2")�� GetItemIndex("ASSAULT_M4A1_CAMO") �� ���� GetItemIndex("ASSAULT_M4A1")�� ��ȯ)
	INT32				QPI_GetWeaponTypeEx(void);

	INT32				QPI_GetWeapon_DerivedFrom( WEAPON_CLASS_TYPE type, INT32 num);

	// ���ΰ��� �ٸ� ĳ���͸� ���� ���, �Ѿ��� ���� ��� ĳ������ ������ ��ȯ�Ѵ�.
	HIT_CHARA_PART		QPI_GetHitPart(void);

	// �ٸ� ĳ���Ϳ� ���� ���ΰ��� ������ ������ ���, ���ΰ��� �ǰ� ������ ��ȯ�Ѵ�.
	HIT_CHARA_PART		QPI_GetHittedPart(void);

	// ���� Stage������ �� Kill Count�� ��ȯ�Ѵ�.
	INT32				QPI_GetKillCount(void);

	// ������ Respawn ���ĺ����� Kill Count�� ��ȯ�Ѵ�.
	INT32				QPI_GetKillCountInALife(void);

	// ��������� ���� Kill Count�� ��ȯ�Ѵ�.
	INT32				QPI_GetKillCountTotal(void);

	// ���� Stage������ Death Count�� ��ȯ�Ѵ�.
	INT32				QPI_GetDeathCount(void);

	// ��������� ���� Death Count�� ��ȯ�Ѵ�.
	INT32				QPI_GetDeathCountTotal(void);

	// ���� Stage�� Type
	STAGE_TYPE			QPI_GetStageType(void);

	// ���� Stage�� ID
	STAGE_ID			QPI_GetStage(void);

	// ���� ���θ� ��ȯ�Ѵ�.
	BOOL				QPI_IsIntercept(void);

	// Round ���� Stage�� ���, �¸� Round ȸ���� ��ȯ�Ѵ�.
	INT32				QPI_GetWinRoundCount(void);

	// Round ���� Stage�� ���, �й� Round ȸ���� ��ȯ�Ѵ�.
	INT32				QPI_GetDefeatRoundCount(void);

	// Round ���� Stage�� ���, ��ü Round ȸ���� ��ȯ�Ѵ�.
	INT32				QPI_GetTotalRoundCount(void);

	// ���� Round ���� Stage�� �ƴ� ���, ��ü �ð��� �� ������ ��ȯ�Ѵ�. �׷��� �ʴٸ�, �� Round�� �ð� ������ �� ������ ��ȯ�Ѵ�.
	INT32				QPI_GetLimitTime(void);

	// ���� Kill Count�� �¸� ���ǿ� �ִٸ�, �ش� ������ ��ȯ�Ѵ�. �׷��� �ʴٸ� 0�� ��ȯ�Ѵ�.
	INT32				QPI_GetLimitKillCount(void);

	// ������� ����� ��ȯ�Ѵ�. (0~51)
	INT32				QPI_GetPlayerRank(void);

	// ������� �������� ����� �Ҽ����� ���� ��ŷ�� ��ȯ�Ѵ�.
	INT32				QPI_GetPlayerStageRank(void);

}

#endif
