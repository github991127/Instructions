
static bool __b_spell_reg1 = CSpellMgr::single()->RegisterSpellGetAddAttactDis( (unsigned int)YOKA_SKILL::SKILL_CHARACTER_SFMATCH_YI_CONG,boost::bind(&YiCongJ::GetAddAttackDis,_1));
static bool __b_spell_reg2 = CSpellMgr::single()->RegisterSpellGetAddDefDis( (unsigned int)YOKA_SKILL::SKILL_CHARACTER_SFMATCH_YI_CONG,boost::bind(&YiCongJ::GetAddDefDis,_1));

YiCongJ::YiCongJ(const CCardSpellData *pSpellData, CGame *pGame):CSpell(pSpellData, pGame,0){}

int YiCongJ::GetAddAttackDis(CRole* pRole)
{
	return 1;
}

int YiCongJ::GetAddDefDis(CRole* pRole)
{
	if (!pRole || pRole->GetCurHp() > 2)
		return 0;
	return 1;
}

void YiCongJ::BroadcastSpellOptMsg(CRole* pRole,int oldhp,int newhp)
{
	unsigned int spellid =  (unsigned int)YOKA_SKILL::SKILL_CHARACTER_SFMATCH_YI_CONG;
	if (!pRole || !pRole->HasCharacterSpell(spellid)
		|| oldhp == newhp
		|| !pRole->GetGame())
		return;

	bool b = false;
	if (oldhp <= 2 && newhp > 2)
		b = true;
	else if (oldhp > 2 && newhp <= 2)
		b = true;
	if (!b)
		return;

	MsgUseSpell msg;
	msg.spellId = spellid;
	msg.srcSeatId = pRole->GetSeatId();
	msg.useCardCount = 0;
	msg.user_param[0] = newhp > 2;
	msg.Len = sizeof(msg) - sizeof(msg.datas[0])*(msg.scDataMaxCnt-msg.destCount-msg.useCardCount);
	pRole->GetGame()->Broadcast(&msg);
}
