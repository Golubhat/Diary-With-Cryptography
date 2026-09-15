#pragma once

#include "Encryption.h"
#include "BackupManager.h"

#include <wx/colour.h>
#include <wx/settings.h>

#include <cstdint>
#include <string>
#include <vector>

class UserManager
{
public:
    UserManager();

    bool Initialize();

    // Called by DiaryApp before the application exits.
    bool EncryptSettings();

    bool RegisterUser(
        const std::string& username,
        const std::string& password,
        int& newUserId);

    bool Login(
        int id,
        const std::string& username,
        const std::string& password);

    bool Logout(int userId);

    bool DeleteUser(
        int userId,
        const std::string& username,
        const std::string& password);

    int GetNumberOfUsers() const;

    bool WriteNote(
        int userId,
        const std::string& title,
        const std::string& text,
        int position);

    bool LoadNoteTitles(
        int userId,
        std::vector<std::string>& titles);

    bool ReadNote(
        int userId,
        int position,
        std::string& title,
        std::string& text);

    bool AppendToNote(
        int userId,
        int position,
        const std::string& textToAppend);

    bool RemoveNote(
        int userId,
        int position);

    bool ChangeUsernamePassword(
        int userId,
        const std::string& newUsername,
        const std::string& newPassword);

    // -------------------------------------------------
    // Form colours
    // -------------------------------------------------

    bool SetFormColours(
        std::uint8_t backgroundColour,
        std::uint8_t foregroundColour);

    std::uint8_t GetBackgroundColour() const;

    std::uint8_t GetForegroundColour() const;

    wxColour GetColour(
        std::uint8_t colour) const;

    // -------------------------------------------------
    // Backups
    // -------------------------------------------------

    bool CreateSettingsBackup();
    bool RemoveSettingsBackup();

    bool CreateUserBackup(int userId);
    bool RemoveUserBackup(int userId);

private:
    struct Settings
    {
        std::int32_t numberOfUsers = 0;
        std::uint8_t backgroundColour = 0;
        std::uint8_t foregroundColour = 1;
    };

    Settings settings{};

    Encryption::Code encryptSettings{};

    bool initialized = false;
    bool settingsEncrypted = false;

    std::string GetDataDirectory() const;
    std::string GetSettingsFile() const;
    std::string GetEncryptionCodeFile() const;
    std::string GetUserFile(int id) const;

    bool CreateDataDirectory();

    bool CreateEncryptionCodeFile();
    bool LoadEncryptionCode();

    bool CreateSettingsFile();
    bool LoadSettings();

    bool FindUserEncryptionCode(
        int id,
        Encryption::Code& encryptionCode) const;

    bool AppendUserToSettings(
        int id,
        const Encryption::Code& encryptionCode);

    bool UserIdExists(int id) const;
};