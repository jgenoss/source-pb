#if !defined( _GAME_BUTTON_H__)
#define _GAME_BUTTON_H__

#if ������_�͵�

class CGameButton : public i3GuiButton
{
	I3_CONCRETE_CLASS_DEFINE( CGameButton, i3GuiButton);

public:
	CGameButton(void);
	virtual ~CGameButton( void);

public:
	virtual void		OnInitAfterLoad(void);
};

#endif

#endif