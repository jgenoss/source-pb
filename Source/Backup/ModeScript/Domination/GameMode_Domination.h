#pragma once

#ifdef	DOMI_CORE

#include "StageBattle.h"
#include "../../GameMode.h"

#include "Domination_Defines.h"
#include "Domination_FlowBase.h"
#include "Domination_WaveMission.h"
#include "../../Camera/AnimatedCamera/AniCamRegisterByMode.h"


enum TAKEBACK_AREA
{
	TAKEBACK_AREA_0		= 0,
	TAKEBACK_AREA_1,
	TAKEBACK_AREA_2,
	TAKEBACK_AREA_3,

	MAX_TAKEBACK_AREA_COUNT,
	TAKEBACK_AREA_UNKNOWN,
};


struct LayerObjects
{
	std::string				layerName;
	std::vector<i3Object*>	layerObjects;

	void Clear()
	{
		layerName.clear();
		layerObjects.clear();
	}
};



namespace Domination_Util
{
	class CNaviMesh
	{
		CDomiStage*			m_pMesh;
		CUSTOM_VERTEX*		m_pVertex;
		INT32				m_nVertexCount;
	public:
		CNaviMesh( const STAGE_ID id);
		~CNaviMesh();
		INT16 GetNaviMeshIdx( VEC3D& v);
		INT32 GetVertexCount() const { return m_nVertexCount;}
		CUSTOM_VERTEX* GetVertexList() const { return m_pVertex;}
	};

	void RenderNaviMesh(i3Viewer* pViewer, void* pData);
};
namespace du = Domination_Util;

class UIHUDDominationMgr;
//class IModeScript;
class i3Object;

class gmode_domination : public gmode
{
private:
	du::CNaviMesh*				m_pNaviMesh;

	UIHUDDominationMgr*			m_pHud;
	
#ifdef DOMI_ANIM_CAM
	// Animation Camera ���п� ��� ����
	AnimCamRegisterMode*		m_animCamRegisterDomi;
#endif

	// �� �÷��� �ð�
	static REAL32				s_totalPlayTime[SLOT_MAX_COUNT];

	// ���� ���� �� Domi_BGM_Boss_oneplay.ogg ���� ��� ����. 
	// �� ���� �� �� ��� �Ŀ� Domi_BGM_Boss.ogg�� ���� ����ȴ�.  
	BOOL						m_firstBossBgmPlayed;


private:
	void	_ProcessEventDeath( INT32 Arg);

public:
	virtual void ProcessCreateStage( CStageBattle* pStage);

	virtual	void ProcessEventPreStartRound(void);
	virtual void ProcessEventRoundStart(void);
	virtual void ProcessEventRoundEnd(void);

	virtual void ProcessEventDefault( INT32 nEvent, INT32 Arg);

	virtual void ProcessUpdate( RT_REAL32 rDeltaSec);		// ��ü ������Ʈ
	virtual void OnUpdateEndGame( REAL32 rDeltaSec);		// ����������

	virtual void InitialIzeStage(void);
	virtual void InitLayer(void);
	virtual void ResetObjectStatus(void) {}

#ifdef DOMI_ANIM_CAM
	void    ProcessEventAnimationCamera(INT32 eventIdx);
#endif

	void	UpdateTotalPlayTime(REAL32 deltaSeconds);
	REAL32	GetTotalPlayTime(INT32 i)	{ return s_totalPlayTime[i]; }

	i3::vector<i3RespawnObj*>&	GetRespawnObjectList() { return m_pStage->getRespawnObjList();}
	du::CNaviMesh*				GetNaviMesh() const { return m_pNaviMesh;}
	
	gmode_domination();
	virtual ~gmode_domination();
};

#endif