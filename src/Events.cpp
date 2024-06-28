#include "Events.h"
#include "FormLoader.h"
#include "Console.h"

using ModlistVersion = ModlistMaintenance::ModlistVersion;

namespace Events
{
    void LoadEvent::RefreshFormsForUpdate()
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
                auto settings = Settings::GetSingleton();
                auto updateMsg = fmt::format("Updating {} to version {}", settings->ModlistName, settings->CurrentVersion.getVersionAsString()).c_str();
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
                auto settings = Settings::GetSingleton();

                auto updateMsg = fmt::format("{} updated to version {}", settings->ModlistName, settings->CurrentVersion.getVersionAsString()).c_str();
                RE::DebugNotification(updateMsg);
            });

        }).detach();
    }

    void LoadEvent::SaveSafeUpdate()
    {
        RefreshFormsForUpdate();
    }

    void LoadEvent::SaveUnsafeUpdate()
    {
        std::jthread([=] {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            SKSE::GetTaskInterface()->AddTask([=] {
                auto settings = Settings::GetSingleton();
                // Show message box
                std::string unsafe{
                    settings->ModlistName + "\nThis update is savegame incompatible!\n Continuing to play on this save forfeits official support for this modlist. \n\n"
                };
                RE::DebugMessageBox(unsafe.c_str());
            });
        }).detach();     
    }

    void LoadEvent::LoadCheckVersion()
    {
        logger::debug("---Load: Checking version---");
        auto settings = Settings::GetSingleton();

        if (!settings->CurrentSaveVersion.IsEmptyVersion()) {
            auto saveVersion    = settings->CurrentSaveVersion;
            auto currentVersion = settings->CurrentVersion;
            logger::debug("Current save version {}", saveVersion.getVersionAsString());
            if (saveVersion < currentVersion) {
                logger::info("Update detected. Update to {}", currentVersion.getVersionAsString());
                if (settings->LastSafeVersion > saveVersion) {
                    logger::info("Save unsafe update detected! Last save safe version to current was {}", settings->LastSafeVersion.getVersionAsString());
                    SaveUnsafeUpdate();
                }
                else {
                    logger::info("Save safe update. Continuing");
                    SaveSafeUpdate();
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
                SaveSafeUpdate();
            }
        }
        settings->CurrentSaveVersion = settings->CurrentVersion;
    }
}
