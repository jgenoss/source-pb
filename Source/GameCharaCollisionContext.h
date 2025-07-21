/**
 * \file	GameCharaCollisionContext.h
 *
 * Declares the game chara collision context class.
 */


#if !defined( __GAME_CHARA_COLLISION_CONTEXT_H__)
#define __GAME_CHARA_COLLISION_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaDef.h"

enum CHARA_CONTACT_BONE
{
	CHARA_CONTACT_BONE_ROOT = 0,
	CHARA_CONTACT_BONE_SPINE3,
	CHARA_CONTACT_BONE_LARM,
	CHARA_CONTACT_BONE_RARM,
	CHARA_CONTACT_BONE_LFOOT,
	CHARA_CONTACT_BONE_RFOOT,

	CHARA_CONTACT_BONE_COUNT
} ;

class CGameCharaCollisionContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaCollisionContext, CGameCharaContextBase);
protected:
	REAL32				m_rColRadius = 0.84f;				//ĳ���� ���� �浹üũ�� ���� ���� ������
	i3Transform2 *		m_pRoughHitTrans = nullptr;			// ������Ʈ �ø��� ����(Sphere)
	INT32				m_DropCount = 0;				// ���� ����Ʈ�� ī��Ʈ

	REAL32				m_rContactTimer = 0.0f;

//	List *			m_pContactShapeSetList;		// �ù����̼� �� �浹 �� ����Ʈ ó���� ..������ �ʾƼ� ���� ����� ��� �ּ�ó����(2012.05.30.����)
	bool				m_bContactBones[CHARA_CONTACT_BONE_COUNT] = { false, };		// 
	VEC3D				m_vBonesForceDir[ CHARA_CONTACT_BONE_COUNT ];

	i3::vector<I3_PHYSIX_CONTACT_REPORT_PTR> m_vecContactReport;

	//	Crosshair info
	VAR_LR(I3_PHYSIX_HIT_RESULT*, 33)	m_CrossHitResult = new I3_PHYSIX_HIT_RESULT();	// ũ�ν���� ��Ʈ ����
	VAR_LR(REAL32, 34)					m_rCrossHitTimer = 0.0f;	// ũ�ν���� üũ Ÿ��
	VAR_LR(CGameCharaBase*, 35)		m_pCrossHitChara = nullptr;	// ũ�ν��� �ø����� ĳ����, NULL�̸� ����.
	VAR_LR(REAL32, 36)					m_CrossHitDist = 0.0f;		// ũ�ν��� �ø����� �Ÿ�.
	i3CollideeLine						m_CrossHitLine;		// ũ�ν���� ��Ʈ����
	
	// Floor hit collision
	VAR_LR(I3_PHYSIX_HIT_RESULT*, 37)	m_FloorHitResult = new I3_PHYSIX_HIT_RESULT();	// �ٴڿ� ���� �ø��� ����

	UINT16				m_nCollisionGroup = 0;

protected:
	void			AttachRoughHitCollision( void);
	void			DetachRoughHitCollision( void);

	/** \brief Shape�� ���� ������ ��ȯ�մϴ�.
		\note �����		*/
	CHARA_HIT_PART	_GetHitHuman( i3PhysixShapeSet* pDestShapeSet);

	/** \brief Shape�� ���� ������ ��ȯ�մϴ�.
		\note Raptor */
	CHARA_HIT_PART	_GetHitPartRaptor( i3PhysixShapeSet* pDestShapeSet);

	/** \brief Shape�� ���� ������ ��ȯ�մϴ�.
		\note Rex */
	CHARA_HIT_PART	_GetHitPartRex( i3PhysixShapeSet * pDestShapeSet);

	/** \brief Mission�� ��ź�� �ٶ󺸰� �ִ��� Ȯ���մϴ�. */
	void			_CheckCrossHitMissionBomb( void);

	void			_CollisionHitMission( WeaponBase * pWeapon);



