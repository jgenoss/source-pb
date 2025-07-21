/**
 * \file	GameCharaCameraContext.h
 *
 * Declares the game chara camera context class.
 */


#if !defined( __GAME_CHARA_CAMERA_CONTEXT_H__)
#define __GAME_CHARA_CAMERA_CONTEXT_H__

#include "GameCharaContextBase.h"

class CGameCharaCameraControl;
class CGameCharaCameraControl1PV;
class CGameCharaCameraControl3PV;

class CGameCharaCameraContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaCameraContext, CGameCharaContextBase);

	friend class CGameCharaBase;
	friend class CGameCharaCameraControl;

protected:
	CGameCharaCameraControl *		m_pCurrentCameraControl = nullptr;
	CGameCharaCameraControl1PV *	m_p1PVCameraControl = nullptr;
	CGameCharaCameraControl3PV *	m_p3PVCameraControl = nullptr;

	//1��Ī ī�޶��϶� ���� �ڼ��� ���ڼ��� ������ ���� ik���� ���� �����մϴ�.
	CHARA_STATE_FOR_CAM m_nChangeFromState = CSFC_STAND;
	CHARA_STATE_FOR_CAM m_StateForCam = CSFC_STAND;				// ī�޶� ���� �ɸ��� ����

	REAL32				m_rChangePoseTime = 0.0f;			//�ڼ� ����� �ɸ��� �ð�
	REAL32				m_rChangePoseAccTime = 0.0f;		//�ڼ� ���� �����ð�
	bool				m_bChangeToStand = false;			//���ڼ��� �������� �����ڼ��� ��������.

	REAL32				m_rIKHeight = 0.0f;				// ĳ������ IK ����

public:
	/** \brief IK�������� camera�� ���̸� ��ȯ�մϴ�. */
	REAL32				getIKHeightForCam( void);

	/** \brief IK ���̸� ��ȯ�մϴ�. */
	REAL32				getIKHeight( void)					{ return m_rIKHeight; }

	/** \brief camera�� ��ġ�� ���ɴϴ�.
		\param[in] pPos ��ȯ�Ǵ� ��ġ��
		\param[in] pMat ��ȯ�Ǵ� Matrix��
		\param[in] bDump Trace ��� ���� */
	void				getVirtualCamPosition( VEC3D * pPos, MATRIX * pMat = nullptr, bool bDump = false);

public:
	virtual ~CGameCharaCameraContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV( void) override;
	virtual void	OnChange3PV( void) override;

	virtual void	OnChangeLocal( void) override;
	virtual void	OnChangeNetwork( void) override;
	virtual void	OnChangeNPC( void) override;
	
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief �ɱ� ���⿡ ���� �������� �����մϴ�. */
	void			ProcessStandOn1PV( CHARA_STATE_FOR_CAM state, REAL32 rDeltatime);

	/** \brief �ɱ� ���⿡ ���� ó���� �մϴ�. */
	void			ProcessChangePose( REAL32 rDeltatime);

	/** \brief ���� Camera�� �����մϴ�. */
	void			SetZoom( void);

	/** \brief Scope�� ���� Level�� �����մϴ�. */
	bool			SetScopeLevel( INT32 nScopeLevel);

	////////////////////////////////////////////////////////////////////////////////////
	// internal interface
protected:
	/** \brief �⺻ Camera�� �����մϴ�. (FPS Camera) */
	void			_Idle_Camera();

	/** \brief Kill Camera�� �����Ѵ�.
		\param[in] killerIdx ���� ���� index	*/
	void			_Kill_Camera( INT32 killerIdx);

	/** \brief Observer mode���� Zoom�� �����մϴ�
		\note ���⿡ ���� �ڵ����� ���� */
	void			_Observer_Zoom( void);

	/** \brief Respawn�� Camera�� �����մϴ�.
		\note FPS camera */
	void			_Respawn_Camera();
};

#endif
