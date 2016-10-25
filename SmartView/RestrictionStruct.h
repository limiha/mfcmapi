#pragma once
#include "SmartViewParser.h"

class RestrictionStruct : public SmartViewParser
{
public:
	RestrictionStruct();
	~RestrictionStruct();

	void Init(bool bRuleCondition, bool bExtended);
private:
	void Parse() override;
	_Check_return_ wstring ToStringInternal() override;

	// Caller allocates with new. Clean up with DeleteRestriction and delete[].
	bool BinToRestriction(ULONG ulDepth, _In_ LPSRestriction psrRestriction, bool bRuleCondition, bool bExtendedCount);
	// Neuters an SRestriction - caller must use delete to delete the SRestriction
	static void DeleteRestriction(_In_ LPSRestriction lpRes);

	bool m_bRuleCondition;
	bool m_bExtended;
	LPSRestriction m_lpRes;
};