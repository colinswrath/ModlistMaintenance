#include "ModlistVersion.h"

using ModlistVersion = ModlistMaintenance::ModlistVersion;

class Settings
{
public:

    inline static std::string ModlistName;
    inline static bool DebugLogs;
    inline static ModlistVersion CurrentVersion;
    inline static ModlistVersion CurrentSaveVersion = { 0, 0, 0 };
    inline static ModlistVersion LastSafeVersion;
    inline static bool UnsafeOnFirstInstall;
    inline static bool SafeOnFirstInstall;

    inline static std::vector<std::pair<RE::FormID, std::string>> PlayerSpellsToRefresh;
    inline static std::vector<std::pair<RE::FormID, std::string>> PlayerPerksToRefresh;
    inline static std::vector<std::pair<RE::FormID, std::string>> PlayerQuestsToRefresh;
    inline static std::vector<std::string>                        ConsoleCommandsToRun;

    inline static std::int32_t currentStyleIndex;

    static Settings* GetSingleton()
    {
        static Settings settings;
        return &settings;
    }

    static RE::FormID ParseFormID(const std::string& str)
    {
        RE::FormID         result;
        std::istringstream ss{ str };
        ss >> std::hex >> result;
        return result;
    }

    static std::vector<std::string> split(const std::string& s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string              token;
        std::istringstream       tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    static std::string toLower(const std::string& str)
    {
        std::string result = str;
        for (char& c : result) {
            c = std::tolower(c);
        }
        return result;
    }

    static std::vector<std::string> ParseVersionString(const std::string& str)
    {
        std::vector<std::string> version;
        auto versions = split(str, '.');

        if (versions.size() < 3) {
            logger::error("Unable to parse MAJOR.MINOR.PATCH from {}", str);
        }
        return versions;
    }

    static void LoadSettings()
    {
        logger::info("Loading settings");

        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(R"(.\Data\SKSE\Plugins\ModlistMaintenance.ini)");

        DebugLogs           = ini.GetBoolValue("General", "bDebugLogs", false);
        logger::info("\tDebug log: {}", DebugLogs);
        if (DebugLogs) {
            spdlog::get("Global")->set_level(spdlog::level::level_enum::debug);
            logger::debug("Debug logging enabled");
        }

        UnsafeOnFirstInstall = ini.GetBoolValue("General", "bUnsafeOnFirstInstall", false);
        SafeOnFirstInstall = ini.GetBoolValue("General", "bUpdateOnNewInstall", true);           

        ModlistName = std::string(ini.GetValue("General", "sModlistName", "EMPTY_NAME"));
        logger::info("Modlist Name {}", ModlistName);

        auto current = ini.GetValue("General", "sCurrentVersion", 0);
        logger::info("Current version from ini {}", current);
        auto currentVersions = ParseVersionString(current);
        CurrentVersion       = ModlistVersion(currentVersions);

        logger::info("Successfuly parsed current version");

        auto last = ini.GetValue("General", "sLastSaveSafeVersion", 0);
        logger::info("Last save safe version from ini {}", last);
        LastSafeVersion = ModlistVersion(ParseVersionString(last));

        logger::info("Successfuly parsed last version");

        auto section = ini.GetSection("RefreshPlayerOnUpdate");
        if (section) {

            for (auto& [key, value] : *section) {
                auto itemInfo = Settings::split(value, '~');
                logger::info("Setting Found: Type {}: FormID {}: Filename {}", key.pItem, itemInfo[0], itemInfo[1]);
                auto formattedFormId = ParseFormID(itemInfo[0]);

                auto typeString = toLower(std::string(key.pItem));

                if (formattedFormId) {
                    if (typeString.find("spell") != std::string::npos) {
                        logger::debug("Spell {} loaded", value);
                        PlayerSpellsToRefresh.push_back(std::make_pair(formattedFormId, itemInfo[1]));
                    }
                    else if (typeString.find("quest") != std::string::npos) {
                        logger::debug("Quest {} loaded", value);

                        PlayerQuestsToRefresh.push_back(std::make_pair(formattedFormId, itemInfo[1]));
                    }
                    else if (typeString.find("perk") != std::string::npos) {
                        logger::debug("Perk {} loaded", value);

                        PlayerPerksToRefresh.push_back(std::make_pair(formattedFormId, itemInfo[1]));
                    }
                }
                else {
                    logger::info("Could not parse formID {}, please double check formatting/ID",value);
                }
            }
        }

        auto console = ini.GetSection("ConsoleCommands");

        if (console) {

            for (auto& [key, value] : *console) {
                logger::debug("Saving console command {}", value);

                ConsoleCommandsToRun.push_back(value);
            }
        }
        

        logger::info("Settings loaded");
    }

};
