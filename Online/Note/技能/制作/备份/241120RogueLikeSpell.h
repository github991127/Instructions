
/*
SKILL_ROGUELIKE_KU_ROU = 14109, //苦肉
武将	：黄盖（隐)
技能名称：苦肉
技能描述：出牌阶段限3次，你可以弃置一张牌，可以失去1点体力，然后回复1点体力
配置项：spell_time_limit = 3,discard_count = 0,add_hp = 1,
*/
class Roguelike_KuRou : public CSpell, public CreateXSpell<Roguelike_KuRou, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_KU_ROU>
{
	enum
	{
		step_begin = 0,
		step_lose_hp,
		step_end
	};
public:
	Roguelike_KuRou(const CCardSpellData* pSpellData, CGame* pGame);
	static  eSpellCastResult CanCast(CGame* pGame, CanCastParam* pParam);

protected:
	void Resolve();
	void onStepBegin();
	void onStepLoseHp();
	void onStepAddHp();
};
/*
SKILL_ROGUELIKE_LANG_XI = 14113, //狼袭
register_opp(Opp_phase_begin,  (unsigned int)YOKA_SKILL::SKILL_CHARACTER_LANG_XI); //肉鸽狼袭
武将	：李傕肉鸽
技能名称：狼袭
技能描述：每个人的准备阶段，你可以对一名体力值不大于你的其他角色造成1~2点随机伤害。
*/
class Roguelike_LangXi : public CSpell, public CreateXSpell<Roguelike_LangXi, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_LANG_XI>
{
public:
	Roguelike_LangXi(const CCardSpellData* pSpellData, CGame* pGame);
	static bool CanTriggerMe(CGame* pGame, CanTriggerMeParam* pParam);
	static eSpellCastResult CanCast(CGame* pGame, CanCastParam* pParam);
protected:
	void Resolve()override;
};
/*
SKILL_ROGUELIKE_PO_JUN = 14112,

register_opp(Opp_after_choose_target, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_PO_JUN);
register_opp_passive(Opp_after_turn_end, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_PO_JUN, TriggerSpell::mark_not_check_role_have_spell | TriggerSpell::mark_trigger_single | TriggerSpell::mark_not_notify_client_usespell);
register_opp_use_mark(Opp_When_Cause_Damage, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_PO_JUN, TriggerSpell::mark_passive_trigger);
-
remove_type_po_jun_rogue,

m_remove_zone_ex[remove_type_po_jun_rogue].SetSpellId((unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_PO_JUN);
m_remove_zone_ex[remove_type_po_jun_rogue].SetDiscardWhenSpellLose(false);
m_remove_zone_ex[remove_type_po_jun_rogue].SetPublicVisible(false);
武将	：大宝 界徐盛 肉鸽
技能名称：破军
技能描述：当你使用【杀】指定目标后，你可以将其所有手牌移出游戏直到回合结束（X为其体力值）。你使用的【杀】对手牌区与装备区内牌数皆不大于你的角色造成的伤害+1。
*/
class Roguelike_CPoJun : public CSpell, public CreateXSpell<Roguelike_CPoJun, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_PO_JUN>
{
	enum
	{
		step_begin = 0,
		step_wait_movecard,
		step_end
	};

	enum
	{
		effect_delcard = 1, // 盖牌
		effect_givecard = 2,	 // 还原牌
		effect_add_damage = 3, // 加伤
	};

public:
	Roguelike_CPoJun(const CCardSpellData* pSpellData, CGame* pGame);
	static	bool CanTriggerMe(CGame* pGame, CanTriggerMeParam* pParam);
	static  eSpellCastResult CanCast(CGame* pGame, CanCastParam* pParam);
	virtual void NetMsgMoveCardRpy(const MsgMoveCard* pMsg, CGsUser* pUser);
	virtual void TimeOutCallBack(unsigned int seatId);
	static void removeSpellState(CGame* pGame, CRole* pCaster);
protected:
	void Resolve();
	bool DoMoveCard(PPlayCardList& cardlist, PPlayCardList& handlist, bool bTimeOut = false);
	// 删牌
	void ResolveDelCard();
	// 归还牌
	void ResolveGiveCard();
	// 增加伤害
	void ResolveAddDamage();
};

