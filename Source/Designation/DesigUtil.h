#pragma once

struct DesigSet;
class CUserDesigInfo;
struct TempUserDesigInfo;
class DesigAttr;

namespace DesigUtil
{
	bool						LoadDesignationPef( const i3::rc_string& strFileName );

	void						ClearDesigItemInfosFromServer();
	void						UpdateDesigItemInfosFromServer(INT32 begShopItemIdx, INT32 currShopItemCount );

	// �Ʒ� 2���� ����������Ŷ�� ������ �� �����Լ�����..
	const i3::vector<UINT32>*	FindItemIDsFromDesigType( UINT8 desigSetType);
	const i3::vector<UINT8>*	FindDesigTypesFromItemID( UINT32 itemID);

	// Find���� SAFE�ϰ� ó�� �ǰ�..
	const DesigSet*				FindDesigSetByType(INT32 desigSetType);
	const DesigSet*				FindDesigSetByUI( INT32 idx);

	// Get���� ���� üũ�� ���� �ʴ´�..
	const DesigSet*				GetDesigSetByType(INT32 desigSetType);
	const DesigSet*				GetDesigSetByUI( INT32 idx);

	INT32						GetDesigSetCountByUI();
	INT32						GetDesigSetCountByType();

	INT32						GetDesigAttrCount();
	const DesigAttr*			FindDesigAttr(INT32 desigID);
	
	// ���� ���� ���θ�... (ȹ�� ȣĪ���� ������)
	bool	CheckPurchasable_MyItemByDesig(UINT32 itemID, const i3::vector<UINT8>** outNeedDesigSetType = nullptr);
	bool	CheckPurchasable_ItemByDesig( UINT32 itemID, const CUserDesigInfo* userDesigInfo, const i3::vector<UINT8>** outNeedDesigSetType = nullptr);
	
	// ���� ���� ���θ� ����... ( ���� ������ ���� ȣĪ���� ������)
	bool	CheckWearable_MyItemByDesig(UINT32 itemID, const i3::vector<UINT8>** outNeedDesigSetType = nullptr);
	bool	CheckWearable_ItemByDesig( UINT32 itemID, const CUserDesigInfo* userDesigInfo, const i3::vector<UINT8>** outNeedDesigSetType = nullptr);
	bool	IsItemLimitedByDesigSlot(T_ItemID itemID);

	void	AcquireGainedDesigSetsForUI( i3::vector<bool>& outGainedForUI, size_t numUIControl);

	// ���� ���ú�� �ž� �ִ� �Լ���...
	void Update_Recv_PROTOCOL_BASE_GET_USER_INFO_ACK(UINT64 myGainedDesigFlag, 
		const UINT8 (&desigSetTypes)[MAX_EQUIP_USER_TITLE], INT32 limitCount);
	void Update_Recv_PROTOCOL_BASE_USER_TITLE_CHANGE_ACK(INT32 limitCount);
	void Update_Recv_PROTOCOL_BASE_USER_TITLE_EQUIP_ACK();
	void Update_Recv_PROTOCOL_BASE_USER_TITLE_RELEASE_ACK();
	void Update_Recv_PROTOCOL_BASE_USER_TITLE_INFO_ACK(bool IsMyInfo, UINT64 GainedDesigFlag, 
		const UINT8 (&desigSetTypes)[MAX_EQUIP_USER_TITLE], INT32 limitCount);

	// ���� ���ú� ���� EVENT ó���� �žƳִ� �Լ���...
	void Update_Recv_EVENT_DESIGNATION_GET(i3::rc_wstring& outMessage);

	// ���� ���۽� �ž� �ִ� �Լ���..
	void						Update_Send_EVENT_DESIGNATION_GET(UINT8 gaindSetType); 
	void						Update_Send_EVENT_DESIGNATION_CHANGE(UINT8 slot, UINT8 desigSetType); 
	void						Update_Send_EVENT_DESIGNATION_RELEASE(UINT8 slot, UINT8& outDesigSetType); 

	//
	TempUserDesigInfo*			GetTempUserDesigInfo();
	CUserDesigInfo*				GetMyUserDesigInfo();
	//

	void EnableDesigApplyInfo(UINT8 desigSetType, DesigApplyInfo* inoutInfo);


}
