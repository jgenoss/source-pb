/**
 * \file	UI\UIUtil.h
 *
 * Declares the user interface utility class.
 */


#if !defined( __UI_UTIL_H__)
#define __UI_UTIL_H__

#include "UIDefine.h"
#include "UIBattleDef.h"
#include "UIMessageBox.h"
#include "UILoadingIcon.h"
#include "CSI_Stage.h"

class CUserLocationInfo;
class UIFloatingBase;
class UIPopupBase;
class UIBattlePopupBase;
class UIPhaseBase;
class UIPhaseSubBase;

/** Message Box Type */
enum MSG_TYPE
{
	MSG_TYPE_ERROR,				// OK
	MSG_TYPE_SYSTEM_OK,			// OK
	MSG_TYPE_SYSTEM_QUERY,		// OK, Cancel
	MSG_TYPE_SYSTEM_WAIT,		// NONE
	MSG_TYPE_GAME_OK,			// OK
	MSG_TYPE_GAME_QUERY,		// OK, Cancel
	MSG_TYPE_GAME_WAIT,			// none
	MSG_TYPE_GAME_USER,			// User, OK, Cancel
	MSG_TYPE_WEB,				// Web
	MSG_TYPE_BLOCK_NOTICE,		// BlockUser Popup
	MSG_TYPE_INBATTLE,
	MSG_TYPE_MAX,
};

typedef i3::vector< std::string > EnableStageString;

namespace GameUI {
	UIFloatingBase *	GetFloating( UIFLOATWIN type);
	UIPopupBase *		GetPopup( UIPOPUPWIN type);
	UIBattlePopupBase *	GetBattlePopup( UIBATTLE_POPUP type);
	UIPhaseBase *		GetPhase( UIPHASE type);
	UIPhaseSubBase *	GetSubPhase( UISUBPHASE type);

	/** \brief �ش� �˾��� ���� ������ �ݴ´� */
	void		ClosePopup( UIPOPUPWIN type);

	void		ExitGamePopup(const i3::rc_wstring& exit_reason);

	// ���δ� ������Ÿ���� �����Ǵ°��� ���� �̰��� �ۼ��ص� - �̵���
	INT32		GetCharaImageIndex( T_ItemID itemID );

	//----------------------------------------------------------------------------------------------//
	//									about Items information										//
	//----------------------------------------------------------------------------------------------//
	/** \brief ������ StringValue�� ����´�.
		\param[out] pOutName	���� �̸�
		\param[out] pOutDesc	���� ����
		\param[in] type WEAPON_CLASS_TYPE
		\param[in] num ���� ��ȣ */
	void		GetWeaponString( i3::rc_wstring& outName, i3::rc_wstring* pOutDesc, i3::rc_wstring* pOutCategory, WEAPON_CLASS_TYPE type, WEAPON::RES_ID num);

	/** \brief ������ StringValue�� ����´�.
		\param[out] pOutName	���� �̸�
		\param[out] pOutDesc	���� ����
		\param[in] ItemID Goods ItemID */
	void		GetWeaponString( i3::rc_wstring& outName, i3::rc_wstring* pOutDesc, i3::rc_wstring* pOutCategory, T_ItemID ItemID);

	/** \brief ������ Shape Index�� ��ȯ�մϴ�.
		\param[in] ItemID Goods ItemID
		\return Shape Index */
	INT32		GetWeaponImageIndex( T_ItemID ItemID);

	/** \brief ���������� 0~1�� ������ ȯ���մϴ�.
		\param[in] type WEAPON_CLASS_TYPE
		\param[in] num Weapon Number
		\param[out] Damage ���ݷ�
		\param[out] Rapid �����
		\param[out] Accuracy ���߷�
		\param[out] Recoil_V �����ݵ�
		\param[out] Recoil_H ����ݵ�
		\param[out] Speed �̵��ӵ�
		\param[out] Range �����Ÿ�
		\param[out] BulletCount źȯ�� */
	void		GetNormalizedWeaponInfo( T_ItemID ItemID, 
		INT32& Damage, INT32& Rapid, INT32& Accuracy, INT32& Recoil_V, 
		INT32& Recoil_H, INT32& Speed, i3::stack_wstring& out_wstrRange, i3::stack_wstring& out_wstrBulletCount);