public:
	/** \brief collision group �� ��ȯ�մϴ�. */
	UINT16				getCollisionGroup( void)				{ return m_nCollisionGroup; }

	/** \brief collision�� ���������� ��ȯ�մϴ�. */
	REAL32				getCollisionRadius( void)				{ return m_rColRadius; }

	/** \brief collision�� ���������� �����մϴ�. */
	void				setCollisionRadius( REAL32 val)			{ m_rColRadius = val; }

	/** \brief ����� �浹�� ���� shape hit count�� ��ȯ�մϴ�. */
	INT32				getDropHitCount( void)					{ return m_DropCount; }
	void				setDropHitCount( INT32 val)				{ m_DropCount = val; }

	/** \brief ĳ���͸� �ѷ��� Sphere shape�� ��ȯ�մϴ�. */
	i3Transform2 *		getRoughBound( void )					{ return m_pRoughHitTrans; }

	/** \brief �浹�� bone���� ���� ������ ��ȯ�մϴ�. */
	VEC3D *				getBonesForceDir( INT32 idx)			{ I3_BOUNDCHK( idx, CHARA_CONTACT_BONE_COUNT); return &m_vBonesForceDir[ idx]; }

	/** \brief �浹 shape�� ���� list�� ��ȯ�մϴ�. */
	// List *			getContactShapeSetList( void)			{ return m_pContactShapeSetList; }

	/** \brief Crosshair�� hit line�� ��ȯ�մϴ�. */
	i3CollideeLine *	getCrossHitLine(void)					{ return &m_CrossHitLine;		}

	/** \brief Crosshair�� hit �Ÿ��� ��ȯ�մϴ�.
		\note ���⿡ ���� ����˴ϴ�. */
	REAL32				getCrossHitDistance(void)				{ return m_CrossHitDist; }

	/** \brief �ٴڸ鿡 ���� raycast ����� ��ȯ�մϴ�. */
	I3_PHYSIX_HIT_RESULT * getFloorHitResult( void)				{ return m_FloorHitResult; }

	/** \brief �ٴڸ鿡 ���� raycast ����� �����մϴ�. */
	void				setFloorHitResult( I3_PHYSIX_HIT_RESULT * pResult )	{ I3ASSERT( pResult != nullptr); i3Physix::HitResultCopyTo( m_FloorHitResult, pResult); }

public:
	CGameCharaCollisionContext();
	virtual ~CGameCharaCollisionContext();

	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	/** \brief ĳ������ �ø��� �׷��� �����մϴ�. */
	void			SetCollisionGroup( UINT16 group, UINT32 mask);

	/** \brief Capsule controller�� �����ϰ� Rough shape�� �����ؼ� ĳ���Ϳ� ���Դϴ�.
		\param[in] pParentNode �ٿ��� scene node
		\param[in] nColGroupIndex collision group index */
	void			AttachCollisionNode( i3Node * pParentNode, UINT32 nColGroupIndex);

	/** \brief Capsule controller�� �����ϰ� Rough shape�� �����Ѵ�. */
	void			DetachCollisionNode( void);

	/** \brief ���˿� Shape Ȱ��ȭ ����Դϴ�. 
		\note ����� ������� ����. */
	void			SetContactShapeSetEnable( bool bVal);
	void			QueContactReport( const I3_PHYSIX_CONTACT_REPORT_PTR& spReport );
	void			ParseContactReport( void);
	void			ProcessCollisionEffect( void);
	void			ContactShapeSetUpdate( void);

	/** \brief Shape�� ���� ������ ��ȯ�մϴ�. */
	CHARA_HIT_PART	GetHitPart( i3PhysixShapeSet* pDestShapeSet);

	/** \brief ĳ���� Capsule�� ���� Overlap���θ� �Ǻ��մϴ�. */
	bool			IsOverlappedBy( UINT32 nGroupIdx);

	/** \brief CrossHair�� ���� Nick ǥ�ø� Update�մϴ�.
		\note �ʴ� 20ȸ raycast�� �����մϴ�.		*/
	bool			UpdateNickOnTarget( void);

	
};

#endif
