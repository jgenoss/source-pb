#ifndef _EquipShapeManager_h
#define _EquipShapeManager_h

#include "../../../Common/CommonSource/WeaponDefine.h"

#include "CSI_Stage.h"

class CEquipShapeManager : public i3ElementBase
{
	I3_CLASS_DEFINE(CEquipShapeManager, i3ElementBase);

protected:
	typedef i3::vector_map< i3::rc_string, i3::vector<i3Texture*> > TextureFileList;	// ����ũ��� 12����Ʈ������ ������ �־ ũ�� �������� ����..(2012.05.31.����)
	TextureFileList m_TextureList;
	i3Texture* GetShapeTexture(const i3::rc_string& TextureName, INT32 Index);
	i3::vector<i3Texture*>& GetTextureList(const i3::rc_string& TextureName);

private:
	i3Texture *		m_pWeaponDescImage; //���� �κ��丮���� ����ϴ� ���� ���� ������ 
	i3Texture *		m_BadgeTexture;	// ���� ����
	i3Texture *		m_pCardTexture;		// �ӹ�ī�� ����
	i3Texture *		m_pEventCardSet;	// �̺�Ʈī��� �̹��� ����
	UINT8			m_iMaxWeaponKillSize;

	i3Texture * m_pBossStartTexture;	//Boss Stage ���� �̹���

	void InitTextureList();


public:
	//������ : ���� �̹���

	//INT32 : ���� ��ȣ �ε���
	//INT32 : ���� �̹��� �ε���
	typedef i3::vector_map< INT32, INT32 > ImageIndexMap;

	//INT32 : ���� Ÿ�� �ε���
	//ImageIndexMap : ���� �̹��� ��
	typedef i3::vector_map< INT32, ImageIndexMap > WeaponImageMap;

	WeaponImageMap mWeaponImageMap;

	//������ : �� �̹���
	//std::string : �� �̸�
	//INT32 : �� �̹��� �ε���
	typedef i3::vector_map< i3::string, INT32 > StageImageMap;
	StageImageMap m_StageImageMap;

	//������ : ���� �� �̹���
	//std::string : �� �̸�
	//INT32 : �� �̹��� �ε���
	StageImageMap m_EscapeImageMap;

public:
	//���� �̹��� �ε����� �ʱ�ȭ �Ѵ�.
	//pWeaponListKey : ���� Pef ���Ͽ� �ش�Ǵ� I3RegKey�� �ְ��ش�.
	//���� :	weapon.pef ���� ����
	void _InitWeaponImageIndex(i3RegKey* pWeaponListKey);

	//�� �̹��� �ε����� �ʱ�ȭ �Ѵ�.
	//pStageListKey : �� Pef ���Ͽ� �ش�Ǵ� I3RegKey�� �ְ��ش�.
	//���� :	stage.pef ���� ����
	void _InitStageImageIndex(i3RegKey* pStageListKey);


private:
	INT32 _GetWeaponOrdIdx(WEAPON_CLASS_TYPE iWeaponClass, WEAPON::RES_ID iWeaponNum);

	//���� �̹��� �ε����� �߰��Ѵ�.
	//Type : ���� Ÿ��
	//Number : ���� ��ȣ
	//Index : ���� �̹��� �ε���
	void _AddWeaponImageIndex(WEAPON_CLASS_TYPE Type, WEAPON::RES_ID Number, INT32 Index);

	//�� �̹��� �ε����� �߰��Ѵ�.
	//���� :	�̹� ��ϵ� �� �ε����� ���� ���, ��� ������.
	//IndexBuf : Pef�� ��ϵ� Content �̸��� �ȴ�.
	//ShapeIndex : �� �̹��� �ε���
	void _AddStageImageIndex(const char* IndexBuf, INT32 ShapeIndex);

	//���� �� �̹��� �ε����� �߰��Ѵ�.
	//�޸� :	pef�� ��� ���� ���� _BossUIShapeIndex �� -1�� ���õȴ�.
	//���� :	�̹� ��ϵ� �� �ε����� ���� ���, ��� ������.
	//IndexBuf : Pef�� ��ϵ� Content �̸��� �ȴ�.
	//ShapeIndex : ���� �� �̹��� �ε���
	void _AddEscapeImageIndex(const char* IndexBuf, INT32 ShapeIndex);

public:
	CEquipShapeManager();
	virtual ~CEquipShapeManager();

	//Resource 
	void CreateGlobalImageRes(void);			//��ü ���ҽ� 
	void DestroyGlobalImageRes(void);

	void CreateReadyStageImageRes(void);		//����� ���ҽ� 
	void DestroyReadyStageImageRes(void);

	void CreateBattleStageImageRes(void);		//��Ʋ ���ҽ� 
	void DestroyBattleStageImageRes(void);

	void CreateWeaponDescImageRes(INT32 nWeaponFlag, i3GuiStatic* pDescStatic1, i3GuiStatic* pDescStatic2);
	void DestroyWeaponDescImageRes(void);

	INT32 GetWeaponOrdIdx(WEAPON_CLASS_TYPE iWeaponClass, WEAPON::RES_ID iWeaponNum) { return _GetWeaponOrdIdx(iWeaponClass, iWeaponNum); }	// 11.05.19.����. (UI������ ������ �Լ�������.)

	INT32 GetCharacterShape(T_ItemID ItemID);

	bool SetWeaponKillMark(i3Sprite2D* pSprite, WEAPON_CLASS_TYPE nWeaponClass, WEAPON::RES_ID nWeaponNum, bool bHP_Up, bool bHeadShot);	// revision 57808
	bool SetWeaponSelectMark(i3GuiStatic* pStatic, WEAPON_CLASS_TYPE nWeaponClass, WEAPON::RES_ID nWeaponNum);			//
	bool SetCharacterShape(i3GuiStatic* pStatic, T_ItemID ItemID);					//

	INT32	GetMapIdx(STAGE_ID id);
	INT32	GetMapIdx(STAGE_UID ord);
	INT32	GetEscapeMapIdx(STAGE_ID id);
	INT32	GetEscapeMapIdx(STAGE_UID ord);

	INT32	GetRandomMapIdx(void);
	bool SetRandomMapShape(i3GuiStatic* pStatic);//
	bool SetCashItemShape(i3GuiStatic* pStatic, T_ItemID ItemID);
	bool SetPackageGoodsShape(i3GuiStatic* pStatic, INT32 GoodsId);
	bool SetCardShape(i3GuiStatic* pStatic, INT32 cardSetIdx, INT32 cardIdx);							//
	bool SetCardSetShape(i3GuiStatic* pStatic, INT32 cardSetIdx);									//
	bool SetRibbonShape(i3GuiStatic* pStatic, INT32 ribbonId);										//
	bool SetEnsignShape(i3GuiStatic* pStatic, INT32 ensignId);										//
	bool SetMedalShape(i3GuiStatic* pStatic, INT32 medalId);											//

	void CreateBossStageStartImageRes();
};

//bool	GetPackageItemShapeInfo( UINT32 GoodsID, SHAPE_TEXTURE_TYPE & TexType, INT32 & TexIndex, INT32 & ShapeIndex );

#endif //_EquipShapeManager_h