	void		GetGaugeString(INT32 gauge, i3::stack_wstring& out_wstr );

	/** \brief ĳ���� ���ҽ��� �̸��� ������ ���´�.
		\param[out] pOutName �̸� size MAX_STRINGC_COUNT
		\param[out] pOutDesc ���� size 2048
		\param[in] charaID	Item ID		*/
	void		GetCharacterString( i3::rc_wstring& outName, i3::rc_wstring& outDesc, T_ItemID ItemID);

	/** \brief ItemID�� CashItem�� Image���� �޾ƿ´�.
		\param[out] outUsageType ITEM_TYPE
		\param[in] ItemID Item ID
		\return Image Index	*/
	INT32		GetItemImageIndex( INT32 &outUsageType, INT32& outTexIndex, T_ItemID ItemID);

	/** \brief �������� �̸��� ������ ���´�.
		\param[out] pOutName �̸� size MAX_STRING_COUNT
		\param[out] pOutDesc ���� size 2048
		\param[in] ItemID Item ID */
	void		GetItemString( i3::rc_wstring& outName, i3::rc_wstring& outDesc, T_ItemID ItemID);

	/** \brief �������� ���� �Ⱓ �Ǵ� ������ ���´�.
		\param[out] pOutTerm
		\param[in] IsCountItem Count�� ������ ����
		\param[in] Type
		\param[in] Arg */
	void		GetItemTermString( i3::rc_wstring& pOut, const size_t sizeOutMax, bool IsCountItem, UINT8 Type, UINT32 Arg );

	///
	///  Usage���� �����͸� ����.. ������ ���߿� ó���ϰ�, �켱 ���� ��縸 �����ؼ� ó��..������ Usage�� ������ ������ ��� ��ģ��.
	///  �������� �������� �Ӽ��̸�, �� �κ��� pef�� �ݿ��Ǿ� ���� �ʴ� ���� ������� ó���ϹǷ� �۾��� ���߿� �߰��ϱ�� �Ѵ�...
	///  �ణ�� ���ܴ� ȣĪ�� ���� �Ӽ��߰��ε�..�̺κ��� �ϴ� ������� �ʵ��� �ߴ�.....
	///
	void		GetRatioEquippedItemsByEquipInfo( CEquipInfo* pEquipInfo, float& outExp, float& outPoint,
		float& outPenetrate, float& outDamage, float& outDefense, float& outSpeed, float& outAccuracy);
	
	/** \brief ĳ����â�� ��� ������ �����Ӽ��� �ۼ�Ƽ�� ������ ȹ��.. ĳ�����۰� ������ ��θ� ���� (���� �ҿ��� ���������Դϴ�..)...*/
	void		GetRatioEquippedItems( bool is_red, float& outExp, float& outPoint, float& outPenetrate, float& outDamage, 
		float& outDefense, float& outHelmetDefense, float& outSpeed, float& outAccuracy);

	//----------------------------------------------------------------------------------------------//
	//									about MissionCard information								//
	//----------------------------------------------------------------------------------------------//
	/** \brief Ribbon Shape Index�� ��ȯ�մϴ�. */
	INT32		GetRibbonShape( INT32 ribbonID);
	INT32		GetEnsignShape( INT32 ensignID);
	INT32		GetMedalShape( INT32 medalID);
			
	//----------------------------------------------------------------------------------------------//
	//									about User information										//
	//----------------------------------------------------------------------------------------------//

	/** \brief ���� UID�� ã�´�. */
	INT64		FindUserUidByNick( const i3::wliteral_range& wNickRng);

	/** \brief NickName Color�� ��ȯ�Ѵ�. */
	const I3COLOR *	GetNickNameColor( UINT8 colorIdx);

	/** \brief NickName Color�� ���� Table Index�� ��ȯ�Ѵ�. */
	UINT8		GetNickNameColorIndexByColor( I3COLOR * pColor);

