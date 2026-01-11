#include"CSGameRules.hpp"

void C_CSGameRules::Update() {
	std::unique_lock lock(this->GameRulesMtx);
	if (!this->Address)return;
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bFreezePeriod, this->m_bFreezePeriod);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bWarmupPeriod, this->m_bWarmupPeriod);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_fWarmupPeriodEnd, this->m_fWarmupPeriodEnd);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_fWarmupPeriodStart, this->m_fWarmupPeriodStart);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bTerroristTimeOutActive, this->m_bTerroristTimeOutActive);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bCTTimeOutActive, this->m_bCTTimeOutActive);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flTerroristTimeOutRemaining, this->m_flTerroristTimeOutRemaining);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flCTTimeOutRemaining, this->m_flCTTimeOutRemaining);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nTerroristTimeOuts, this->m_nTerroristTimeOuts);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nCTTimeOuts, this->m_nCTTimeOuts);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bTechnicalTimeOut, this->m_bTechnicalTimeOut);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bMatchWaitingForResume, this->m_bMatchWaitingForResume);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundTime, this->m_iRoundTime);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_fMatchStartTime, this->m_fMatchStartTime);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_fRoundStartTime, this->m_fRoundStartTime);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flRestartRoundTime, this->m_flRestartRoundTime);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bGameRestart, this->m_bGameRestart);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flGameStartTime, this->m_flGameStartTime);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_timeUntilNextPhaseStarts, this->m_timeUntilNextPhaseStarts);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_gamePhase, this->m_gamePhase);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_totalRoundsPlayed, this->m_totalRoundsPlayed);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nRoundsPlayedThisPhase, this->m_nRoundsPlayedThisPhase);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nOvertimePlaying, this->m_nOvertimePlaying);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iHostagesRemaining, this->m_iHostagesRemaining);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bAnyHostageReached, this->m_bAnyHostageReached);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bMapHasBombTarget, this->m_bMapHasBombTarget);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bMapHasRescueZone, this->m_bMapHasRescueZone);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bMapHasBuyZone, this->m_bMapHasBuyZone);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bIsQueuedMatchmaking, this->m_bIsQueuedMatchmaking);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nQueuedMatchmakingMode, this->m_nQueuedMatchmakingMode);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bIsValveDS, this->m_bIsValveDS);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bLogoMap, this->m_bLogoMap);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bPlayAllStepSoundsOnServer, this->m_bPlayAllStepSoundsOnServer);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iSpectatorSlotCount, this->m_iSpectatorSlotCount);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_MatchDevice, this->m_MatchDevice);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bHasMatchStarted, this->m_bHasMatchStarted);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nNextMapInMapgroup, this->m_nNextMapInMapgroup);
	// 字符串类型需要特殊处理（此处省略具体实现）
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nTournamentPredictionsPct, this->m_nTournamentPredictionsPct);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flCMMItemDropRevealStartTime, this->m_flCMMItemDropRevealStartTime);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flCMMItemDropRevealEndTime, this->m_flCMMItemDropRevealEndTime);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bIsDroppingItems, this->m_bIsDroppingItems);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bIsQuestEligible, this->m_bIsQuestEligible);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bIsHltvActive, this->m_bIsHltvActive);
	// 数组类型需要循环读取（此处省略具体实现）
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_numBestOfMaps, this->m_numBestOfMaps);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nHalloweenMaskListSeed, this->m_nHalloweenMaskListSeed);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bBombDropped, this->m_bBombDropped);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bBombPlanted, this->m_bBombPlanted);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundWinStatus, this->m_iRoundWinStatus);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_eRoundWinReason, this->m_eRoundWinReason);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bTCantBuy, this->m_bTCantBuy);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bCTCantBuy, this->m_bCTCantBuy);
	// 数组类型需要循环读取（此处省略具体实现）
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundEndTimerTime, this->m_iRoundEndTimerTime);
	// 字符串和特殊类型需要特殊处理（此处省略具体实现）
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundEndPlayerCount, this->m_iRoundEndPlayerCount);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_bRoundEndNoMusic, this->m_bRoundEndNoMusic);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundEndLegacy, this->m_iRoundEndLegacy);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nRoundEndCount, this->m_nRoundEndCount);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_iRoundStartRoundNumber, this->m_iRoundStartRoundNumber);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_nRoundStartCount, this->m_nRoundStartCount);
	MulNXB::Memory::Read(this->Address + cs2_dumper::schemas::client_dll::C_CSGameRules::m_flLastPerfSampleTime, this->m_flLastPerfSampleTime);

	return;
}