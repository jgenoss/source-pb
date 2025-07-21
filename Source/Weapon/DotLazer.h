#if !defined( __DOT_LAZER_H)
#define __DOT_LAZER_H

class CDotLazer : public i3ElementBase
{
	I3_CLASS_DEFINE(CDotLazer, i3ElementBase);

private:
	// �ѿ� �پ��ִ� ����Ʈ��
	i3AttrSet *				m_pAttrSetDot;	
	i3GeometryAttr *		m_pGeoAttr;
	i3VertexArray *			m_pVADot;
	i3Billboard *			m_pBillboardGlow;
	i3TextureBindAttr	*	m_pTargetTextureAttr;

	// ����Ʈ ���̿��� �ٴ� ����Ʈ
	i3TimeStamp*			m_pDotStamp;

	VEC3D					m_StartVector;
	VEC3D					m_EndVector;

	VEC3D					m_vecDotPos;

	VEC3D					m_pos[12];
	VEC2D					m_tex[12];
	REAL32					m_fThickness;
	REAL32					m_fDistance;

public:
	CDotLazer();

	void					CreateVertex(void);
	void					UpdateVertex(void);
	void					CreateLazerDot(void);
	void					DotStampDisable();
	void					DotStampEnable();
	void					UpdateDotStamp();
	void					CreateLazerSight(i3SceneObject* pSceneObj);
	void					GetShotTargetPos(VEC3D * pDest, VEC3D * pStart, bool bReloaded, REAL32 rRange, CGameCharaBase* pOwner, MATRIX* pFX_Mat);
	void					SetEnableDotLaser( bool bFlag);
	void					OnLostDevice( bool bLostDevice);
	void					OnRevive( i3RenderContext * pCtx);
	void					ReturnInstance( void);
	void					SetEnableLaserBar( bool bFlag);
};

#endif // __DOT_LAZER_H