	/** \brief User�� ��ġ ���� class�� �����մϴ�. */
	bool		GetUserLocationInfo( CUserLocationInfo & Out, USERLIST_TAB tab, INT32 idx);

	/** \brief Friend ���¿� ���� Text ������ ��ȯ�մϴ�. */
	void		GetTextFriendState( i3::wstring& out_wstr, UINT32 State );

	/** \brief Clan ���¿� ���� Text�� ��ȯ�մϴ�. */
	void		GetTextClanState( i3::wstring& out_wstr, UINT64 State);

	/** \brief ã�ư���
		\param[in] tab USERLIST_TAB
		\param[in] idx Community Selected Index */
	void		FindAwayUser( USERLIST_TAB tab, INT32 idx);
	void		FindAwayFriend( INT32 idx);
	void		FindAwayClan( INT32 idx);
	void		FindAwayNormalUser( void);

	/** \brief ģ�� �߰� */
	void		AddFriend( const i3::rc_wstring& wstrNick);
	/** \brief ģ�� ���� */
	void		RemoveFriend( INT32 idx);
	/** \brief ģ�� ���� */
	void		AcceptFriend( INT32 idx);
	/** \brief ģ�� ���� */
	void		RefuseFriend( INT32 idx, const CMsgBoxCallBack & cbfunc, void * pThis);

	/** \brief ģ�� ���� Ȯ�� */
	bool		IsCommunityFriend( const i3::wliteral_range& wNickRng);
	bool		IsCommunityFriend( INT64 uid);

	/** \brief ���� �ʴ�
		\desc Room���� �ʴ븦 �ؾ��Ѵ�. */
	void		InviteUser( USERLIST_TAB tab, INT32 idx);
	void		InviteFriend( USERLIST_TAB tab, INT32 idx);
	void		InviteClanMember( INT32 idx);

	/** \brief Ŭ�� �ʴ� */
	void		InviteClan( const i3::wliteral_range& wstrNick, const CMsgBoxCallBack & cbfunc, void * pThis);

	/** \brief ��� �߰� */
	void		AddBlockList( const i3::rc_wstring& wstrNick, const CMsgBoxCallBack & cbfunc, void * pThis);

	//----------------------------------------------------------------------------------------------//
	//										about Clan information									//
	//----------------------------------------------------------------------------------------------//
	/** \brief Clan Texture�� �����մϴ�.
		\param[in,out] pImgBox i3UIImageBoxEx class
		\param[in] size CLAN_MARK_SIZE
		\param[in] nClanMarkFlag */
	void		SetClanTexture( i3UIImageBoxEx * pImgBox, CLAN_MARK_SIZE size, UINT32 nClanMarkFlag);

	/** \brief Clan Texture�� �����մϴ�.
		\desc pControl�� �������� ��� ������ ���ݴϴ�.
		\param[in,out] pImgBox i3UIImageBoxEx class
		\param[in] pControl i3UIControl class
		\param[in] size CLAN_MARK_SIZE
		\param[in] nClanMarkFlag 
		\Param[in] rShapeSize*/
	void		SetClanTextureAndCenterPos( i3UIImageBoxEx * pImgBox, i3UIControl * pControl, CLAN_MARK_SIZE size, UINT32 nClanMarkFlag, REAL32 rShapeSize);

	/** \brief ListView Box�� ������ Clan Mark�� �����մϴ�.
		\desc ClanMark�� UINT32�� �Ǿ� ������ Lua���� ���� �̻��ϰ� �Ǿ� ���ս����� ó���մϴ�. */
	void		SetClanTextureForListView( i3UIListView_Cell * pCell, void * pData);

	/** \brief ListView Box�� ���� pData ��(Ŭ����ũ ���ս�)���� Clan Mark Flag�� ��ȯ�մϴ�.
	\desc Clan Mark Flag�� ��ȯ�մϴ�. */
	UINT32		GetClanMarkFlag(const char * pszData);

	/** \brief Clan Mark�� LuaStack�� �ֽ��ϴ�.
		\desc UINT32�� �������� �ʴ� Lua.. 1byte�� Ǯ� �Ѱ��ְ� �ڵ忡�� ���� �ÿ��� �����ϴ� ������� �մϴ�.
			�� 5���� Stack�� Push�մϴ�. */
	INT32		SetClanMarkToLua( LuaState * L, UINT32 mark);

