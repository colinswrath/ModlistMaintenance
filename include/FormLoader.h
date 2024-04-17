#pragma once

#include "Settings.h"

class FormLoader
{
public:

    inline static std::vector<RE::SpellItem*> SpellItemsToRefresh;

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
            auto* spell = LoadFormPointer<RE::SpellItem>(pair.first, pair.second);
            if (spell) {
                SpellItemsToRefresh.push_back(spell);
            }
        }

        logger::info("Forms loaded");
    }
};
