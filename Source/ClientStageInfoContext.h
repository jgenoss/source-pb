#if !defined( __CLIENT_STAGE_INFO_CONTEXT_H)
#define __CLIENT_STAGE_INFO_CONTEXT_H

/*
ClientStageInfoContext

�ΰ��� �÷��� �ϴ� ȯ���� ���������� ���� �� �� ������,
���� ���� ���� ū ������ Stage ��� �����Ѵ�. ( ���� ���� ������ ������ ���Ͽ� )
�ش� ������ �����ϱ� ���� �������� ���� ���� DATA���� �������� ���(����)���� �͵鸸,
Ŭ�� �˸°� �����ϰ�(�α��ν� �������), �� �� �ش� �������� Ȱ�� �� �� �ֵ��� DATA���� �����ϴ� Ŭ�����̴�.

Stage		= rule + stage_uid
Rule		= mode + ���� ���� �÷��� Ư¡. ( �ڼ��Ѱ� commondef_stage ���� )
CSI_STAGE	= MAP_STAGE_MATCHING Ŭ���.
CSI_RULE	= MAP_RULE_DATA Ŭ���.
CSI_MAP		= map �����Ͱ� UID ���� ���� �Ǿ� ���� ( �̸��� �� )
*/

#include "CSI_TypeDef.h"

typedef i3::unordered_multimap<INT32, CSI_STAGE>									rule_stage_data_mmap;
typedef rule_stage_data_mmap::iterator												iterator_rule_stage_data_mmap;
typedef i3::pair< iterator_rule_stage_data_mmap, iterator_rule_stage_data_mmap >	iterator_rule_stage_data_pair;

class ClientStageInfoContext : public i3::shared_ginst<ClientStageInfoContext>
{
public:
	
	ClientStageInfoContext();
	~ClientStageInfoContext();

	void	CreateMapDataAtPefData();			
	void	AddRuleDataAtServerData(MAP_RULE_DATA* ruledata);
	void	AddStageListAtServerData(MAP_STAGE_MATCHING* matchingdata);
	void	StrightRuleData();

	const	iterator_rule_stage_data_pair	GetStageList(INT32 ruleidx);
	const	iterator_rule_stage_data_pair	GetStageListofEvent();
	const	iterator_rule_stage_data_pair	GetStageListofEvent(INT32 ruleidx);

	i3::vector< CSI_RULE* >&			GetRuleLists(bool custom_list = false);

	const	CSI_RULE*					GetRuleData(INT32 rule_idx);
	const	CSI_RULE*					GetRuleData(UINT32 rule);
	
	const	CSI_STAGE*					GetStageData(STAGE_ID stageid);

	const	CSI_MAP*					GetMapData(STAGE_UID stage_uid);
	const	CSI_MAP*					GetMapData(const char* map_code_name);

	const	i3::vector< CSI_STAGE >*	GetALLStageList()								{ return &m_vecStageData; }

	const	INT32						GetServerRuleMaxIdx()							{ return m_nServerRuleMaxIdx; }

	/*-- ������ ���� �ν�Ʈ �̺�Ʈ �� ǥ�� ���� --*/
	void SetUseRandomMapRenewalSystem(bool bUseRandomMapRenewalSystem) { m_bUseRandomMapRenewalSystem = bUseRandomMapRenewalSystem; }
	bool IsUseRandomMapRenewalSystem() const { return m_bUseRandomMapRenewalSystem; }
	void SetRandomMapEventData(UINT32 n32Exp, UINT32 n32Point);
	const i3::rc_wstring& GetRandomMapEventString() const { return m_RandomMapEventString; }
	bool IsRandomMapEventOpened() const  { return (!(m_nRandomMapEventExp == 0 && m_nRandomMapEventPoint == 0)); } // ������ �ν�Ʈ �̺�Ʈ���� �ƴϸ� �� ���� 0���� ���´�.

public: 

	INT32	m_nServerRuleMaxIdx;														//������ �� �� �ε��� �� ���� ū�� ���� �س��´�. (4:4���� �Ÿ��� ���Ͽ�) 										

private:

	void	MakeMaxPersonRule(	rule_stage_data_mmap&  selecte_hmmstage,
								i3::vector<CSI_RULE*>& target_v_rule, 
								CSI_RULE*			  find_rule );

	i3::vector< CSI_MAP  >		m_vecMapData;											//PEF�� ��ϵǾ� �ִ� ��� �ʵ� �ڵ����(PEFŽ����), ǥ�ó���(�����Ǵ°�) ����.

	i3::vector< CSI_RULE* >		m_vecRuleData;
	i3::vector< CSI_RULE* >		m_vecRuleDataMain;										//�������� ���� ��� ���� �꿡 ���� ����Ÿ ����.
	i3::vector< CSI_RULE* >		m_vecRuleDataCustom;									//�������� ���� ��� Ŀ���� �꿡 ���� ����Ÿ ����. ( �������� �ɼ��� �ɸ� �길 )
	
	i3::vector< CSI_STAGE>		m_vecStageData;											//�������� ���� ��� �������� ���̵� ���� ����Ÿ ����.
	rule_stage_data_mmap			m_hmmStageforRuleId;									//���� ��� RULE(����ڰ� ���� ���)�� ���� �� �̺�Ʈ ���� ��� �������� ���� ����.
	rule_stage_data_mmap			m_hmmStageofEventforRuleId;								//���� ��� RULE(����ڰ� ���� ���)�� ���� �̺�Ʈ ���� ��� �������� ���� ����.

	INT32	m_nRuleMaxIdx;																//Ŭ���� �� �ε��� �� ���� ū�� ���� �س��´�. (4:4���� ����� ���Ͽ�)

	/*-- ������ ���� �ν�Ʈ �̺�Ʈ �� ǥ�� ���� --*/
	UINT32				m_nRandomMapEventExp = 0;
	UINT32				m_nRandomMapEventPoint = 0;
	bool				m_bUseRandomMapRenewalSystem = false;
	i3::rc_wstring		m_RandomMapEventString;
};

#endif