	//----------------------------------------------------------------------------------------------//
	//										about stage information									//
	//----------------------------------------------------------------------------------------------//
	/** \brief Stage Map Name�� ��´�.
		\param[in] useRandomMap RandomStage ����
		\param[in] stageID STAGE_ID
		\param[out] strMapName	*/
	void		GetStageMapName(bool useRandomMap, STAGE_ID stageID,  i3::rc_wstring& out_wstrMapName);

	/** \brief StageID�� Stage Mask���� UI_MODE_TYPE���� ��ȯ�Ѵ�. */
	UI_MODE_TYPE  GetStageModeType(CSI_STAGE* stage, bool isClanMatch);

	/** \brief ROOM_INFO_BASIC ���� Ȱ���Ͽ� �������� �� ���������� �����Ͽ� ��ȯ�Ѵ�. */
	INT32 GetModeOptionShapeIndex(ROOM_INFO_BASIC & roomInfo);

	INT32 GetMapImageShapeIndex( CSI_STAGE* stageType);
	//----------------------------------------------------------------------------------------------//
	//											Message Box											//
	//----------------------------------------------------------------------------------------------//
	/** \brief Message Box�� ���ϴ�.
		\desc UI�� Input�� Disable��ŵ�ϴ�. */
	UIMessageBox *	MsgBox( MSG_TYPE type, const i3::rc_wstring& wstr_message, const i3::rc_wstring* pwstr_caption = nullptr, const CMsgBoxCallBack & cbfunc = nullptr, void * pThis = nullptr, INT32 fontSize = 10);
	UIMessageBox * WebBox(const i3::rc_wstring& wstr_message, SIZE32 Size, const i3::rc_wstring& URL);

	/** \brief Message Box�� �ݽ��ϴ�. */
	void			CloseMsgBox( UIMessageBox * pMsgBox);
	void			CloseAllMsgBox();
	void			CloseGameMsgBox();

	//----------------------------------------------------------------------------------------------//
	//											Chat Message 										//
	//----------------------------------------------------------------------------------------------//
	void		PrintSystemChat( const i3::rc_wstring& str);
	void		ProcessInputDisableChangePhase(INT32 eventID);


	//=---------------------------------------------------------------------------------------------//
	//											Error Handling										//
	//----------------------------------------------------------------------------------------------//
	void		ErrorHandleStage( INT32 GameEvent,INT32 arg);

	//Esoprt ��ũ ���ø� ��ȣ�� �˷��ش�.
	INT32 GetEsportsTemplateIndex(ESPORT_ID_LEVEL EsId);


	//=---------------------------------------------------------------------------------------------//
	//											Option Function										//
	//----------------------------------------------------------------------------------------------//
	i3::rc_wstring	GetKeymenuString( GAME_KEY_MAP key_map);
	i3::rc_wstring	GetKeyboardString( GAME_KEY_STRUCT * pGameKey);
	i3::rc_wstring	GetKeymapString( GAME_KEY_MAP key_map);

	//----------------------------------------------------------------------------------------------//
	//										Font & Text Function									//
	//----------------------------------------------------------------------------------------------//
	/* \brief pStr���� Color ��ɾ �����Ѵ�.	*/
	void		RemoveColorKeyword( const char * pStr, char * OutStr, INT32 OutStrSize);
	void		RemoveColorKeyword( std::string & str);
	void		RemoveColorKeyword( i3::string& str);
	void		RemoveColorKeyword( i3::rc_wstring& wstr);
	void		RemoveColorKeyword( i3::wstring& wstr);
	void		RemoveColorKeyword( i3::stack_wstring& wstr);

	void		RemoveColorKeywordCopy(const i3::string& in_str, i3::string& out_str);
	//----------------------------------------------------------------------------------------------//
	//										Input Device Function	(keyboard, mouse)				//
	//----------------------------------------------------------------------------------------------//
	bool IsKeyboardPressed(I3I_KEY key);
	bool IsKeyboardStroked(I3I_KEY key);
	I3I_KEY GetKeyboardPressed();
	I3I_KEY GetKeyboardStroke();

