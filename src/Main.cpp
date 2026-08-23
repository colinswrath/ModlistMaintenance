
#include "FormLoader.h"
#include "Serialization.h"
#include "Events.h"

void InitListener(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
        Settings::GetSingleton()->LoadSettings();
        FormLoader::GetSingleton()->LoadAllForms();
		break;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SKSE::Init(skse);
    logger::init();

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto version{ plugin->GetVersion() };
    auto runtimcompat = plugin->GetRuntimeCompatibility();

    logger::info("{} {} loading...", plugin->GetName(), version);

    #ifdef SKYRIM_SUPPORT_AE
        logger::info("Post 1130 build is active.");
    #endif // SKYRIM_SUPPORT_AE

    auto messaging = SKSE::GetMessagingInterface();
    if (!messaging->RegisterListener(InitListener)) {
        return false;
    }

    if (auto serialization = SKSE::GetSerializationInterface()) {
        serialization->SetUniqueID(Serialization::ID);
        serialization->SetSaveCallback(&Serialization::SaveCallback);
        serialization->SetLoadCallback(&Serialization::LoadCallback);
        serialization->SetRevertCallback(&Serialization::RevertCallback);
    }

    logger::info("ModlistMaintenance loaded successfully");
    return true;
}
