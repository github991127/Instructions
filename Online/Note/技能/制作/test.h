⚫●◽★
——————————————————————
SKILL_ROGUELIKE_YAO_LI_JING_TONG3 = 13314,  //药理精通1-1
SKILL_ROGUELIKE_YAO_LI_JING_TONG4 = 13315,  //药理精通1-3
SKILL_ROGUELIKE_YAO_LI_JING_TONG5 = 13316,  //药理精通2-1
SKILL_ROGUELIKE_YAO_LI_JING_TONG6 = 13317,  //药理精通2-3
SKILL_ROGUELIKE_YAO_LI_JING_TONG = 13179, //药理精通·壹
SKILL_ROGUELIKE_YAO_LI_JING_TONG2 = 13180, //药理精通·贰	

SKILL_CHARACTER_SFMATCH_FEI_YANG = 21071


void Roguelike_ChuLi::TimeOutCallBack(unsigned int seatId)
{
	//随机弃一张牌
	CRole* pTarget = GetCurTargetRole();
	if (pTarget != NULL && pTarget->IsAlive())
	{
		CPlayCard* pCard = pTarget->RandomGetMyCard();
		if (pTarget == GetCaster())
		{
			pCard = pTarget->RandomGetMyCanDisCard(GetSpellId());
		}
		if(pCard)
		{
			if (!DisCard(pTarget, pCard))
				APPLOG_ERROR_GAME_LOGIC(FUN_FILE_LINE);
		}
	}
	ClearAllOfWaitingOpt();
	SetResolveStep(step_next_target);
}