class Roguelike_CPoJun_State :public CSpellState {
public:
	Roguelike_CPoJun_State(CRole* pCasterRole, CRole* pOwner);
	virtual bool CanRemove(const TRemoveParam& removeParam)const override;
};
/*
SKILL_ROGUELIKE_SHEN_JI = 14131, // 神戟
maxCnt += Roguelike_ShenJi::GetMaxTargetCnt(m_prole);
Roguelike_ShenJi::IngoreUseShaTimes(GetCaster())
武将	：神鬼无前(虎牢关吕布)
技能名称：神戟
技能描述：你出牌阶段使用【杀】无次数限制，且可以多指定两个目标。
*/

class Roguelike_ShenJi : public CSpell, public CreateXSpell<Roguelike_ShenJi, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_JI>
{
public:
	Roguelike_ShenJi(const CCardSpellData *pSpellData, CGame *pGame);

	static unsigned int GetMaxTargetCnt(CRole* pRole);
	static bool IngoreUseShaTimes(CRole* pRole);
	static bool BroadcastSpellOptMsg(CRole* pRole, SeatIdList& targets);
private:
protected:
	static bool CanUse(CRole* pRole);

public:
private:
protected:
};

/*
SKILL_ROGUELIKE_SHEN_QU = 14132,

register_opp(Opp_when_turn_begin,  (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_QU);
register_opp(Opp_hurt_but_not_dead,  (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_QU);

武将	：神鬼无前(虎牢关吕布)
技能名称：神躯
技能描述：①每名角色的准备阶段，若你的手牌数不大于体力上限，你可以摸X张牌。（X为你的当前体力上限）②当你受到伤害后，你可以使用一张【桃】。
*/
class Roguelike_ShenQu : public CSpell, public CreateXSpell<Roguelike_ShenQu, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_QU>
{
	enum
	{
		step_begin = 0,
		step_wait_use_tao,
		step_end
	};
public:
	Roguelike_ShenQu(const CCardSpellData *pSpellData, CGame *pGame);

	static bool CanTriggerMe(CGame* pGame, CanTriggerMeParam* pParam);

	static eSpellCastResult CanCast(CGame* pGame, CanCastParam* pParam);

	eSpellCastResult NetMsgCancelRpyResult(const MsgClientRoleOptRep *pMsg, CGsUser *pUser);

	eSpellCastResult NetMsgUseCardRpyResult(const MsgUseCard *pMsg, CGsUser *pUser);

	eSpellCastResult NetMsgUseSpellRpyResult(const MsgUseSpell *pMsg, CGsUser *pUser);
protected:
	virtual void Resolve();

	void TimeOutCallBack(unsigned int seatId);

	void SuccessUseCard();

	void FailUseCard();

	void CancelEffect();
};

/*
SKILL_ROGUELIKE_SHEN_WEI = 14130, // 神威
register_opp(Opp_add_deal_cnt,  (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_WEI); //神威
limitcnt += Roguelike_ShenWei::GetExHandCardLimit(prole);
武将	：暴怒的战神(虎牢关吕布)
技能名称：神威
技能描述：锁定技，摸牌阶段，你多摸X张牌，你的手牌上限+X（X为敌方角色数）。
*/
class Roguelike_ShenWei : public CSpell, public CreateXSpell<Roguelike_ShenWei, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_WEI>
{
public:
	Roguelike_ShenWei(const CCardSpellData *pSpellData, CGame *pGame);
	static bool CanTriggerMe(CGame *pgame, CanTriggerMeParam* pparam);
	static eSpellCastResult CanCast(CGame *game, CanCastParam* pparam) { return SPELL_CAST_OK; }
	static int GetExHandCardLimit(CRole* pRole);
protected:
	virtual void Resolve();
};

/*
SKILL_ROGUELIKE_XIU_LUO = 14129, // 修罗
register_opp(Opp_phase_begin,  (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_XIU_LUO);

武将	：神吕布
技能名称：修罗
技能描述：准备阶段，你可以弃置一张牌，然后弃置你判定区里所有牌。
*/

class Roguelike_XiuLuo : public CSpell, public CreateXSpell<Roguelike_XiuLuo, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_XIU_LUO>
{
	enum
	{
		step_begin = 0, //选择要弃的牌
		step_ask_discard_judge, //弃置判定区所有牌
		step_end
	};
public:
	Roguelike_XiuLuo(const CCardSpellData *pSpellData, CGame *pGame);
	static bool CanTriggerMe(CGame *pgame, CanTriggerMeParam* pparam);
	static eSpellCastResult CanCast(CGame *game, CanCastParam* pparam);
protected:
	virtual void Resolve();
};
