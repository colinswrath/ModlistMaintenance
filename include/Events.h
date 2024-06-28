#pragma once

namespace Events
{
    class LoadEvent
    {
    public:
        static LoadEvent* GetSingleton()
        {
            static LoadEvent load;
            return &load;
        }

        void                     RefreshFormsForUpdate();
        void                     SaveSafeUpdate();
        void                     SaveUnsafeUpdate();
        void                     LoadCheckVersion();

        std::string intToHexString(int value)
        {
            std::stringstream stream;
            stream << std::hex << value; // Convert integer to hexadecimal string
            return stream.str();         // Return the string representation
        }

        std::string insertPeriods(int number)
        {
            std::string numString = std::to_string(number);

            for (size_t i = 1; i < numString.size(); i += 2) {
                numString.insert(i, ".");
            }

            return numString;
        }
    };
} // namespace Events
