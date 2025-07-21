
#ifndef __POPUP_LINKSHOP_H__
#define __POPUP_LINKSHOP_H__

#include "PopupBase.h"


class CPopupLinkShop : public CPopupBase
{
	I3_CLASS_DEFINE(CPopupLinkShop);

public:
	CPopupLinkShop(void);
	virtual ~CPopupLinkShop();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnExitKey(void);
	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	virtual	BOOL	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

private:
	
	i3GuiStatic* m_pCheckPrimaryWeapon;
	i3GuiStatic* m_pSelectLinkShop;
	i3GuiStatic* m_pBuyPrimaryWeapon;
	
	i3GuiButton* m_pBuyPrimaryWeaponConfirm;
	i3GuiButton* m_pCheckPrimaryWeaponConfirm;
	i3GuiButton* m_pSelectLinkShopConfirm;
	i3GuiButton* m_pSelectLinkShopCancle;

	i3GuiEditBox* m_pCheckPrimaryWeaponMessage;//�ֹ��Ⱑ ������ �ʾ� �������� �̵��մϴ�.
	i3GuiEditBox* m_pBuyPrimaryWeaponMessage;  //�ֹ��⸦ ������ ����ϼ���
	i3GuiEditBox* m_pSelectLinkShopMessage;    //�ֹ��Ⱑ ������ �ʾҽ��ϴ�. �������� �̵��Ͻðڽ��ϱ�?
	i3GuiEditBox* m_pCheckPrimaryWeaponCaption;
	i3GuiEditBox* m_pBuyPrimaryWeaponCaption;  
	i3GuiEditBox* m_pSelectLinkShopCaption;  

	void _CheckPrimaryWeaponConfirm(void);
	void _BuyPrimaryWeaponConfirm(void);

	void _SelectLinkShopConfirm(void);
	void _SelectLinkShopCancle(void);


};

#endif	//	