	void ResetMousePressedUp(); //IsMousePressedUp �� �Լ����� ������ ���·� ����
	bool IsMousePressedUp(UINT32 mouse); //���� ���¿��� �������� ���θ� �˷��ش�.
	bool IsMousePressed(UINT32 mouse); //��� ���� ���� ���θ� �˷��ش�.
	bool IsMouseStroked(UINT32 mouse); //�ѹ� ���� ���� ���θ� �˷��ش�.
	int GetMouseStroke();

	POINT GetScreenMousePosition(); //Screen ��ǥ�� ���콺 ��ġ ��ȯ (�׻� ��,�� ��ǥ�� 0,0 �̴�.  )
	POINT GetUIMousePosition(); //UI ��ǥ�� ���콺 ��ġ ��ȯ( ��,�� ��ǥ�� 0,0 �� �ƴ� �� �ִ�.)

	void SetMousePosition(int x, int y);
	void SetMouseCenterPosition();

	//----------------------------------------------------------------------------------------------//
	//										Loading image											//
	//----------------------------------------------------------------------------------------------//
	/** \brief Loading Icon�� Ȱ��ȭ�Ѵ�.
		\param[in] pWnd Loading Icon�� ����� Wnd
		\return UILoadingIcon */
	UILoadingIcon *	EnableLoadingIcon( UIBase * pBase, i3UIControl * pCtrl, bool bVisibleChild = true);

	/** \brief Loading Icon�� ��Ȱ���Ѵ�.
		\param[in] pIcon UILoadingIcon */
	void			DisableLoadingIcon( UILoadingIcon * pIcon);

	/** \brief ����� Loading Icon�� �����Ѵ�*/
	void			ReleaseLoadingIcon( void);

	//�ش� ��޿� ���� �ִ� ����ġ�� ��ȯ�Ѵ�.
	UINT32 GetMaxExp(UINT32 rank); //(�强��&������ �ִ� ����ġ�� ���ʿ��ϴ�.)
	UINT32 GetExpRate(UINT32 rank, INT32 curExp); // ��ũ�� �´� Rate�� ���Ѵ�.




	/*********************************************/
	//AutoPosition - �̰� ��� �� �ص� ���� ���� �Ŵ�.
	/*	AutoPosition �ػ󵵿� ���� ����Ǵ� ��ġ�� �˷��ش�.
	 *	
	*/
	enum AutoPosDir
	{
		APD_NONE = -1,
		APD_CENTER = 0,	APD_LEFT = 0x1,	APD_RIGHT = 0x10,	APD_TOP = 0x100,		APD_BOTTOM = 0x1000,
	};
	struct AutoPosition
	{
		INT32 x,y,w,h;
		UINT32 aps;

		AutoPosition() {	x = y = w = h = 0; aps = (UINT32)APD_NONE;}

		void set_dir(AutoPosDir dir1, AutoPosDir dir2 = APD_NONE);

		void set(REAL32 x, REAL32 y, REAL32 w, REAL32 h) { set_pos(x, y); set_size(w, h); }
		void set(INT32 x, INT32 y, INT32 w, INT32 h) { set_pos(x, y); set_size(w, h); }
		void set(const VEC3D & p, const VEC2D & s) { set_pos(p); set_size(s); }
		void set(VEC3D * p, VEC2D * s) {set_pos(p); set_size(s);}

		void set_pos(REAL32 x, REAL32 y) { set_pos((INT32)x, (INT32)y);}
		void set_pos(INT32 x, INT32 y) { this->x = x; this->y = y; }
		void set_pos(const VEC3D & p) {set_pos(p.x, p.y);}
		void set_pos(VEC3D * p) {set_pos(p->x, p->y);}

		void set_size(REAL32 w, REAL32 h) { set_size((INT32)x, (INT32)y);}
		void set_size(INT32 w, INT32 h) { this->w = w; this->h = h;}
		void set_size(const VEC2D & s) { set_size(s.x, s.y);}
		void set_size(VEC2D * s) { set_size(s->x, s->y);}

