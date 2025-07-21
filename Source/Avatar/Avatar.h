#pragma once

#include "../../Avatar/AvatarDefine.h"
#include "GameCharaBase.h"

class CGameCharaManager;
class AvatarGen;

typedef void (*AvatarLoadedCallback)(void * pAvatar, INT32 idx, AI_BIND_DATA * pAIBind);

class Avatar : public CGameCharaBase
{
	I3_CLASS_DEFINE( Avatar, CGameCharaBase); 

	friend class CGameCharaEquipContext;
	friend class cAvatarManager;
	friend class CGameCharaManager;

private:
	i3::vector<i3VertexArray*>		m_EquipmentVAList;	// ���� VA��

	struct tagTeamBandInfo {
		i3Node *			_pNode = nullptr;
		i3TextureBindAttr *	_pAttr = nullptr;
	};
	i3::vector<tagTeamBandInfo>		m_TeamBandSgList;

	typedef i3::vector<i3Node*>		tagHeadResInfo;
	i3::vector<tagHeadResInfo>		m_HeadSgList;	// �Ӹ� ���ҽ��� ��ü�ϱ� ���� ����Ʈ (0, 1, 2 �� ���� Ÿ������ ����)

private:
	void			__SearchTeamBandSg( void);
	void			__SearchHeadSg( void);

protected:
	AvatarLoadedCallback	m_pfGameNodeEvent = nullptr;
	AvatarLoadedCallback	m_pLoadFinishCallBack = nullptr;

	AvatarGen *		m_pGen;			// Generator [initialize at constructor]
	i3Body *		m_pBody = nullptr;		// ���� Body
	i3Body *		m_pTempBody = nullptr;	// �ε��� Body, m_pBody�� ��ü�� nullptr�� �ʱ�ȭ�Ѵ�.

	bool			m_bLoaded = false;		//���� �ε� �Ǿ����� üũ
	bool			m_bReleaseQue = false;	// ������ �༮ (�ε��� �Ϸ�Ǹ� ����)
	
	i3Transform*	m_pTrans = nullptr;		//ȸ���� �ٵ��� ������ �ִ�..
	
	GLOBAL_CHARA_INFO	m_CharaInfoForLoad;	// �ε��ϱ� ���� Chara Info // �ε��߿��� ���, �ε� �Ŀ��� CharaManager�� Info�� ����ϰԵȴ�.
	GLOBAL_CHARA_INFO *	m_pTempCharaInfo = nullptr;	// CharaManager�� Info �ε� �Ϸ� �� �̰����� ��ü

	bool			m_bInBattle = false;
	bool			m_bThreadMode = false;
	bool			m_bUseDummyResource = false;

protected:
	/** \brief Frame LOD�� ã�´�.*/
	INT32			_FindFrameLOD( i3Body * pBody);

	////////////////////////////////////////////////////////////////////////////////////////////////
	//									inline functions
protected:
	void			SetCallBack( AvatarLoadedCallback pCallBack)			{ m_pfGameNodeEvent = pCallBack; }
	void			SetLoadFinishCallback( AvatarLoadedCallback pCallBack)	{ m_pLoadFinishCallBack = pCallBack; }
	void			SetThreadMode( bool bMode)								{ m_bThreadMode = bMode; }

	void			_Destroy( void);
	
public:
	Avatar();
	virtual ~Avatar(void);

	virtual void	ResetChara( void) override;

	virtual void	AttachScene( void) override;
	virtual void	DetachScene( void) override;

	virtual void	ReturnInstance( void) override;

	void			LostDevice( void);
	void			Revive( i3RenderContext * pCtx);

	/** \brief ���̸� �ƹ�Ÿ ������ �� Res�� ��ü�Ѵ�. */
	void			ApplyLoadedRes( void);

	bool			OnUpdateAvatarGen_InThread( void);

	void			Create( GLOBAL_CHARA_INFO* pInfo);

	void			SetPartsEquipInfo( const CHARA_PARTS_INFO * pPartsInfo);

	i3::rc_string	GetPartName( EQUIP::ePART part) const;

	const i3Body *		GetBody()		{ return m_pBody;}

	void			SetCharaInfoForLoad( GLOBAL_CHARA_INFO * pInfo);

	/** \brief ����� ����
		\param[in] TeamColor ������ ���� true(������ ����), false(�������θ� ����)*/
	void			SetTeamBand( bool TeamColor = false);

	void			DisableTeamBand( void);

	// 
	void			ProcessCallback(void);
	void			LoadFinish( void);

	AvatarLoadedCallback GetCallBack( void)							{ return m_pfGameNodeEvent; }

	//���� ����
	void			SetParts( T_ItemID ItemID, bool bInLobby);
	void			SetParts( const CHARA_PARTS_INFO * pInfo, bool bInLobby, bool bForceRebuild = false);

	/** \brief ���� ����
		\note �κ�� ĳ���Ϳ����� ȣ���մϴ�. */
	void			SetWeapon( WEAPON_SLOT_TYPE slot, T_ItemID Itemid, bool bIsSameWeapon);

	//������ �ε� �Ǿ���?
	bool			GetLoadedParts() const			{ return m_bLoaded; }
	void			SetLoadedParts( bool bVal)	{ m_bLoaded = bVal; }

	void			SetEquipResource( T_ItemID itemID, INT32 res_subtype);

	/** \brief Common Res�� ��� in Battle
		\desc Battle ���Խ� �κ񿡼� ������ �ڽ��� Res�� �������� �ʵ��� �Ѵ�.*/
	void			KeepResource( void);

	/** \brief Headgear ������ ���� mask�� ���� */
	void			SetCharaHead( bool bAttachedHeadgear);

	void			Cmd_PrepareEquip( EQUIP::ePART part);

	/** \brief Dummyresource ��� ���� */
	bool			IsDummyResource( void)		{ return m_bUseDummyResource; }

	void			SetReleaseQue( void)		{ m_bReleaseQue = true; }
	bool			isReleaseQue( void)			{ return m_bReleaseQue; }

	/** \brief Skin ��� ���� */
	bool			IsSkinResource( void);

	/** \brief Eye Enable */
	virtual void	SetEyeEnable( bool enable) override;
};