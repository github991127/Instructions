
/*
武将	：界公孙瓒
技能名称：义从
技能描述：锁定技，你计算与其他角色的距离-1；若你已损失的体力值大于等于2，其他角色计算与你的距离+1。
操作提示：
*/

class YiCongJ : public CSpell, public CreateXSpell<YiCongJ,  (unsigned int)YOKA_SKILL::SKILL_CHARACTER_SFMATCH_YI_CONG>
{
public:
	YiCongJ(const CCardSpellData *pSpellData, CGame *pGame);
	static int GetAddAttackDis(CRole* pRole);
	static int GetAddDefDis(CRole* pRole);
	static void BroadcastSpellOptMsg(CRole* pRole,int oldhp,int newhp);
};
