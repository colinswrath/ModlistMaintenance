#pragma once

using ModlistVersion = ModlistMaintenance::ModlistVersion;

namespace Serialization
{
    static constexpr std::uint32_t SerializationVersion = 2;
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
            auto vector = version.getVersionAsVector();

            logger::info("Serializing {}.{}.{}", vector[0], vector[1], vector[2]);

            if (!a_skse->WriteRecordData(vector.size())) {
                logger::error("Failed to write size of record data");
                return;
            }
            else {
                for (auto& elem : vector) {
                    if (!a_skse->WriteRecordData(elem)) {
                        logger::error("Failed to write data for warmth elem");
                        return;
                    }
                    logger::info(FMT_STRING("Serialize: {}"), std::to_string(elem));
                }
                logger::info(FMT_STRING("Serialized version: {}"), version.getVersionAsString());
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

        std::vector<int> deserializedVector;
        std::size_t size;
        if (!a_skse->ReadRecordData(size)) {
            logger::error("Failed to load size");
            return;
        }

        logger::info("Size {}", std::to_string(size));

        for (std::size_t i = 0; i < size; ++i) {
            int elem;
            if (!a_skse->ReadRecordData(elem)) {
                logger::error("Failed to load setting element");
                return;
            }
            else {
                logger::info(FMT_STRING("Deserialized: {}"), std::to_string(elem));
                deserializedVector.push_back(elem);
            }
        }

        auto settings                = Settings::GetSingleton();
        settings->CurrentSaveVersion = ModlistVersion(deserializedVector[0], deserializedVector[1], deserializedVector[2]);
        logger::info(FMT_STRING("Deserialized: {}"), settings->CurrentSaveVersion.getVersionAsString());
    }

    inline void RevertCallback([[maybe_unused]] SKSE::SerializationInterface* a_skse)
    {
        logger::info("Revert");
        auto settings                = Settings::GetSingleton();
        settings->CurrentSaveVersion = {0,0,0};
    }
} // namespace Serialization
