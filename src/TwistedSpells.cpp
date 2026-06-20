#include "ObjectMgr.h"
#include "Player.h"
#include "Item.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "SpellAuraEffects.h"
#include "TwistedSpells.h"
#include "TwistedMgr.h"
#include "TownPortal.h"
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
        return ValidateSpellInfo({
            SPELL_TREASURE_FIND_PCT_1,
            SPELL_TREASURE_FIND_PCT_5,
            SPELL_TREASURE_FIND_PCT_10,
            SPELL_TREASURE_FIND_BUFF_1,
            SPELL_TREASURE_FIND_BUFF_5,
            SPELL_TREASURE_FIND_BUFF_10,
        });
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

class TownPortalSpell : public SpellScript
{
    PrepareSpellScript(TownPortalSpell);

    void HandleOnCast()
    {
        Player* player = GetCaster()->ToPlayer();
        if (!player) return;

        // We've come to an impasse, the homebind is private information in Player.
        // If we want to continue without engine modifications we will need to rethink the town portal situation.
        // We could instead create two items:
        //      Brittle Hearthstone - sets your field pos then returns you to homebind, consumes 1 quantity
        //      Brittle Skipstone - returns you to the field pos, consumes 1 quantity

        if (GameObjectTemplate const* info = sObjectMgr->GetGameObjectTemplate(GO_TOWN_PORTAL))
        {
            GameObject* pObj = sTwistedMgr->ObtainTownPortal(info, player);
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(TownPortalSpell::HandleOnCast);
    }
};

class ImbueItemSpell : public SpellScript
{
    PrepareSpellScript(ImbueItemSpell);

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Item* TargetItem = GetHitItem();

        // pick the EnchantId from some configurable tables
        const std::vector<uint32>& EnchantIds = sTwistedMgr->GetItemImbueEnchantments();
        uint32 Idx = urand(0, EnchantIds.size());
        uint32 enchant_id = EnchantIds[Idx];
        if (!enchant_id)
            return;

        SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!pEnchant)
            return;

        // item can be in trade slot and have owner diff. from caster
        Player* ItemOwner = TargetItem->GetOwner();
        if (!ItemOwner)
            return;

        // remove old enchanting before applying new if equipped
        ItemOwner->ApplyEnchantment(TargetItem, PERM_ENCHANTMENT_SLOT, false);

        TargetItem->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant_id, 0, 0, GetCaster()->GetGUID());

        // add new enchanting if equipped
        ItemOwner->ApplyEnchantment(TargetItem, PERM_ENCHANTMENT_SLOT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(ImbueItemSpell::HandleHit, EFFECT_0, SPELL_EFFECT_ENCHANT_ITEM);
    }
};

void AddTwistedSpellScripts()
{
    RegisterSpellScript(TreasureFindSpell);
    RegisterSpellScript(TownPortalSpell);
    RegisterSpellScript(ImbueItemSpell);
}