		VEC3D get_pos();
		operator VEC3D() {return get_pos();}

		static void UpdateRevive();
		static VEC3D GetUpdatedPos(INT32 x, INT32 y, INT32 w, INT32 h, AutoPosDir dir1, AutoPosDir dir2 = APD_NONE);

		//�ִ� ��ġ�� �˷��ش�.
		static INT32 get_max_left();
		static INT32 get_max_right();
		static INT32 get_max_top();
		static INT32 get_max_bottom();
	};

	enum AutoOffsetDir
	{
		AOD_DECREASE = 0, AOD_MIDDLE, AOD_INCREASE,
	};
	struct AutoOffset
	{
		AutoOffsetDir dir;
		INT32 x, y,offset_x,offset_y;

		static INT32 get_auto_offset_position(AutoOffsetDir dir, INT32 start, UINT32 offset, UINT32 idx, UINT32 max_idx = 0);

		void init(AutoOffsetDir dir, INT32 x, INT32 y, UINT32 offset_x, UINT32 offset_y) { 
			this->dir = dir;	this->x = x; this->y = y;	this->offset_x = offset_x; this->offset_x = offset_x; }

		VEC2D get_horizon_position(UINT32 idx, UINT32 max_idx = 0);
		VEC2D get_vertical_position(UINT32 idx, UINT32 max_idx = 0);
	};


	//----------------------------------------------------------------------------------------------//
	//										Pre macro Util											//
	//----------------------------------------------------------------------------------------------//
	i3::string pre_macro_date_to_string();
	i3::string pre_macro_time_to_string();

};

namespace domi_util
{
	void to_string_reward_grade_room(i3::wstring&  ret, INT16& curr_grade);
	void to_string_reward_grade_room_inbattle(i3::wstring& ret, INT16& curr_grade);
	void to_string_reward_grade_start_battle(i3::wstring& ret);
	void to_string_reward_grade_in_user(i3::wstring& ret, const i3::rc_wstring& user_name);
	void to_string_reward_grade_out_user(i3::wstring& ret, const i3::rc_wstring& user_name);

	INT32 reward_item_image_Idx();
	const wchar_t* reward_item_image_name();
	const wchar_t* round_grade_result();
}

namespace GameUI 
{
	bool is_over_hero_rank(INT32 nRank); //���� �̻� ����̸� true (RANK_NEWBIE, RANK_GAMEMASTER)
	bool is_hero_rank(INT32 nRank, bool include_over_rank); //���� ����̸� true, include_over_rank : true �� ��� �� �̻� ��� ����
	bool is_general_rank(INT32 nRank, bool include_over_rank); //�强��&���� ����̸� true, include_over_rank : true �� ��� �� �̻� ��� ����
}

namespace GameUI
{
	void	 CheckWeaponEndurance(void); //���� �������� �˻��ϰ� ä��â�� ǥ���Ѵ�.
}

namespace GameUI
{
	struct NationMarkTemplate
	{
		i3UIButtonImageSet* pNation;
		i3UIButtonImageSet* pEffect;

		void Reset()
		{
			pNation = nullptr;
			pNation = nullptr;
		}
	};

	struct UserUI_NationMarkData
	{
		bool			isVisible;
		REAL32			rTimer;
		REAL32			rAlphaValue;
		REAL32			rElapsedTimeByEffect;
		UINT8			ui8EffectIdx;

		i3::vector<UINT8>	vShapeIdxList;

		void _Reset()
		{
			isVisible = false;
			rTimer = 0.0f;
			rAlphaValue = 0.0f;
			rElapsedTimeByEffect = 0.0f;
			ui8EffectIdx = 0;

			vShapeIdxList.clear();
		}
	};

	const UINT8 MAX_EFFECT_IDX = 4;

	void	 Update_RankAndNationMark_Swap(REAL32 rDeltaSecond, UserUI_NationMarkData& UserUI_NationMark);		//���� <-> ��ũ ó�� �Լ�		
	void	 Update_RankMark_AlphaValue(i3UIListView_Box* LV_BOX, INT32 CellNumber, REAL32 RankMarkAlpha);
}
#endif
