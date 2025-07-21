#if !defined( __UI_POPUP_ROULETTE_JACKPOT_H__)
#define __UI_POPUP_ROULETTE_JACKPOT_H__

#include "UIPopupBase.h"


class UIPopupRouletteJackpot : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupRouletteJackpot, UIPopupBase);

protected:
	void			_ModifyControl( void);
	i3UI3DViewBox *		m_p3DView;

	INT32			m_nIndex;

public:
	UIPopupRouletteJackpot();
	virtual ~UIPopupRouletteJackpot();
	virtual void	OnCreate( i3GameNode * pParent);


	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();
	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnEntranceEnd( void);


		
	/** \brief 3DViewBox�� �����մϴ�.
		\param[in] pParent Attach�� Parent Node */
	void			Create3DViewBox( i3UIControl * pParent);

	/** \brief 3DViewBox�� �����մϴ�. */
	void			Delete3DViewBox( void);

	/** \brief Character�� �����մϴ�.
		\desc ������������ 3DViewBox�� �����ǰ� ȣ��Ǿ� ��*/

	void			CreateViewEffect();

	void			SetName();

};

#endif
