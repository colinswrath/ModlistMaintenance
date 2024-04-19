#pragma once

#include "Settings.h"

class FormLoader
{
public:

    inline static std::vector<RE::SpellItem*>   SpellItemsToRefresh;
    inline static std::vector<RE::BGSPerk*>     PerkItemsToRefresh;
    inline static std::vector<RE::TESQuest*>    QuestItemsToRefresh;
    inline static std::vector<std::string>    ConsoleCommandsToRun;

    static FormLoader* GetSingleton()
    {
        static FormLoader formLoader;
        return &formLoader;
    }

    template <typename T>
    T* LoadFormPointer(RE::FormID formId, std::string filename)
    {
        if (formId) {
            auto form = RE::TESDataHandler::GetSingleton()->LookupForm(formId, filename)->As<T>();
            return form;
        }
        else {
            return nullptr;
        }
    }

    void LoadAllForms()
    {
        auto* settings = Settings::GetSingleton();
        logger::info("Loading forms");

        for (const auto& pair: settings->PlayerSpellsToRefresh) {
            logger::info("Loading spell forms");
            auto* spell = LoadFormPointer<RE::SpellItem>(pair.first, pair.second);
            if (spell) {
                logger::info("Spell form loaded: {}",spell->GetName());
                SpellItemsToRefresh.push_back(spell);
            }
            else {
                logger::info("Could not load spell at FormId {}",pair.first);
            }
        }

        for (const auto& pair : settings->PlayerPerksToRefresh) {
            logger::info("Loading perks forms");
            auto* perk = LoadFormPointer<RE::BGSPerk>(pair.first, pair.second);
            if (perk) {
                logger::info("Perk form loaded: {}", perk->GetName());
                PerkItemsToRefresh.push_back(perk);
            }
            else {
                logger::info("Could not load perk at FormId {}", pair.first);
            }
        }

        for (const auto& pair : settings->PlayerQuestsToRefresh) {
            logger::info("Loading quest forms");
            auto* quest = LoadFormPointer<RE::TESQuest>(pair.first, pair.second);
            if (quest) {
                logger::info("quest form loaded: {}", quest->GetName());
                QuestItemsToRefresh.push_back(quest);
            }
            else {
                logger::info("Could not load quest at FormId {}", pair.first);
            }
        }

        ConsoleCommandsToRun = settings->ConsoleCommandsToRun;

        logger::info("Forms loaded");
    }
};
