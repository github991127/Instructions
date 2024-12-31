⚫●◽★
——————————————————————
SKILL_ROGUELIKE_LIANG_CAO_BO_DUO = 13159, //粮草剥夺
SKILL_ROGUELIKE_LIANG_CAO_BO_DUO2 = 13173, //粮草剥夺2
SKILL_ROGUELIKE_LIANG_CAO_BO_DUO_XIN1 = 13271,//粮草剥夺?新?壹
SKILL_ROGUELIKE_LIANG_CAO_BO_DUO_XIN2 = 13272,//粮草剥夺?新?贰
SKILL_ROGUELIKE_SHI_QI_BO_DUO = 13161, //士气剥夺
SKILL_ROGUELIKE_SHI_QI_BO_DUO2 = 13175, //士气剥夺2
SKILL_ROGUELIKE_SHI_QI_BO_DUO_XIN1 = 13254,//士气剥夺?新?壹
SKILL_ROGUELIKE_SHI_QI_BO_DUO_XIN2 = 13255,//士气剥夺?新?贰


- TEST
粮草剥夺；402-陆绩

3变2-OK✔️
2变2-OK✔️

//当前摸牌数：不大于2，则不减；大于2，则最多减至2
unsigned int deal_count_raw = GetGame()->GetPhaseMgr()->GetRawDealCardCnt();
if(deal_count_raw > 2){
	unsigned int deal_count = 2;
	deal_count = max(deal_count, deal_count_raw - GetTriggerParams()[0]);
	GetGame()->GetPhaseMgr()->SetRawDealCardCnt(deal_count);
}
SetOverMark();

①
②
③
④
unsigned int deal_count = 2;
deal_count = max(deal_count,GetGame()->GetPhaseMgr()->GetRawDealCardCnt() - GetTriggerParams()[0]);
GetGame()->GetPhaseMgr()->SetRawDealCardCnt(deal_count);
SetOverMark();
①
②
③
④
甾裁：锁定技，当你受到伤害时，若你的体力值不小于来源且为奇数/偶数，你回复1点体力/失去半数体力，然后防止此伤害。
渐缞：你为使用者或目标的伤害牌结算后，若X小于目标数，你可以重铸使用者任意张牌，若重铸牌数小于X，其可将这些牌当可额外指定X名目标的【杀】使用（X为此牌造成的伤害数）。
