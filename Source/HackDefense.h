#if !defined(__HACK_DEFENSE_H)
#define __HACK_DEFENSE_H


namespace hack_defense {}
namespace hd = hack_defense;

namespace hack_defense
{
	class check_observer
	{
	public:
		static check_observer*	i() { return tinst<check_observer>(); }
		void					ProcessCreateStage();
		void					init_roominfo();
		void					check();
	private:
		ROOM_INFO_ADD	m_roomInfo;		// ��ŷ��� ������� �߰��� ����
	};	

	class check_speedhack				// //speed hack// ����� ������ �ּ�ó���Ǿ�������, StageBattle���� ������ �ű�.
	{
	public:
		static check_speedhack*	i() { return tinst<check_speedhack>(); }
		void					ProcessCreateStage();
		void					init_pos();
		void					checkSpeedPos();
		void					checkMaxSpeed(REAL32 fSpeed);
		bool					getFirstRespawn()const{return m_bFirstSpeedCheck;}
	private:
		VEC3D					m_vPos;
		bool					m_bFirstSpeedCheck;
	};

	
	class check_UnknwonPos				// //UnknwonPos// ĳ���� ��ġ�� �������� �ٴڿ� ��Ʈ�ø����� ���� ��� ����
	{
	public:
		static check_UnknwonPos*	i() { return tinst<check_UnknwonPos>(); }
		void					check(i3CollideeLine * pLine);
	private:

	};

	class check_PhysixDll				// //PhysixDll// ������ ���� DLL ���� ���� ����.
	{
	public:
		static check_PhysixDll*	i() { return tinst<check_PhysixDll>(); }
		void					check();
	private:

	};
	
	// �Ʒ� �ʵ����� üũ�� ��ƾ�� �����ϴµ�, ������� �������� ������ �������� �ʰ� ����...(����������Ʋ���� �ű�)
	class check_mapdata
	{
	public:
		static check_mapdata* i() { return tinst<check_mapdata>(); }
		void					ProcessCreateStage();
		void					check(INT32	nStageIndex);
		bool					is_hacked() const { return !m_IsStageDataSafe; }
	private:
		bool					m_IsStageDataSafe;
	};

	class check_elapsedsynctimer
	{
	public:
		static check_elapsedsynctimer* i() { return tinst<check_elapsedsynctimer>(); }
		void					ProcessCreateStage();
		void					Update(REAL32 rDeltaSeconds);
		void					ResetTimer() { m_fElapsedTimerCheck = 0.f; }
		void					ResetGravityTimer() { m_fElapsedGravityTimerCheck = 0.f; }

//		void					InitMagnetStartTime();
//		void					InitMagnet();

	private:
	
		REAL32					m_fElapsedTimerCheck;
		REAL32					m_fElapsedSpeedTimerCheck;
		REAL32					m_fElapsedGravityTimerCheck;
	};



	
}



#endif