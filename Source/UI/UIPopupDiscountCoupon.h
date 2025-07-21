#if !defined( __UI_POPUP_DISCOUNT_COUPON_H__)
#define __UI_POPUP_DISCOUNT_COUPON_H__

#include "UIPopupBase.h"

#if defined(USE_SALE_COUPON)

struct DISCOUNT_COUPON
{
	SALE_COUPON _Coupon;
	bool		_Used;
};
#endif

class UIPopupDiscountCoupon : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupDiscountCoupon, UIPopupBase);

public:
	UIPopupDiscountCoupon();
	virtual ~UIPopupDiscountCoupon();

#if defined(USE_SALE_COUPON)
	virtual void	OnCreate(i3GameNode * pParent) override;
	/*
	param[in] pArg1 i3UIControl*
	*/
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd() override;
	virtual void	OnExitEnd() override;
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;

public:
	void			OnClickButton();	// ��� or �ݱ⿡ ���� ó��
	void			Confirm();			// ���
	void			Cancel();			// �ݱ�
	virtual bool	OnKey_Enter() override { OnClickButton(); return true; }
	virtual bool	OnKey_Escape() override { Cancel(); return true;  }
	bool			CheckDiscountCoupon(SHOP_GOODS * pGoods);
	void			ChangeCurrnetSlot(INT32 slot);

private:

	void			_InitCouponList();	// ���� ����Ʈ ����
	void			_SetNoticeMsg();	// ���� �޼��� ����
	void			_SetButtonText();	// ��ư �ؽ�Ʈ ���� (��� or �ݱ�)
	void			_AddCouponItem(INT32 slot, INT32 nCouponIdx);
	void			_SetCashLimit(i3::stack_wstring& wstrDest, SALE_COUPON * pCoupon);
	void			_SetDiscountRate(i3::stack_wstring& wstrDest, SALE_COUPON * pCoupon);
	void			_SetItemTypeLimit(i3::stack_wstring& wstrDest, SALE_COUPON * pCoupon);
	void			_SetTerm(i3::stack_wstring& wstrDest, SALE_COUPON * pCoupon, UINT32 ItemTerm);

	bool			m_bCouponUseWnd;	// ���� ��� â
	INT32			m_nCurrentSlot;
	i3UIControl *	m_pBtnCtrl;
#endif
};


#endif