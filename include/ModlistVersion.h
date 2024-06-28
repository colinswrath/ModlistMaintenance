#pragma once

#include <iostream>

namespace ModlistMaintenance
{
    class ModlistVersion
    {
    public:
        int Major;
        int Minor;
        int Patch;

        // Constructor
        ModlistVersion(int major = 0, int minor = 0, int patch = 0) : Major(major), Minor(minor), Patch(patch) {}

        ModlistVersion(const std::vector<std::string>& versionVec)
        {
            if (versionVec.size() != 3) {
                throw std::invalid_argument("Version vector must contain exactly three elements.");
            }
            Major = std::stoi(versionVec[0]);
            Minor = std::stoi(versionVec[1]);
            Patch = std::stoi(versionVec[2]);
        }

        // Overload the '=' operator
        ModlistVersion& operator=(const ModlistVersion& other)
        {
            if (this != &other) {
                Major = other.Major;
                Minor = other.Minor;
                Patch = other.Patch;
            }
            return *this;
        }

        bool operator>(const ModlistVersion& other) const
        {
            if (Major > other.Major)
                return true;
            if (Major < other.Major)
                return false;
            if (Minor > other.Minor)
                return true;
            if (Minor < other.Minor)
                return false;
            return Patch > other.Patch;
        }

        bool operator<(const ModlistVersion& other) const
        {
            if (Major < other.Major)
                return true;
            if (Major > other.Major)
                return false;
            if (Minor < other.Minor)
                return true;
            if (Minor > other.Minor)
                return false;
            return Patch < other.Patch;
        }

        // Overload the '==' operator
        bool operator==(const ModlistVersion& other) const { return Major == other.Major && Minor == other.Minor && Patch == other.Patch; }

        std::string getVersionAsString() const
        {
            std::ostringstream oss;
            oss << Major << '.' << Minor << '.' << Patch;
            return oss.str();
        }

        std::vector<int> getVersionAsVector() const
        {
            return { Major, Minor, Patch };
        }

        bool IsEmptyVersion() const
        {
            if (Major == 0 && Minor == 0 && Patch == 0)
            {
                return true;
            }
            else {
                return false;
            }
        }
    };
}
