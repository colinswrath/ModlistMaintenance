#include "Events.h"
#include "FormLoader.h"
#include "Console.h"

namespace Events
{
    void LoadEvent::RefreshFormsForUpdate(int newVersion)
    {
        logger::debug("Spinning up refresh threads");
        auto spellItems = FormLoader::GetSingleton()->SpellItemsToRefresh;
        auto perkItems  = FormLoader::GetSingleton()->PerkItemsToRefresh;
        auto questItems = FormLoader::GetSingleton()->QuestItemsToRefresh;
        auto consoleCommands = FormLoader::GetSingleton()->ConsoleCommandsToRun;
        auto* player     = RE::PlayerCharacter::GetSingleton();

        std::jthread([=] {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            SKSE::GetTaskInterface()->AddTask([=] {
                auto updateMsg = fmt::format("Updating Winds of the North to version {}", insertPeriods(newVersion)).c_str();
                RE::DebugNotification(updateMsg);

                for (auto spell : spellItems) {
                    logger::info("Removing spell item {}", spell->GetName());

                    player->RemoveSpell(spell);
                }

                for (auto perk : perkItems) {
                    logger::info("Removing perk item {}", perk->GetName());

                    player->RemovePerk(perk);
                }

                for (auto quest : questItems) {
                    logger::info("Stopping quest item {}", quest->GetName());

                    if (quest->IsRunning()) {
                        quest->Stop();
                    }
                }

                for (auto command : consoleCommands) {
                    logger::info("Running console command {}", command);
                    Console::ExecuteCommand(command);
                }
            });

            //Wait briefly before processing adds
            std::this_thread::sleep_for(std::chrono::seconds(5));
            SKSE::GetTaskInterface()->AddTask([=] {
                for (auto spell : spellItems) {
                    logger::info("Adding spell item {}", spell->GetName());

                    player->AddSpell(spell);
                }

                for (auto perk : perkItems) {
                    logger::info("Adding perk item {}", perk->GetName());

                    player->AddPerk(perk);
                }

                for (auto quest : questItems) {
                    logger::info("Starting quest item {}", quest->GetName());

                    quest->Start();
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
