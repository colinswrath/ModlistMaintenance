#include "Events.h"
#include "FormLoader.h"
#include "Console.h"

namespace Events
{
    //Credit: ThirdEyeSqueegee for ModlistUpdateChecker
    void LoadEvent::RefreshFormsForUpdate(int newVersion)
    {
        logger::debug("Spinning up refresh threads");
        auto  spellItems = FormLoader::GetSingleton()->SpellItemsToRefresh;
        logger::debug("{} Spells to refresh", spellItems.size());
        auto* player     = RE::PlayerCharacter::GetSingleton();

        std::jthread([=] {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            SKSE::GetTaskInterface()->AddTask([=] {

                for (auto spell : spellItems) {
                    logger::info("Removing spell items {}", spell->GetName());

                    player->RemoveSpell(spell);
                }            
            });
        }).detach();

        //Stagger out adding spells from removing
        //For some reason with certain spells, if you remove then add too quickly it tends to cause issues
        std::jthread([=] {
            std::this_thread::sleep_for(std::chrono::seconds(12));
            SKSE::GetTaskInterface()->AddTask([=] {

                for (auto spell : spellItems) {
                    logger::info("Adding spell items {}", spell->GetName());

                    player->AddSpell(spell);
                }

                auto updateMsg = fmt::format("Winds of the North updated to version {}", insertPeriods(newVersion)).c_str();
                RE::DebugNotification(updateMsg);
            });
        }).detach();
    }

    void LoadEvent::SaveSafeUpdate(int newVersion)
    {
        RefreshFormsForUpdate(newVersion);
    }

    void LoadEvent::SaveUnsafeUpdate()
    {
        std::jthread([=] {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            SKSE::GetTaskInterface()->AddTask([=] {
                // Show message box
                std::string unsafe{
                    "Winds of the North\nThis update is savegame incompatible!\n Continuing to play on this save forfeits official support for this modlist. \n\n"
                };
                RE::DebugMessageBox(unsafe.c_str());
            });
        }).detach();     
    }

    void LoadEvent::LoadCheckVersion()
    {
        logger::debug("---Load: Checking version---");
        auto settings = Settings::GetSingleton();

        if (settings->CurrentSaveVersion > 0) {
            auto saveVersion    = settings->CurrentSaveVersion;
            auto currentVersion = settings->CurrentVersion;
            logger::debug("Current save version {}", saveVersion);
            if (saveVersion < currentVersion) {
                logger::info("Update detected. Update to {}", currentVersion);
                if (settings->LastSaveSafeVersion > saveVersion) {
                    logger::info("Save unsafe update detected! Last save safe version to current was {}", settings->LastSaveSafeVersion);
                    SaveUnsafeUpdate();
                }
                else {
                    logger::info("Save safe update. Continuing");
                    SaveSafeUpdate(settings->CurrentVersion);
                }
            }
            else {
                logger::debug("No update detected.");
            }
        }
        else {
            logger::debug("No existing save data for version");
            if (settings->UnsafeOnFirstInstall) {
                logger::info("Save unsafe on first install set");
                SaveUnsafeUpdate();
            }
            else if (settings->SafeOnFirstInstall) {
                logger::info("Update on first install. Refreshing");
                SaveSafeUpdate(settings->CurrentVersion);
            }
        }
        settings->CurrentSaveVersion = settings->CurrentVersion;
    }
}
