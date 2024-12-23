
//////////////////////////////////////////////////////////////////////////
//���� ���
auto ID_RNS_SKILL_ROGUELIKE_KU_ROU = (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_KU_ROU;
REGISTER_NORMAL_SPELL(Roguelike_KuRou, ID_RNS_SKILL_ROGUELIKE_KU_ROU);
Roguelike_KuRou::Roguelike_KuRou(const CCardSpellData* pSpellData, CGame* pGame) : CSpell(pSpellData, pGame, step_begin)
{

}

eSpellCastResult Roguelike_KuRou::CanCast(CGame* pGame, CanCastParam* pParam)
{
	if (NULL == pGame || NULL == pParam)
		return SPELL_FAILED_UNKNOWN;

	if (NULL == pParam->GetCaster() || !pParam->GetCaster()->IsAlive())
		return SPELL_FAILED_NO_SRC;

	//���޼�飺����
	if (pParam->GetCaster()->GetCurHp() < 1)
		return SPELL_FAILED_UNKNOWN;

	//��ȡ���ã����ƽ׶����ƴ���
	const CCardSpellExtendData* pSpellExData = CCardDataRepository::Singleton(pGame->GetGameStartTime())->GetSpellExtendData(SpellId());
	unsigned int spell_time_limit = 3;
	if (pSpellExData)
	{
		spell_time_limit = pSpellExData->getParam(0);
	}
	else
	{
		APPLOG_ERROR_GAME_LOGIC("KuRou spell config error:spell_time_limit" << FUN_FILE_LINE);
	}

	//���޼�飺ʹ�ô���
	CRoleSpellData* pSpellData = NULL;
	if (pParam->GetCaster()->GetSpellStateMgr() != NULL)
		pSpellData = pParam->GetCaster()->GetSpellStateMgr()->GetData((unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_KU_ROU);
	if (NULL == pSpellData || pSpellData->GetSpellUseTimes() > spell_time_limit)
		return SPELL_FAILED_UNKNOWN;

	//��ȡ���ã�Ҫ��������
	unsigned int discard_count = 0;
	if (pSpellExData)
	{
		discard_count = pSpellExData->getParam(1);
	}
	else
	{
		APPLOG_ERROR_GAME_LOGIC("KuRou spell config error:discard_count" << FUN_FILE_LINE);
	}

	//���޼�飺���ƺϷ�
	if (discard_count != 0) {
		if (pParam->use_cardlist.size() != discard_count)
			return SPELL_FAILED_USECARD_WRONG_USE_COUNT;

		CPlayCard* pCard = pParam->GetCaster()->GetMyPlayCard(pParam->use_cardlist[0]);
		if (NULL == pCard || !CSpellMgr::single()->CanDiscard(*pParam->GetCaster(), *pCard, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_KU_ROU))
			return SPELL_FAILED_USECARD_NOT_HAVE;
	}

	return SPELL_CAST_OK;
}

void Roguelike_KuRou::Resolve()
{
	switch (GetResolveStep())
	{
		//����
	case step_begin:
		onStepBegin();
		break;
	case step_lose_hp:
		onStepLoseHp();
		break;
	case step_end:
		onStepAddHp();
		SetOverMark();
		break;
	default:
	{
		APPLOG_ERROR_GAME_LOGIC("Step error, step = " << GetResolveStep() << FUN_FILE_LINE);
		SetOverMark();
	}
	break;
	}
}

void Roguelike_KuRou::onStepBegin()
{
	//�޸ģ�����ʹ�ô�������
	if (GetGame() != NULL && GetCaster() != NULL)
	{
		if (GetCaster()->GetSpellStateMgr() != NULL)
		{
			CRoleSpellData* pRoleSpellData = GetCaster()->GetSpellStateMgr()->GetData(GetSpellId());
			if (pRoleSpellData != NULL)
				pRoleSpellData->AddSpellUseTimes();
		}

		//��ȡ���ã�Ҫ��������
		const CCardSpellExtendData* pSpellExData = CCardDataRepository::Singleton(GAME_START_TIME)->GetSpellExtendData(SpellId());
		unsigned int discard_count = 0;
		if (pSpellExData)
		{
			discard_count = pSpellExData->getParam(1);
		}
		else
		{
			APPLOG_ERROR_GAME_LOGIC("KuRou spell config error:discard_count" << FUN_FILE_LINE);
		}

		//��Ҫ�����ƣ�ֱ����һ���������仯
		if (discard_count == 0) {
			SetResolveStep(step_lose_hp);
			return;
		}

		//Ҫ������
		PPlayCardList card_list;
		for (CardIdList::const_iterator iter = GetCardList().begin(); iter != GetCardList().end(); ++iter)
		{
			CPlayCard* pCard = GetCaster()->GetMyPlayCard(*iter);
			if (pCard != NULL)
				card_list.push_back(pCard);
		}
		if (!card_list.empty())
		{
			if (!DisCardFromRole(GetCaster(), GetCaster(), card_list))
				APPLOG_ERROR_GAME_LOGIC("Discard error" << FUN_FILE_LINE);
		}
		SetResolveStep(step_lose_hp);
		return;
	}
	SetOverMark();
}

void Roguelike_KuRou::onStepLoseHp()
{
	if (GetGame() != NULL && GetCaster() != NULL && GetCaster()->GetCurHp() > 0)
	{
		//ʧȥ����
		CDamageAction* pDamAc = CDamageAction::Damage(GetGame(), sc_invalid_seat_id, GetCaster()->GetSeatId(), this, 1, CardIdList(), CDamageAction::Property_lose);
		if (NULL == pDamAc)
			APPLOG_ERROR_GAME_LOGIC("Damage error" << FUN_FILE_LINE);
	}
	SetResolveStep(step_end);
}

void Roguelike_KuRou::onStepAddHp(){
	if (GetGame() != NULL && GetCaster() != NULL && GetCaster()->GetCurHp() > 0)
	{
		//��ȡ���ã��ظ�����ֵ
		const CCardSpellExtendData* pSpellExData = CCardDataRepository::Singleton(GAME_START_TIME)->GetSpellExtendData(SpellId());
		unsigned int add_hp = 1;
		if (pSpellExData) {
			add_hp = pSpellExData->getParam(2);
		}
		else {
			APPLOG_ERROR_GAME_LOGIC("KuRou spell config error:add_hp" << FUN_FILE_LINE);
		}
		//�ظ�����
		CAddHpAction::AddHp(GetGame(), GetCaster(), GetCaster(), add_hp, this);
	}
}

//////////////////////////////////////////////////////////////////////////
//��Ϯ ���
auto ID_RTS_SKILL_ROGUELIKE_LANG_XI = (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_LANG_XI;
REGISTER_TRIGGER_SPELL(Roguelike_LangXi, ID_RTS_SKILL_ROGUELIKE_LANG_XI)
Roguelike_LangXi::Roguelike_LangXi(const CCardSpellData* pSpellData, CGame* pGame) :CSpell(pSpellData, pGame, 0)
{

}

bool Roguelike_LangXi::CanTriggerMe(CGame* pGame, CanTriggerMeParam* pParam)
{
	if (nullptr == pGame || nullptr == pParam)
		return false;

	CRole* pRole = pGame->GetRoleBySeatId(pParam->seat_id);
	if (nullptr == pRole || !pRole->IsAlive())
		return false;

	// ���޼�飺ѡ���ɫ����
	CRole* pTarget = nullptr;
	pGame->ForEachRole([&](CRole& role)->bool
	{
		if (role.GetSeatId() != pRole->GetSeatId() && role.GetCurHp() <= pRole->GetCurHp())
		{
			pTarget = &role;
			return false;
		}
		return true;
	}, true);
	pParam->uTriggerIndex = 2;
	return pTarget != nullptr;
}

eSpellCastResult Roguelike_LangXi::CanCast(CGame* pGame, CanCastParam* pParam)
{
	if (nullptr == pGame || nullptr == pParam)
		return SPELL_FAILED_UNKNOWN;

	const auto pCaster = pParam->GetCaster();
	if (pCaster == nullptr || !pCaster->IsAlive())
		return SPELL_FAILED_NO_SRC;

	//���޼�飺ѡ���ɫ��
	if (pParam->targets.size() != 1)
		return SPELL_FAILED_TARGET_CNT_ERROR;

	//���޼�飺ѡ���ɫ����
	const auto pTarget = pGame->GetRoleBySeatId(pParam->targets.at(0));
	if (pTarget == nullptr || !pTarget->IsAlive()
		|| pTarget->GetCurHp() > pCaster->GetCurHp()
		|| pTarget->GetSeatId() == pCaster->GetSeatId())
		return SPELL_FAILED_TARGET_ILLEGALITY;
	return SPELL_CAST_OK;
}

void Roguelike_LangXi::Resolve()
{
	if (GetCurTargetRole() != nullptr && GetCurTargetRole()->IsAlive())
	{
		//��ȡ���ã��˺���������
		unsigned int lowerLimit = 1;
		unsigned int upperLimit = 2;
		const CCardSpellExtendData*pSpellExData = CCardDataRepository::Singleton(GAME_START_TIME)->GetSpellExtendData(SpellId());
		if (pSpellExData)
		{
			lowerLimit = pSpellExData->getParam(0);
			upperLimit = pSpellExData->getParam(1);
		}
		else
		{
			APPLOG_ERROR_GAME_LOGIC("LangXi spell config error:lowerLimit upperLimit" << FUN_FILE_LINE);
		}

		//����˺�ֵ
		unsigned int count = CSgsPubFun::rand_uint(lowerLimit, upperLimit);
		if (count > 0)
		{
			CDamageAction::Damage(GetGame(), GetCasterSeatId(), GetCurTarget(), this, count);
		}
	}
	SetOverMark();
}

//////////////////////////////////////////////////////////////////////////
//�ƾ� ���
auto ID_RTS_SKILL_ROGUELIKE_PO_JUN = (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_PO_JUN;
REGISTER_TRIGGER_SPELL(Roguelike_CPoJun, ID_RTS_SKILL_ROGUELIKE_PO_JUN)

Roguelike_CPoJun::Roguelike_CPoJun(const CCardSpellData* pSpellData, CGame* pGame) : CSpell(pSpellData, pGame, step_begin)
{

}

bool Roguelike_CPoJun::CanTriggerMe(CGame* pGame, CanTriggerMeParam* pParam)
{
	if (NULL == pGame || NULL == pParam)
	{
		return false;
	}

	CRole* pRole = pGame->GetRoleBySeatId(pParam->seat_id);
	if (NULL == pRole || !pRole->IsAlive())
	{
		return false;
	}

	if (pParam->pTriggerAction->get_opp() == CTriggerAction::Opp_after_choose_target)
	{
		if (!pParam->paction)
			return false;

		//ƥ���飺ɱ��ʹ����
		Sha* pSha = dynamic_cast<Sha*>(pParam->paction);
		if (NULL == pSha || pSha->GetCaster() != pRole)
			return false;

		//�ռ�飺Ŀ��������
		unsigned int seatid = pSha->GetCurTarget();
		CRole* pTarget = pGame->GetRoleBySeatId(seatid);
		if (!pTarget || !pTarget->IsAlive() || pTarget->IsHandZoneEmpty())
			return false;

		pParam->uTriggerIndex = effect_delcard;
		pParam->targetList.push_back(seatid);
		pParam->bTargetLocked = true;
		return true;
	}
	else if (pParam->pTriggerAction->get_opp() == CTriggerAction::Opp_after_turn_end)
	{
		CRemovedFromGameZone* pZone = pRole->GetExRemoveCardZone(CRole::remove_type_po_jun_rogue);
		if (!pZone || pZone->Empty())
		{
			return false;
		}
		pParam->uTriggerIndex = effect_givecard;
		return true;
	}
	else if (pParam->pTriggerAction->get_opp() == CTriggerAction::Opp_When_Cause_Damage)
	{
		CDamageAction* pDamAc = dynamic_cast<CDamageAction*>(pParam->paction);
		if (NULL == pDamAc || pDamAc->IsDamagePrevent())
			return false;

		if (pDamAc->GetHurter() != pRole)
			return false;

		CRole* pTarget = pDamAc->GetTarget();
		if (!pTarget || !pTarget->IsAlive())
			return false;

		Sha* pSha = dynamic_cast<Sha*>(pDamAc->GetSrcAction());
		if (pSha == NULL)
			return false;

		// ���޼�飺����
		if (pTarget->GetHandCardSize() > pRole->GetHandCardSize())
			return false;
		if (pTarget->GetEquipCardSize() > pRole->GetEquipCardSize())
			return false;

		pParam->uTriggerIndex = effect_add_damage;
		return true;
	}
	return false;
}

eSpellCastResult Roguelike_CPoJun::CanCast(CGame* pGame, CanCastParam* pParam)
{
	return SPELL_CAST_OK;
}

void Roguelike_CPoJun::Resolve()
{
	if (!GetGame() || !GetCaster())
	{
		SetOverMark();
		ClearAllOfWaitingOpt();
		return;
	}
	switch (GetSpellEffectIdx())
	{
	case effect_delcard:
	{
		ResolveDelCard();
	}
	break;
	case effect_givecard:
	{
		ResolveGiveCard();
	}
	break;
	case effect_add_damage:
	{
		ResolveAddDamage();
	}
	break;
	default:
		break;
	}
}

void Roguelike_CPoJun::ResolveDelCard()
{
	switch (GetResolveStep())
	{
	case step_begin:
	{
		if (NULL == GetSrcAction())
		{
			APPLOG_ERROR_GAME_LOGIC("Error srcac" << FUN_FILE_LINE);
			SetOverMark();
			return;
		}

		Sha* pSha = dynamic_cast<Sha*>(GetSrcAction());
		if (NULL == pSha)
		{
			APPLOG_ERROR_GAME_LOGIC("Error srcac" << FUN_FILE_LINE);
			SetOverMark();
			return;
		}

		if (!GetCurTargetRole() || !GetCurTargetRole()->IsAlive() || (GetCurTargetRole()->IsHandZoneEmpty()))
		{
			APPLOG_ERROR_GAME_LOGIC("Error targetRole" << FUN_FILE_LINE);
			SetOverMark();
			return;
		}
		SetResolveStep(step_wait_movecard);
		return;
	}
	break;
	case step_wait_movecard:{
		CRole* pTarget = GetCurTargetRole();
		if (NULL == pTarget)
		{
			SetOverMark();
			return;
		}

		//ѡ����������
		PPlayCardList cardlist;
		PPlayCardList handlist;
		for (unsigned int i = 0; i < (pTarget->GetHandCardSize()); ++i)
		{
			CPlayCard* pCard = pTarget->GetHandCardZone()->At(i);
			if (pCard != NULL)
			{
				handlist.push_back(pCard);
			}
		}
		DoMoveCard(cardlist, handlist, true);
		SetOverMark();
	}
							break;
	case step_end:
	{
		SetOverMark();
	}
	break;
	default:
	{
		ClearAllOfWaitingOpt();
		SetOverMark();
	}
	break;
	}
}

void Roguelike_CPoJun::ResolveGiveCard()
{
	//�ռ�飺�佫����
	CRemovedFromGameZone* pZone = GetCaster()->GetExRemoveCardZone(CRole::remove_type_po_jun_rogue);
	if (!pZone || pZone->Empty())
	{
		SetOverMark();
		return;
	}
	SeatIdList seatlist;
	seatlist.push_back(GetCaster()->GetSeatId());
	CMoveCardAction* pac = CMoveCardAction::MoveCards(GetGame(), EmActMode::Mode_MoveTo,
		pZone->GetPlayCardList(), seatlist, GetCasterSeatId(), GetCaster(), GetSpellId(),
		pZone, GetCaster()->GetHandCardZone(), GetCaster()->GetSeatId(), GetCaster()->GetSeatId());
	if (!pac)
	{
		APPLOG_ERROR_GAME_LOGIC("movecard Error pj" << FUN_FILE_LINE);
	}

	//��Ϣ������Ч����Ч
	MsgRoleSpellOptRep msg;
	msg.seatId = GetCasterSeatId();
	msg.optType = MsgRoleSpellOptRep::OPP_SPELL_EFFECT;
	msg.SpellId = GetSpellId();
	msg.dataCnt = 0;
	msg.Len = sizeof(msg) - sizeof(msg.data);
	GetGame()->Broadcast(&msg);
	SetOverMark();
	return;
}

void Roguelike_CPoJun::ResolveAddDamage()
{
	if (!GetCaster() || !GetCaster()->IsAlive() || !GetCaster()->GetSpellStateMgr()) {
		SetOverMark();
		return;
	}

	if (!GetCaster()->GetSpellStateMgr()->FindSpellState(GetSpellId())) {
		/* ��Ͽͻ��˲������� */
		MsgUseSpell useSpell;
		useSpell.chrId = GetFromCharacterId();
		useSpell.effectIndex = 99;
		useSpell.spellId = GetSpellId();
		useSpell.srcSeatId = GetCasterSeatId();
		useSpell.Len = sizeof(MsgUseSpell) - sizeof(useSpell.datas[0]) * (useSpell.scDataMaxCnt - useSpell.destCount - useSpell.useCardCount);
		GetGame()->Broadcast(&useSpell);
		SetUseSpellFlag(true);
	}

	CDamageAction* pDamAc = dynamic_cast<CDamageAction*>(GetSrcAction());
	if (pDamAc != NULL)
	{
		pDamAc->AddDamageHp(1);
	}
	SetOverMark();
	return;
}

void Roguelike_CPoJun::NetMsgMoveCardRpy(const MsgMoveCard* pMsg, CGsUser* pUser)
{
	CRole* ptarget = GetCurTargetRole();
	if (NULL == GetGame() || NULL == ptarget || (ptarget->IsHandZoneEmpty()))
	{
		Log_BaseInfo(GetSpellId(), GetCaster(), true);
		APPLOG_ERROR_GAME_LOGIC("logic error" << FUN_FILE_LINE);
		return;
	}

	//ƥ���飺����
	if (GetSpellEffectIdx() != effect_delcard || GetResolveStep() != step_wait_movecard)
	{
		return;
	}

	//�ռ�飺ѡ����
	if (0 == pMsg->cardCnt)
	{
		APPLOG_ERROR_GAME_LOGIC("client logic error" << FUN_FILE_LINE);
		return;
	}
	if (!ptarget->GetHandCardZone() || !ptarget->GetEquipCardZone())
		return;

	//��Ҫ���������������
	PPlayCardList hand_cards;
	ptarget->GetHandCardZone()->Random((pMsg->cardCnt - pMsg->dataCnt), hand_cards);

	//��ѡ��װ��
	PPlayCardList equip_cards;
	for (unsigned int i = 0; i < (unsigned int)pMsg->dataCnt; ++i)
	{
		CPlayCard* pcard = ptarget->GetEquipCardZone()->Find(pMsg->data[i]);
		if (!pcard)
		{
			APPLOG_ERROR_GAME_LOGIC("client logic error equip id=" << pMsg->data[i] << FUN_FILE_LINE);
			return;
		}
		equip_cards.push_back(pcard);
	}

	//�ƶ���
	if (DoMoveCard(equip_cards, hand_cards, false))
	{
		ClearAllOfWaitingOpt();
		SetOverMark();
		return;
	}
	APPLOG_ERROR_GAME_LOGIC("movecard = false" << FUN_FILE_LINE);
}

void Roguelike_CPoJun::TimeOutCallBack(unsigned int seatId)
{
	ClearAllOfWaitingOpt();
	if (GetSpellEffectIdx() == effect_delcard && GetResolveStep() == step_wait_movecard)
	{
		CRole* pTarget = GetCurTargetRole();
		if (NULL == pTarget)
		{
			SetOverMark();
			return;
		}

		//ѡ����������
		PPlayCardList cardlist;
		PPlayCardList handlist;
		for (unsigned int i = 0; i < (pTarget->GetHandCardSize()); ++i)
		{
			CPlayCard* pCard = pTarget->GetHandCardZone()->At(i);
			if (pCard != NULL)
			{
				handlist.push_back(pCard);
			}
		}
		DoMoveCard(cardlist, handlist, true);
		SetOverMark();
		return;
	}
	SetOverMark();
	return;
}

void Roguelike_CPoJun::removeSpellState(CGame* pGame, CRole* pCaster)
{
	if (!pGame || !pCaster || !pCaster->IsAlive() || !pCaster->GetSpellStateMgr()) {
		return;
	}

	auto* pState = dynamic_cast<Roguelike_CPoJun_State*>(pCaster->GetSpellStateMgr()->FindSpellState((unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_PO_JUN));
	if (!pState) {
		return;
	}

	pCaster->GetSpellStateMgr()->RemoveSpellState(pState);
}

bool Roguelike_CPoJun::DoMoveCard(PPlayCardList& cardlist, PPlayCardList& handlist, bool bTimeOut /*= false*/)
{
	if (NULL == GetCaster() || NULL == GetGame())
	{
		return false;
	}
	CRole* pTarget = GetCurTargetRole();
	if (NULL == pTarget)
	{
		return false;
	}
	CMoveCardAction* pMovAc = NULL;
	bool bHaveError = false;
	if (!cardlist.empty())
	{
		SeatIdList seatList;
		GetGame()->GetRoleSeatIdListAll(seatList, false);
		pMovAc = CMoveCardAction::MoveCardsFromRole(GetGame(), EmActMode::Mode_MoveTo,
			cardlist, seatList, GetCasterSeatId(), GetCaster(), GetSpellId(),
			pTarget->GetSeatId(), pTarget->GetExRemoveCardZone(CRole::remove_type_po_jun_rogue), pTarget->GetSeatId());
	}

	if (!handlist.empty())
	{
		SeatIdList seatList;
		seatList.push_back(pTarget->GetSeatId());
		if (NULL == pMovAc)
		{
			pMovAc = CMoveCardAction::MoveCards(GetGame(), EmActMode::Mode_MoveTo,
				handlist, seatList, GetCasterSeatId(), GetCaster(), GetSpellId(),
				pTarget->GetHandCardZone(), pTarget->GetExRemoveCardZone(CRole::remove_type_po_jun_rogue),
				pTarget->GetSeatId(), pTarget->GetSeatId());
		}
		else
		{
			if (!pMovAc->AddMoveCards(EmActMode::Mode_MoveTo, GetCaster(), handlist, pTarget->GetHandCardZone(), pTarget->GetExRemoveCardZone(CRole::remove_type_po_jun_rogue), seatList))
				bHaveError = true;
		}
	}

	if (bHaveError || NULL == pMovAc)
	{
		if (!bTimeOut && pMovAc != NULL)
		{
			pMovAc->SetOverMark();
		}
		return false;
	}

	/* Ϊ��������� */
	if (!GetCaster()->GetSpellStateMgr()->FindSpellState(GetSpellId())) {
		GetCaster()->GetSpellStateMgr()->AddSpellState(GetCaster(), GetSpellId());
		/* ��Ͽͻ��˲������� */
		MsgUseSpell useSpell;
		useSpell.chrId = GetFromCharacterId();
		useSpell.effectIndex = 99;
		useSpell.spellId = GetSpellId();
		useSpell.srcSeatId = GetCasterSeatId();
		useSpell.Len = sizeof(MsgUseSpell) - sizeof(useSpell.datas[0]) * (useSpell.scDataMaxCnt - useSpell.destCount - useSpell.useCardCount);
		GetGame()->Broadcast(&useSpell);
		SetUseSpellFlag(true);
		SetResolveStep(step_wait_movecard);
	}
	return true;
}

REGISTER_SPELL_STATE_CREATE(ID_RTS_SKILL_ROGUELIKE_PO_JUN, Roguelike_CPoJun_State)
Roguelike_CPoJun_State::Roguelike_CPoJun_State(CRole* pCasterRole, CRole* pOwner) :CSpellState(pCasterRole, pOwner, ID_RTS_SKILL_ROGUELIKE_PO_JUN)
{

}

bool Roguelike_CPoJun_State::CanRemove(const TRemoveParam& removeParam) const
{
	if (!GetOwer() || !GetOwer()->IsAlive()) {
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
//��� ���

Roguelike_ShenJi::Roguelike_ShenJi(const CCardSpellData *pSpellData, CGame *pGame) :CSpell(pSpellData, pGame, 0)
{
}

bool Roguelike_ShenJi::CanUse(CRole* pRole)
{
	if (!pRole || !pRole->HasCharacterSpell((unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_JI)
		|| !pRole->GetEquipCardZone())
		return false;
	return true;
}

unsigned int Roguelike_ShenJi::GetMaxTargetCnt(CRole* pRole)
{
	if (!CanUse(pRole))
		return 0;

	return 2;
}

bool Roguelike_ShenJi::IngoreUseShaTimes(CRole* pRole)
{
	if (NULL == pRole || NULL == pRole->GetSpellStateMgr() || NULL == pRole->GetGame())
	{
		return false;
	}

	if (!pRole->HasCharacterSpell(SpellId()))
	{
		return false;
	}

	return true;
}

bool Roguelike_ShenJi::BroadcastSpellOptMsg(CRole* pRole, SeatIdList& targets)
{
	if (!CanUse(pRole)
		|| targets.size() <= 1)
		return false;

	MsgUseSpell msgntf;
	msgntf.srcSeatId = pRole->GetSeatId();
	msgntf.destCount = targets.size();
	for (unsigned int i = 0; i < msgntf.destCount; i++)
		msgntf.datas[i] = targets[i];
	msgntf.spellId = (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_JI;
	msgntf.Len = sizeof(msgntf) - sizeof(msgntf.datas[0])*(msgntf.scDataMaxCnt - msgntf.destCount - msgntf.useCardCount);
	if (pRole->GetGame())
		pRole->GetGame()->Broadcast(&msgntf);

	return true;
}

//////////////////////////////////////////////////////////////////////////
//���� ���
auto ID_RTS_SKILL_ROGUELIKE_SHEN_QU = (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_QU;
REGISTER_TRIGGER_SPELL(Roguelike_ShenQu, ID_RTS_SKILL_ROGUELIKE_SHEN_QU)
Roguelike_ShenQu::Roguelike_ShenQu(const CCardSpellData *pSpellData, CGame *pGame) : CSpell(pSpellData, pGame, step_begin)
{

}

bool Roguelike_ShenQu::CanTriggerMe(CGame* pGame, CanTriggerMeParam* pParam)
{
	if (NULL == pGame || NULL == pParam)
		return false;

	CRole* pRole = pGame->GetRoleBySeatId(pParam->seat_id);
	if (NULL == pRole || !pRole->IsAlive())
		return false;

	//���޼�飺������
	if (NULL == pParam->paction)
	{
		unsigned int max_hp = 0;
		if (pRole->GetMaxHp() > 0)
			max_hp = pRole->GetMaxHp();
		if (pRole->GetHandCardSize() > max_hp)
			return false;
		return true;
	}

	CDamageAction* pDamAc = dynamic_cast<CDamageAction*>(pParam->paction);
	if (NULL == pDamAc || pDamAc->GetTarget() != pRole)
		return false;

	//�Ƿ����ʹ����
	CanCastParam castParam;
	castParam.spellid = (unsigned int)YOKA_SKILL::SKILL_TAO;
	castParam.psrc_role = pRole;
	castParam.targets.push_back(pRole->GetSeatId());
	if (CSpellMgr::single()->CanUseCardByBaseCondition(pGame, castParam) != SPELL_CAST_OK)
		return false;
	pParam->bLocked = true;
	return true;
}

eSpellCastResult Roguelike_ShenQu::CanCast(CGame* pGame, CanCastParam* pParam)
{
	return SPELL_CAST_OK;
}

void Roguelike_ShenQu::Resolve()
{
	switch (GetResolveStep())
	{
	case step_begin:
	{
		//ƥ���飺����
		if (GetCaster() != NULL && GetGame() != NULL)
		{
			if (NULL == GetSrcAction())
			{
				GetGame()->DealCard(GetCaster(), GetCaster()->GetMaxHp(), GetCaster(), GetSpellId());
				SetOverMark();
				return;
			}
			if (AskClientResponseSpell(GetCasterSeatId(), GetSpellId(), GetCasterSeatId(), sc_invalid_seat_id, 29, GetGame()->GetPlayCardWaitTime()))
			{
				SetResolveStep(step_wait_use_tao);
				return;
			}
		}
		SetOverMark();
	}
	break;
	case step_wait_use_tao:
		break;
	case step_end:
		SetOverMark();
		break;
	default:
	{
		SetOverMark();
		ClearAllOfWaitingOpt();
		APPLOG_ERROR_GAME_LOGIC("Step error, step = " << GetResolveStep() << FUN_FILE_LINE);
	}
	}
}

eSpellCastResult Roguelike_ShenQu::NetMsgCancelRpyResult(const MsgClientRoleOptRep *pMsg, CGsUser *pUser)
{
	ClearAllOfWaitingOpt();
	SetOverMark();
	return SPELL_CAST_OK;
}

eSpellCastResult Roguelike_ShenQu::NetMsgUseCardRpyResult(const MsgUseCard *pMsg, CGsUser *pUser)
{
	if (NULL == GetCaster())
		return SPELL_FAILED_UNKNOWN;

	//����ʹ����
	if (!UseCardPpy(pMsg, step_wait_use_tao, EmUseMode::mode_use_card, (unsigned int)YOKA_SKILL::SKILL_TAO
		, boost::bind(&Roguelike_ShenQu::SuccessUseCard, this), boost::bind(&Roguelike_ShenQu::FailUseCard, this), nullptr))
	{
		APPLOG_ERROR_GAME_LOGIC("UseCardPpy = false" << FUN_FILE_LINE);
		return SPELL_FAILED_UNKNOWN;
	}
	if (GetGame() != NULL)
	{
		//��Ϣ��ʹ����
		MsgRoleSpellOptRep msg;
		msg.seatId = GetCasterSeatId();
		msg.optType = MsgRoleSpellOptRep::OPP_SPELL_EFFECT;
		msg.SpellId = GetSpellId();
		msg.dataCnt = 1;
		msg.data[0] = GetTarget_Opt();
		msg.Len = sizeof(msg) - (sizeof(msg.data[0]) * (msg.sc_max_data_cnt - msg.dataCnt));
		GetGame()->Broadcast(&msg);
	}
	SetResolveStep(step_end);
	return SPELL_CAST_OK;
}

eSpellCastResult Roguelike_ShenQu::NetMsgUseSpellRpyResult(const MsgUseSpell *pMsg, CGsUser *pUser)
{
	//���ﲻ��ʹ��rpy ������srcspell ����tao cancast�жϴ���
	return SPELL_FAILED_NOT_OPT;
	if (!UseSpellRpy(pMsg, EmUseMode::mode_use_card, step_wait_use_tao, (unsigned int)YOKA_SKILL::SKILL_TAO, nullptr, CanCastParam::mark_active_use))
	{
		APPLOG_ERROR_GAME_LOGIC("UseSpellRpy = false" << FUN_FILE_LINE);
		return SPELL_FAILED_UNKNOWN;
	}

	if (GetGame() != NULL)
	{
		//��ʾ�ͻ���
		MsgRoleSpellOptRep msg;
		msg.seatId = GetCasterSeatId();
		msg.optType = MsgRoleSpellOptRep::OPP_SPELL_EFFECT;
		msg.SpellId = GetSpellId();
		msg.dataCnt = 1;
		msg.data[0] = GetTarget_Opt();
		msg.Len = sizeof(msg) - (sizeof(msg.data[0]) * (msg.sc_max_data_cnt - msg.dataCnt));
		GetGame()->Broadcast(&msg);
	}
	//SetResolveStep(step_end);//�ȴ��ص�
	return SPELL_CAST_OK;
}

void Roguelike_ShenQu::TimeOutCallBack(unsigned int seatId)
{
	ClearAllOfWaitingOpt();
	SetOverMark();
}

void Roguelike_ShenQu::SuccessUseCard()
{
	SetResolveStep(step_end);
}

void Roguelike_ShenQu::FailUseCard()
{
	SetResolveStep(step_end);
}

void Roguelike_ShenQu::CancelEffect()
{
	SetResolveStep(step_end);
}
//////////////////////////////////////////////////////////////////////////
//���� ���

auto ID_RTS_SKILL_ROGUELIKE_SHEN_WEI = (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_WEI;
REGISTER_TRIGGER_SPELL(Roguelike_ShenWei, ID_RTS_SKILL_ROGUELIKE_SHEN_WEI)
//static bool __b_spell_reg = CSpellMgr::single()->RegisterSpellGetBaseCheck(SKILL_ROGUELIKE_SHEN_WEI,boost::bind(&Roguelike_ShenWei::GetBaseCheck,_1,_2,_3,_4,_5));

Roguelike_ShenWei::Roguelike_ShenWei(const CCardSpellData *pSpellData, CGame *pGame) :CSpell(pSpellData, pGame, 0)
{
}

bool Roguelike_ShenWei::CanTriggerMe(CGame *game, CanTriggerMeParam* pparam)
{
	if (!game)
		return false;
	//ƥ���飺��ǰ�غ�
	CRole *pcurrole = game->GetPhaseMgr()->GetCurrentRole();
	if (NULL == pcurrole || pcurrole->GetSeatId() != pparam->seat_id)
		return false;

	//��ȡ������
	unsigned int count = 0;
	PRoleVct role_list;
	game->GetRoleListAll(role_list, true);
	for (auto role : role_list)
	{
		if (role == pcurrole)
			continue;
		if (!game->IsEnemy(*role, *pcurrole))
			continue;
		++count;
	}
	if (count == 0)
		return false;
	pparam->castParam[0] = count;
	return true;
}

void Roguelike_ShenWei::Resolve()
{
	if (!GetGame() || !GetGame()->GetPhaseMgr()
		|| !m_pSrc)
	{
		Log_BaseInfo(GetSpellId(), m_pSrc);
		if (!GetGame())
			APPLOG_ERROR("GetGame()=NULL" << FUN_FILE_LINE);
		else if (!GetGame()->GetPhaseMgr())
			APPLOG_ERROR("GetGame()->GetPhaseMgr()=NULL" << FUN_FILE_LINE);
		if (!m_pSrc)
			APPLOG_ERROR("m_pSrc=NULL" << FUN_FILE_LINE);
		SetOverMark();
		return;
	}
	//����������
	if (PHASE_DEAL == GetGame()->GetPhaseMgr()->GetCurrentPhase())
	{
		GetGame()->GetPhaseMgr()->SetRawDealCardCnt(GetGame()->GetPhaseMgr()->GetRawDealCardCnt() + GetSpellParam(0));
	}
	SetOverMark();
}

int Roguelike_ShenWei::GetExHandCardLimit(CRole* pRole)
{
	if (pRole != NULL && pRole->GetGame() != NULL && pRole->HasCharacterSpell((unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_SHEN_WEI))
	{
		unsigned int count = 0;
		PRoleVct role_list;
		pRole->GetGame()->GetRoleListAll(role_list, true);
		for (auto role : role_list)
		{
			if (role == pRole)
				continue;
			if (!pRole->GetGame()->IsEnemy(*role, *pRole))
				continue;
			++count;
		}
		return count;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//���� ���
auto ID_RTS_SKILL_ROGUELIKE_XIU_LUO = (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_XIU_LUO;
REGISTER_TRIGGER_SPELL(Roguelike_XiuLuo, ID_RTS_SKILL_ROGUELIKE_XIU_LUO)
Roguelike_XiuLuo::Roguelike_XiuLuo(const CCardSpellData *pSpellData, CGame *pGame) :CSpell(pSpellData, pGame, 0)
{
}

bool Roguelike_XiuLuo::CanTriggerMe(CGame *pgame, CanTriggerMeParam* pparam)
{
	if (!pgame || !pparam)
		return false;

	//�ռ�飺�ж���
	CRole* pRole = pgame->GetRoleBySeatId(pparam->seat_id);
	if (pRole != NULL
		&& pgame->GetCurrentTurnRole() == pRole
		&& !pRole->EmptyPlayCard()
		&& pRole->GetJudgeCardZone() != NULL
		&& !pRole->GetJudgeCardZone()->Empty())
	{
		return true;
	}
	return false;
}

eSpellCastResult Roguelike_XiuLuo::CanCast(CGame* pGame, CanCastParam* pParam)
{
	if (NULL == pParam || NULL == pGame)
		return SPELL_FAILED_UNKNOWN;

	//�ռ�飺�ж���
	if (NULL == pParam->psrc_role
		|| NULL == pParam->psrc_role->GetJudgeCardZone()
		|| pParam->psrc_role->GetJudgeCardZone()->Empty())
		return SPELL_FAILED_UNKNOWN;

	//���޼�飺ѡ�����б�
	if (pParam->use_cardlist.size() != 1)
		return SPELL_FAILED_USECARD_WRONG_USE_COUNT;
	CPlayCard* pCard = pParam->psrc_role->GetMyPlayCard(pParam->use_cardlist[0]);
	if (NULL == pCard || !CSpellMgr::single()->CanDiscard(*pParam->psrc_role, *pCard, (unsigned int)YOKA_SKILL::SKILL_ROGUELIKE_XIU_LUO))
		return SPELL_FAILED_USECARD_NOT_HAVE;

	// CPlayCard* playcard_judge = pParam->psrc_role->GetJudgeCardZone()->FindFirstCardBySuit(pCard->GetSuit());
	// if (NULL == playcard_judge)
	// 	return SPELL_FAILED_USECARD_WRONG_COLOR;

	return SPELL_CAST_OK;
}

void Roguelike_XiuLuo::Resolve()
{
	switch (GetResolveStep())
	{
	case step_begin:
	{
		if (NULL == GetCaster() || GetCardList().empty())
		{
			SetOverMark();
			return;
		}
		// ����һ����
		CPlayCard* pCard = GetCaster()->GetMyPlayCard(GetCardList().at(0));
		if (pCard != NULL)
		{
			// m_discard_suit = pCard->GetSuit();
			PPlayCardList card_list;
			card_list.push_back(pCard);
			if (!DisCardFromZone(GetCaster(), pCard->GetZone(), card_list))
			{
				// 	m_discard_suit = SUIT_INVALID;
				return;
			}
			SetResolveStep(step_ask_discard_judge);
			return;
		}
		SetOverMark();
	}
	break;
	case step_ask_discard_judge:
	{
		if (NULL == GetCaster()
			|| !GetCaster()->IsAlive()
			|| NULL == GetCaster()->GetJudgeCardZone()
			|| NULL == GetGame())
		{
			SetOverMark();
			return;
		}
		// �����ж���������
		PPlayCardList judge_cards;
		for (unsigned int i = 0; i < GetCaster()->GetJudgeCardZone()->Size(); i++)
		{
			CPlayCard* pJudgeCard = GetCaster()->GetJudgeCardZone()->At(i);
			if (pJudgeCard != NULL)
				judge_cards.push_back(pJudgeCard);
		}

		DisCardFromZone(GetCaster(), GetCaster()->GetJudgeCardZone(), judge_cards);
		SetResolveStep(step_end);
		return;
	}
	case step_end:
		SetOverMark();
		break;
	default:
	{
		SetOverMark();
		ClearAllOfWaitingOpt();
		APPLOG_ERROR_GAME_LOGIC("step error, step = " << GetResolveStep() << FUN_FILE_LINE);
	}
	break;
	}
}
