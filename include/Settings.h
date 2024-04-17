class Settings
{
public:

    inline static std::string ModlistName;
    inline static bool DebugLogs;
    inline static int CurrentVersion;
    inline static int CurrentSaveVersion = 0;
    inline static int LastSaveSafeVersion;
    inline static bool UnsafeOnFirstInstall;
    inline static bool SafeOnFirstInstall;

    inline static std::vector<std::pair<RE::FormID,std::string>> PlayerSpellsToRefresh;

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
        CurrentVersion = ini.GetLongValue("General", "iCurrentVersion", 0);
        logger::info("Current version from ini {}", CurrentVersion);
        LastSaveSafeVersion = ini.GetLongValue("General", "iLastSaveSafeVersion", 0);
        logger::info("Last save safe version from ini {}", LastSaveSafeVersion);

        auto section = ini.GetSection("RefreshPlayerOnUpdate");
        if (section) {

            for (auto& [key, value] : *section) {
                logger::info("Loaded {}", key.pItem);

                auto itemInfo = Settings::split(value, '~');
                logger::info("FormID {}: Filename {}", itemInfo[0], itemInfo[1]);
                auto formattedFormId = ParseFormID(itemInfo[0]);
                PlayerSpellsToRefresh.push_back(std::make_pair(formattedFormId, itemInfo[1]));
            }
        }
        

        logger::info("Settings loaded");
    }

};
