#if !defined( __GAME_DINO_ANIM_CONTEXT_H__)
#define __GAME_DINO_ANIM_CONTEXT_H__

#include "../GameCharaAnimContext.h"

class CGameDinoAnimContext : public CGameCharaAnimContext
{
	I3_CLASS_DEFINE( CGameDinoAnimContext, CGameCharaAnimContext);

	friend class CGameCharaDino;
	friend class CGameCharaDinoTRex;
	friend class CGameCharaDinoRaptor;
	friend class CGameCharaDinoElite;
	friend class CGameCharaDinoSting;
	friend class CGameCharaDinoTank;
	friend class CGameCharaDinoAcid;
	friend class CGameCharaDinoStingMars;
	friend class CGameCharaDinoCCRaptor;
	friend class CGameCharaDinoCCSting;
	

	friend class CGameCharaActionContext;
	friend class CGameDinoActionContext;
	friend class CGameDinoBoneContext;

	friend class CGameCharaActionControl;
	friend class CGameDinoAnimControl3PV;

private:
	ID_LOWER_DINO_AI	m_nOldDinoLowerId = ID_LOWER_DINO_UNKNOWN;
	ID_UPPER_DINO_AI	m_nOldDinoUpperId = ID_UPPER_DINO_UNKNOWN;

	INT8				m_nDinoAILowerId[ CHARA_BODYLOWER_MAXCOUNT][ ID_LOWER_AI_MAX];
	INT8				m_nDinoAIUpperId[ CHARA_VIEW_MAX][ ID_UPPER_DINO_AI_MAX];

protected:

	/** \brief ���� ��ü animation�� �����Ѵ�.
		\param[in] aiID ID_UPPER_DINO_AI index
		\param[in] startTime ���� �ð�
		\param[in] scale animation scale �� */
	bool			_Play_Anim_DinoUpper( ID_UPPER_DINO_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief ��ü animation�� �����Ѵ�.
		\param[in] aiID ID_LOWER_AI index
		\param[in] startTime ���� �ð�
		\param[in] scale animation scale �� */
	bool			_Play_AnimDino_Lower( ID_LOWER_DINO_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

public:
	CGameDinoAnimContext() {}

	ID_LOWER_DINO_AI	getOldDinoLowerID( void)					{ return m_nOldDinoLowerId; }

	/** \brief ���� lower ai id�� �����մϴ�. */
	void				setOldDinoLowerID( ID_LOWER_DINO_AI id)		{ I3ASSERT( id < ID_LOWER_DINO_AI_MAX); m_nOldDinoLowerId = id; }

	/** \brief ������ ���� upper ai id�� ��ȯ�մϴ�. */
	ID_UPPER_DINO_AI	getOldDinoUpperID( void)				{ return m_nOldDinoUpperId; }

	/** \brief ������ ���� upper ai id�� �����մϴ�. */
	void				setOldDinoUpperID( ID_UPPER_DINO_AI id)			{ I3ASSERT( id < ID_UPPER_DINO_AI_MAX); m_nOldDinoUpperId = id; }

	/** \brief ������ Upper AI id�� ��ȯ�մϴ�.
		\param[in] viewType 1/3��Ī flag
		\param[in] ai ID_UPPER_DINO_AI index */
	INT32				getDinoAIUpperID( ID_UPPER_DINO_AI ai);

		/** \brief ������ Upper AI id�� �����մϴ�.
		\param[in] viewType 1/3��Ī flag
		\param[in] ai ID_UPPER_DINO_AI index
		\param[in] id id */
	void				setDinoAIUpperID( CHARA_VIEW_TYPE viewType, ID_UPPER_DINO_AI ai, INT32 id)
	{
		I3ASSERT( viewType < CHARA_VIEW_MAX);
		I3ASSERT( ai < ID_UPPER_DINO_AI_MAX);

		I3ASSERT( id < 128);
		m_nDinoAIUpperId[ viewType][ ai] = (INT8) id;
	}

	INT32				getDinoAILowerID( ID_LOWER_DINO_AI ai);

	void				setDinoAILowerID( CHARA_ACTION_BODYLOWER body,ID_LOWER_DINO_AI ai, INT32 id)
	{
		I3ASSERT( body < CHARA_BODYLOWER_MAXCOUNT);
		I3ASSERT( ai < ID_LOWER_DINO_AI_MAX);
		I3ASSERT( id < 128);

		m_nDinoAILowerId[ body][ ai] = (INT8) id;
	}

	/** \brief �ٴ� ���⿡ ���� DinoLower AI id�� ��ȯ�մϴ�. */
	ID_LOWER_DINO_AI	getMoveIdByDirForDino( VEC3D * vDir);

	/** \brief �ȴ� ���⿡ ���� Lower AI id�� ��ȯ�մϴ�. */
	ID_LOWER_DINO_AI	getWalkIdByDirForDino( VEC3D * vDir);


	/** \brief Bone List�� �ۼ��մϴ�.
		\note Bone�� ���� ó���� �� �� �ʿ���. */
	virtual void		BindBoneList() override;

};


#endif
