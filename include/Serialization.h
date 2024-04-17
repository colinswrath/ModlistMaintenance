#pragma once

namespace Serialization
{
    static constexpr std::uint32_t SerializationVersion = 1;
    static constexpr std::uint32_t ID                   = 'WOTN';
    static constexpr std::uint32_t SerializationType    = 'WTNV';

    inline void SaveCallback(SKSE::SerializationInterface* a_skse)
    {
        if (!a_skse->OpenRecord(SerializationType, SerializationVersion)) {
            logger::error("Failed to open winds of the north version record");
            return;
        }
        else {
            auto version = Settings::GetSingleton()->CurrentVersion;

            if (!a_skse->WriteRecordData(version)) {
                logger::error("Failed to write size of record data");
                return;
            }
            else {
                logger::info(FMT_STRING("Serialized version: {}"), std::to_string(version));
            }
        }
    }

    inline void LoadCallback(SKSE::SerializationInterface* a_skse)
    {
        logger::debug("Load callback");
        std::uint32_t type;
        std::uint32_t version;
        std::uint32_t length;
        a_skse->GetNextRecordInfo(type, version, length);

        if (type != SerializationType) {
            logger::debug("No save data found");
            return;
        }

        if (version != SerializationVersion) {
            logger::error("Unable to load data");
            return;
        }

        int deserializedVal;
        if (!a_skse->ReadRecordData(deserializedVal)) {
            logger::error("Failed to load size");
            return;
        }
        else {
            Settings::GetSingleton()->CurrentSaveVersion = deserializedVal;
            logger::info(FMT_STRING("Deserialized: {}"), std::to_string(deserializedVal));
        }
    }

    inline void RevertCallback([[maybe_unused]] SKSE::SerializationInterface* a_skse)
    {
        logger::info("Revert");
        auto settings                = Settings::GetSingleton();
        settings->CurrentSaveVersion = 0;
    }
} // namespace Serialization
