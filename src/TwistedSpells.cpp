#include "ObjectMgr.h"
#include "Player.h"
#include "Item.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "SpellAuraEffects.h"
#include "TwistedSpells.h"
#include "TwistedMgr.h"
#include "MapMgr.h"
#include "DBCStores.h"

#include "Log.h"

// TreasureFind spells apply a dummy aura with a float value, which we will track in TwistedMgr.
// Used in TwistedLoot to roll on the special items table.
class TreasureFindSpell : public AuraScript
{
    PrepareAuraScript(TreasureFindSpell);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return sTwistedMgr->GetTreasureFindEnabled();
    }

    void HandleOnEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /* mode */)
    {
        const int32 Amount = aurEff->GetAmount();
        sTwistedMgr->ModifyTreasureFind(GetCasterGUID(), Amount);
        //LOG_INFO("Twisted", "Applying TreasureFind -> +{} (Base {})", Amount);
    }

    void HandleOnEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes /* mode */)
    {
        const int32 Amount = aurEff->GetAmount();
        sTwistedMgr->ModifyTreasureFind(GetCasterGUID(), -Amount);
        //LOG_INFO("Twisted", "Removing TreasureFind -> -{} (Base {})", Amount);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(TreasureFindSpell::HandleOnEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(TreasureFindSpell::HandleOnEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class ImbueItemSpell : public SpellScript
{
    PrepareSpellScript(ImbueItemSpell);

    void ApplyEnchantment(Item* Target, Player* Owner, uint32 EnchantId)
    {
        Owner->ApplyEnchantment(Target, PERM_ENCHANTMENT_SLOT, false);
        Target->SetEnchantment(PERM_ENCHANTMENT_SLOT, EnchantId, 0, 0, GetCaster()->GetGUID());
        Owner->ApplyEnchantment(Target, PERM_ENCHANTMENT_SLOT, true);
    }

    uint32 GetEnchantment(const std::vector<uint32>& EnchantList)
    {
        const uint32 EnchantIndex = urand(0, EnchantList.size());
        const uint32 EnchantId = EnchantList[EnchantIndex];
        return EnchantId;
    }

    bool UseUniqueWeaponEnchant()
    {
        return rand_chance() < sTwistedMgr->GetWeaponUniqueChance();
    }
    bool UseUniqueArmorEnchant()
    {
        return rand_chance() < sTwistedMgr->GetArmorUniqueChance();
    }
    bool UseNextTier()
    {
        return rand_chance() < sTwistedMgr->GetTierUpgradeChance();
    }

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        if (!sTwistedMgr->GetItemImbueEnabled())
            return;

        Item* TargetItem = GetHitItem();
        const Item* CasterItem = GetCastItem();
        if (!CasterItem || !TargetItem)
            return;

        Player* ItemOwner = TargetItem->GetOwner();
        if (!ItemOwner)
            return;

        int32 TierIndex = -1;
        const ItemImbueTierData* TierData = sTwistedMgr->GetItemImbueTier(CasterItem->GetEntry(), TierIndex);
        if (!TierData)
            return;

        const bool bCanUpgrade = TierIndex < sTwistedMgr->GetNumItemImbueTiers() - 1;
        const ItemTemplate* Template = CasterItem->GetTemplate();
        const std::vector<uint32>* List = &TierData->EnchantIds;
        if (Template->IsWeapon() && UseUniqueWeaponEnchant())
        {
            List = &sTwistedMgr->GetWeaponUniqueEnchantIds();
        }
        else if (!Template->IsWeapon() && UseUniqueWeaponEnchant())
        {
            List = &sTwistedMgr->GetWeaponUniqueEnchantIds();
        }
        else if (bCanUpgrade && UseNextTier())
        {
            TierData = sTwistedMgr->GetItemImbueTier(TierIndex + 1);
            List = &TierData->EnchantIds;
        }

        uint32 EnchantId = GetEnchantment(*List);
        const uint32 CurrentEnchantId = TargetItem->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);
        if (EnchantId == CurrentEnchantId)
        {
            EnchantId = (EnchantId + 1) % List->size();
        }

        SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(EnchantId);
        if (!pEnchant)
            return;

        ApplyEnchantment(TargetItem, ItemOwner, EnchantId);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(ImbueItemSpell::HandleHit, EFFECT_0, SPELL_EFFECT_ENCHANT_ITEM);
    }
};

void AddTwistedSpellScripts()
{
    RegisterSpellScript(TreasureFindSpell);
    RegisterSpellScript(ImbueItemSpell);
}
