#include "UserManager.h"
#include "BackupManager.h"

#include <wx/wx.h>
#include <wx/stdpaths.h>

#include <filesystem>
#include <fstream>
#include <random>

namespace
{
    constexpr int MIN_USER_ID = 10000;
    constexpr int MAX_USER_ID = 99999;

    constexpr std::size_t CODE_SIZE = 256;
}

UserManager::UserManager()
{
}

// ------------------------------------------------------------
// Paths
// ------------------------------------------------------------

std::string UserManager::GetDataDirectory() const
{
    wxString documentsPath =
        wxStandardPaths::Get().GetDocumentsDir();

    wxString diaryPath =
        documentsPath + wxFILE_SEP_PATH + "Diary";

    return diaryPath.ToStdString();
}

std::string UserManager::GetSettingsFile() const
{
    return GetDataDirectory() +
        std::string(1, std::filesystem::path::preferred_separator) +
        "settings.dat";
}

std::string UserManager::GetEncryptionCodeFile() const
{
    return GetDataDirectory() +
        std::string(1, std::filesystem::path::preferred_separator) +
        "Encryption Code.dat";
}

std::string UserManager::GetUserFile(int id) const
{
    return GetDataDirectory() +
        std::string(1, std::filesystem::path::preferred_separator) +
        std::to_string(id) +
        ".dat";
}

// ------------------------------------------------------------
// Directory
// ------------------------------------------------------------

bool UserManager::CreateDataDirectory()
{
    try
    {
        std::filesystem::create_directories(
            GetDataDirectory());

        return true;
    }
    catch (...)
    {
        return false;
    }
}

// ------------------------------------------------------------
// Initialization
// ------------------------------------------------------------

bool UserManager::Initialize()
{
    if (initialized)
        return true;

    if (!CreateDataDirectory())
        return false;

    // --------------------------------------------------------
    // Encryption Code.dat
    // --------------------------------------------------------

    const std::string encryptionCodeFile =
        GetEncryptionCodeFile();

    if (!std::filesystem::exists(
        encryptionCodeFile))
    {
        const int answer =
            wxMessageBox(
                "Encryption Code.dat was not found.\n\n"
                "A new encryption code must be created "
                "before Diary can start.\n\n"
                "Do you want to create a new encryption code?",
                "Encryption Code",
                wxYES_NO | wxICON_WARNING);
        // WARNING: Existing encrypted Diary data may no longer be accessible if the original encryption code was deleted.

        if (answer != wxYES)
        {
            // User chose No.
            // Initialize() fails, so the application
            // will exit.
            return false;
        }

        if (!CreateEncryptionCodeFile())
        {
            wxMessageBox(
                "Unable to create Encryption Code.dat.",
                "Encryption Code",
                wxOK | wxICON_ERROR);

            return false;
        }
    }

    if (!LoadEncryptionCode())
    {
        wxMessageBox(
            "Unable to load Encryption Code.dat.",
            "Encryption Code",
            wxOK | wxICON_ERROR);

        return false;
    }

    // --------------------------------------------------------
    // settings.dat
    // --------------------------------------------------------

    if (!std::filesystem::exists(
        GetSettingsFile()))
    {
        // CreateSettingsFile() creates settings.dat
        // in encrypted form.
        if (!CreateSettingsFile())
            return false;

        // settings.dat is currently encrypted.
        //
        // Make its backup BEFORE decrypting it.
        //
        // CreateSettingsBackup() requires initialized == true,
        // so set it temporarily after all initialization
        // prerequisites are ready.
        initialized = true;

        if (!CreateSettingsBackup())
        {
            initialized = false;
            return false;
        }

        initialized = false;

        // Now decrypt settings.dat for runtime use.
        if (!Encryption::TransformFile(
            GetSettingsFile(),
            false,
            encryptSettings))
        {
            return false;
        }

        settingsEncrypted = false;
    }
    else
    {
        // settings.dat was encrypted when the previous
        // application session ended.
        //
        // Make the backup FIRST.
        initialized = true;

        if (!CreateSettingsBackup())
        {
            initialized = false;
            return false;
        }

        initialized = false;

        // Now decrypt settings.dat for runtime use.
        if (!Encryption::TransformFile(
            GetSettingsFile(),
            false,
            encryptSettings))
        {
            return false;
        }

        settingsEncrypted = false;

        if (!LoadSettings())
            return false;
    }

    initialized = true;

    return true;
}

bool UserManager::CreateSettingsBackup()
{
    if (!initialized)
        return false;

    return BackupManager::BackUp(
        GetSettingsFile(),
        GetDataDirectory() +
        std::string(1,
            std::filesystem::path::preferred_separator) +
        "Backup",
        BackupManager::Decision::
        MAKE_THE_BACKUP_FILE,
        encryptSettings);
}

bool UserManager::RemoveSettingsBackup()
{
    return BackupManager::BackUp(
        GetSettingsFile(),
        GetDataDirectory() +
        std::string(1,
            std::filesystem::path::preferred_separator) +
        "Backup",
        BackupManager::Decision::
        REMOVE_THE_BACKUP_FILE,
        encryptSettings);
}

// ------------------------------------------------------------
// Encryption Code.dat
// ------------------------------------------------------------

bool UserManager::CreateEncryptionCodeFile()
{
    Encryption::Code code{};

    if (!Encryption::GenerateCode(code))
        return false;

    std::ofstream file(
        GetEncryptionCodeFile(),
        std::ios::binary |
        std::ios::trunc);

    if (!file)
        return false;

    file.write(
        reinterpret_cast<const char*>(code.data()),
        static_cast<std::streamsize>(code.size()));

    return file.good();
}

bool UserManager::LoadEncryptionCode()
{
    return Encryption::ReadCodeFile(
        GetEncryptionCodeFile(),
        encryptSettings);
}

// ------------------------------------------------------------
// settings.dat
// ------------------------------------------------------------

bool UserManager::CreateSettingsFile()
{
    settings.numberOfUsers = 0;
    settings.backgroundColour = 0;
    settings.foregroundColour = 1;

    std::ofstream file(
        GetSettingsFile(),
        std::ios::binary |
        std::ios::trunc);

    if (!file)
        return false;

    file.write(
        reinterpret_cast<const char*>(
            &settings.numberOfUsers),
        sizeof(settings.numberOfUsers));

    file.write(
        reinterpret_cast<const char*>(
            &settings.backgroundColour),
        sizeof(settings.backgroundColour));

    file.write(
        reinterpret_cast<const char*>(
            &settings.foregroundColour),
        sizeof(settings.foregroundColour));

    if (!file.good())
        return false;

    file.close();

    // Store settings.dat encrypted while the application
    // is not running.
    return Encryption::TransformFile(
        GetSettingsFile(),
        true,
        encryptSettings);
}

bool UserManager::LoadSettings()
{
    std::ifstream file(
        GetSettingsFile(),
        std::ios::binary);

    if (!file)
        return false;

    file.read(
        reinterpret_cast<char*>(
            &settings.numberOfUsers),
        sizeof(settings.numberOfUsers));

    file.read(
        reinterpret_cast<char*>(
            &settings.backgroundColour),
        sizeof(settings.backgroundColour));

    file.read(
        reinterpret_cast<char*>(
            &settings.foregroundColour),
        sizeof(settings.foregroundColour));

    if (!file.good())
        return false;

    if (settings.numberOfUsers < 0 ||
        settings.numberOfUsers > 1000000)
    {
        return false;
    }

    return true;
}

// ------------------------------------------------------------
// Encrypt settings before application exit
// ------------------------------------------------------------

bool UserManager::EncryptSettings()
{
    if (!initialized)
        return true;

    if (settingsEncrypted)
        return true;

    if (!Encryption::TransformFile(
        GetSettingsFile(),
        true,
        encryptSettings))
    {
        return false;
    }

    settingsEncrypted = true;

    return true;
}

// ------------------------------------------------------------
// User lookup
// ------------------------------------------------------------

bool UserManager::UserIdExists(int id) const
{
    return std::filesystem::exists(
        GetUserFile(id));
}

bool UserManager::FindUserEncryptionCode(
    int id,
    Encryption::Code& encryptionCode) const
{
    // settings.dat MUST be decrypted while the application
    // is running.

    std::ifstream file(
        GetSettingsFile(),
        std::ios::binary);

    if (!file)
        return false;

    std::int32_t numberOfUsers;
    std::uint8_t backgroundColour;
    std::uint8_t foregroundColour;

    file.read(
        reinterpret_cast<char*>(&numberOfUsers),
        sizeof(numberOfUsers));

    file.read(
        reinterpret_cast<char*>(&backgroundColour),
        sizeof(backgroundColour));

    file.read(
        reinterpret_cast<char*>(&foregroundColour),
        sizeof(foregroundColour));

    if (!file ||
        numberOfUsers < 0 ||
        numberOfUsers > 1000000)
    {
        return false;
    }

    for (std::int32_t i = 0;
        i < numberOfUsers;
        ++i)
    {
        std::int32_t storedId = 0;

        file.read(
            reinterpret_cast<char*>(&storedId),
            sizeof(storedId));

        if (!file)
            return false;

        Encryption::Code storedCode{};

        file.read(
            reinterpret_cast<char*>(
                storedCode.data()),
            static_cast<std::streamsize>(
                storedCode.size()));

        if (!file)
            return false;

        if (storedId == id)
        {
            encryptionCode = storedCode;
            return true;
        }
    }

    return false;
}

// ------------------------------------------------------------
// Append user to settings.dat
// ------------------------------------------------------------

bool UserManager::AppendUserToSettings(
    int id,
    const Encryption::Code& encryptionCode)
{
    // settings.dat is already decrypted here.

    std::fstream file(
        GetSettingsFile(),
        std::ios::binary |
        std::ios::in |
        std::ios::out);

    if (!file)
        return false;

    settings.numberOfUsers++;

    // Update number of users at the beginning.
    file.seekp(0, std::ios::beg);

    file.write(
        reinterpret_cast<const char*>(
            &settings.numberOfUsers),
        sizeof(settings.numberOfUsers));

    // Go to the end and append the new user.
    file.seekp(0, std::ios::end);

    file.write(
        reinterpret_cast<const char*>(&id),
        sizeof(id));

    file.write(
        reinterpret_cast<const char*>(
            encryptionCode.data()),
        static_cast<std::streamsize>(
            encryptionCode.size()));

    return file.good();
}

// ------------------------------------------------------------
// Registration
// ------------------------------------------------------------

bool UserManager::RegisterUser(
    const std::string& username,
    const std::string& password,
    int& newUserId)
{
    if (!initialized)
        return false;

    if (username.empty() ||
        password.empty())
    {
        return false;
    }

    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_int_distribution<int> distribution(
        MIN_USER_ID,
        MAX_USER_ID);

    int id = 0;

    do
    {
        id = distribution(generator);
    } while (UserIdExists(id));

    Encryption::Code userEncryptionCode{};

    if (!Encryption::GenerateCode(
        userEncryptionCode))
    {
        return false;
    }

    const std::string userFileName =
        GetUserFile(id);

    // --------------------------------------------------------
    // Create user's data file in plaintext first.
    //
    // Format:
    //
    // string username
    // string password
    // int getAmount
    //
    // This matches the BinaryWriter layout from the
    // original C# implementation.
    // --------------------------------------------------------

    std::ofstream file(
        userFileName,
        std::ios::binary |
        std::ios::trunc);

    if (!file)
        return false;

    const std::int32_t usernameLength =
        static_cast<std::int32_t>(
            username.size());

    const std::int32_t passwordLength =
        static_cast<std::int32_t>(
            password.size());

    const std::int32_t getAmount = 0;

    file.write(
        reinterpret_cast<const char*>(
            &usernameLength),
        sizeof(usernameLength));

    file.write(
        username.data(),
        usernameLength);

    file.write(
        reinterpret_cast<const char*>(
            &passwordLength),
        sizeof(passwordLength));

    file.write(
        password.data(),
        passwordLength);

    file.write(
        reinterpret_cast<const char*>(
            &getAmount),
        sizeof(getAmount));

    if (!file.good())
        return false;

    file.close();

    // Encrypt user's #####.dat.
    if (!Encryption::TransformFile(
        userFileName,
        true,
        userEncryptionCode))
    {
        return false;
    }

    // settings.dat is already decrypted while the
    // application is running.
    if (!AppendUserToSettings(
        id,
        userEncryptionCode))
    {
        return false;
    }

    newUserId = id;

    return true;
}

// ------------------------------------------------------------
// Login
// ------------------------------------------------------------

bool UserManager::Login(
    int id,
    const std::string& username,
    const std::string& password)
{
    if (!initialized)
        return false;

    if (id < MIN_USER_ID ||
        id > MAX_USER_ID)
    {
        return false;
    }

    if (username.empty() ||
        password.empty())
    {
        return false;
    }

    Encryption::Code userEncryptionCode{};

    // settings.dat is already decrypted.
    if (!FindUserEncryptionCode(
        id,
        userEncryptionCode))
    {
        return false;
    }

    const std::string userFileName =
        GetUserFile(id);

    if (!UserIdExists(id))
        return false;

    // --------------------------------------------------------
    // CREATE BACKUP BEFORE DECRYPTING USER FILE
    // --------------------------------------------------------

    if (!CreateUserBackup(id))
        return false;

    // --------------------------------------------------------
    // Decrypt user's #####.dat.
    //
    // The encrypted backup already exists.
    // --------------------------------------------------------

    if (!Encryption::TransformFile(
        userFileName,
        false,
        userEncryptionCode))
    {
        return false;
    }

    // --------------------------------------------------------
    // Read credentials from decrypted user file.
    // --------------------------------------------------------

    bool loginSuccessful = false;

    {
        std::ifstream file(
            userFileName,
            std::ios::binary);

        if (file)
        {
            std::int32_t usernameLength = 0;
            std::int32_t passwordLength = 0;

            file.read(
                reinterpret_cast<char*>(
                    &usernameLength),
                sizeof(usernameLength));

            if (file &&
                usernameLength >= 0 &&
                usernameLength <= 1000000)
            {
                std::string storedUsername(
                    static_cast<std::size_t>(
                        usernameLength),
                    '\0');

                file.read(
                    storedUsername.data(),
                    usernameLength);

                file.read(
                    reinterpret_cast<char*>(
                        &passwordLength),
                    sizeof(passwordLength));

                if (file &&
                    passwordLength >= 0 &&
                    passwordLength <= 1000000)
                {
                    std::string storedPassword(
                        static_cast<std::size_t>(
                            passwordLength),
                        '\0');

                    file.read(
                        storedPassword.data(),
                        passwordLength);

                    std::int32_t getAmount = 0;

                    file.read(
                        reinterpret_cast<char*>(
                            &getAmount),
                        sizeof(getAmount));

                    if (file &&
                        storedUsername == username &&
                        storedPassword == password)
                    {
                        loginSuccessful = true;
                    }
                }
            }
        }
    }

    // --------------------------------------------------------
    // IMPORTANT:
    //
    // Do NOT re-encrypt the user's file here.
    //
    // If login was successful, the user's file must remain
    // decrypted while the Dashboard is open.
    // --------------------------------------------------------

    if (!loginSuccessful)
    {
        // Login failed, so restore the encrypted state.
        Encryption::TransformFile(
            userFileName,
            true,
            userEncryptionCode);

        // The user is not logged in, so there should be
        // no active user backup.
        RemoveUserBackup(id);
    }

    return loginSuccessful;
}

bool UserManager::Logout(int userId)
{
    if (!initialized ||
        userId <= 0)
    {
        return false;
    }

    Encryption::Code encryptionCode{};

    if (!FindUserEncryptionCode(
        userId,
        encryptionCode))
    {
        return false;
    }

    const std::string fileName =
        GetUserFile(userId);

    if (!std::filesystem::exists(fileName))
        return false;

    // -------------------------------------------------
    // STEP 1
    //
    // Remove the encrypted backup FIRST.
    // -------------------------------------------------

    if (!RemoveUserBackup(userId))
        return false;

    // -------------------------------------------------
    // STEP 2
    //
    // The user's original file is currently DECRYPTED.
    // Encrypt it before leaving the Dashboard.
    // -------------------------------------------------

    if (!Encryption::TransformFile(
        fileName,
        true,
        encryptionCode))
    {
        return false;
    }

    return true;
}

bool UserManager::DeleteUser(
    int userId,
    const std::string& username,
    const std::string& password)
{
    if (!initialized ||
        userId <= 0 ||
        username.empty() ||
        password.empty())
    {
        return false;
    }

    Encryption::Code encryptionCode{};

    if (!FindUserEncryptionCode(
        userId,
        encryptionCode))
    {
        return false;
    }

    const std::string userFileName =
        GetUserFile(userId);

    // --------------------------------------------------------
    // Decrypt user's data file.
    // --------------------------------------------------------

    if (!Encryption::TransformFile(
        userFileName,
        false,
        encryptionCode))
    {
        return false;
    }

    std::ifstream input(
        userFileName,
        std::ios::binary);

    if (!input)
    {
        Encryption::TransformFile(
            userFileName,
            true,
            encryptionCode);

        return false;
    }

    std::int32_t usernameLength = 0;
    std::int32_t passwordLength = 0;
    std::int32_t amount = 0;

    // --------------------------------------------------------
    // Read stored username.
    // --------------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&usernameLength),
        sizeof(usernameLength));

    if (!input ||
        usernameLength < 0 ||
        usernameLength > 1000000)
    {
        input.close();

        Encryption::TransformFile(
            userFileName,
            true,
            encryptionCode);

        return false;
    }

    std::string storedUsername(
        static_cast<std::size_t>(
            usernameLength),
        '\0');

    input.read(
        storedUsername.data(),
        usernameLength);

    // --------------------------------------------------------
    // Read stored password.
    // --------------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&passwordLength),
        sizeof(passwordLength));

    if (!input ||
        passwordLength < 0 ||
        passwordLength > 1000000)
    {
        input.close();

        Encryption::TransformFile(
            userFileName,
            true,
            encryptionCode);

        return false;
    }

    std::string storedPassword(
        static_cast<std::size_t>(
            passwordLength),
        '\0');

    input.read(
        storedPassword.data(),
        passwordLength);

    // --------------------------------------------------------
    // Verify credentials BEFORE deleting anything.
    // --------------------------------------------------------

    if (storedUsername != username ||
        storedPassword != password)
    {
        input.close();

        Encryption::TransformFile(
            userFileName,
            true,
            encryptionCode);

        return false;
    }

    // --------------------------------------------------------
    // We don't need to read the notes.
    // Credentials have been verified.
    // --------------------------------------------------------

    input.close();

    // --------------------------------------------------------
    // Delete user's diary file.
    // --------------------------------------------------------

    std::error_code error;

    if (!std::filesystem::remove(
        userFileName,
        error))
    {
        if (error)
            return false;
    }

    // --------------------------------------------------------
    // Remove user from settings.dat.
    // --------------------------------------------------------

    std::ifstream settingsInput(
        GetSettingsFile(),
        std::ios::binary);

    if (!settingsInput)
        return false;

    std::int32_t numberOfUsers;
    std::uint8_t backgroundColour;
    std::uint8_t foregroundColour;

    settingsInput.read(
        reinterpret_cast<char*>(&numberOfUsers),
        sizeof(numberOfUsers));

    settingsInput.read(
        reinterpret_cast<char*>(&backgroundColour),
        sizeof(backgroundColour));

    settingsInput.read(
        reinterpret_cast<char*>(&foregroundColour),
        sizeof(foregroundColour));

    if (!settingsInput ||
        numberOfUsers < 0 ||
        numberOfUsers > 1000000)
    {
        return false;
    }

    struct UserEntry
    {
        int id;
        Encryption::Code code;
    };

    std::vector<UserEntry> users;

    bool foundUser = false;

    for (std::int32_t i = 0;
        i < numberOfUsers;
        ++i)
    {
        std::int32_t storedId = 0;

        settingsInput.read(
            reinterpret_cast<char*>(&storedId),
            sizeof(storedId));

        if (!settingsInput)
            return false;

        Encryption::Code storedCode{};

        settingsInput.read(
            reinterpret_cast<char*>(
                storedCode.data()),
            static_cast<std::streamsize>(
                storedCode.size()));

        if (!settingsInput)
            return false;

        if (storedId == userId)
        {
            foundUser = true;
            continue;
        }

        users.push_back(
            { storedId, storedCode });
    }

    settingsInput.close();

    if (!foundUser)
        return false;

    // --------------------------------------------------------
    // Rewrite settings.dat.
    // --------------------------------------------------------

    std::ofstream settingsOutput(
        GetSettingsFile(),
        std::ios::binary |
        std::ios::trunc);

    if (!settingsOutput)
        return false;

    const std::int32_t newNumberOfUsers =
        static_cast<std::int32_t>(
            users.size());

    settingsOutput.write(
        reinterpret_cast<const char*>(
            &newNumberOfUsers),
        sizeof(newNumberOfUsers));

    settingsOutput.write(
        reinterpret_cast<const char*>(
            &backgroundColour),
        sizeof(backgroundColour));

    settingsOutput.write(
        reinterpret_cast<const char*>(
            &foregroundColour),
        sizeof(foregroundColour));

    for (const UserEntry& user : users)
    {
        const std::int32_t storedId =
            static_cast<std::int32_t>(
                user.id);

        settingsOutput.write(
            reinterpret_cast<const char*>(
                &storedId),
            sizeof(storedId));

        settingsOutput.write(
            reinterpret_cast<const char*>(
                user.code.data()),
            static_cast<std::streamsize>(
                user.code.size()));
    }

    if (!settingsOutput.good())
        return false;

    settingsOutput.close();

    return true;
}


// ------------------------------------------------------------
// Information
// ------------------------------------------------------------

bool UserManager::CreateUserBackup(
    int userId)
{
    if (!initialized ||
        userId <= 0)
    {
        return false;
    }

    Encryption::Code encryptionCode{};

    const std::string fileName =
        GetUserFile(userId);

    if (!std::filesystem::exists(fileName))
        return false;

    return BackupManager::BackUp(
        fileName,
        GetDataDirectory() +
        std::string(1,
            std::filesystem::path::preferred_separator) +
        "Backup",
        BackupManager::Decision::
        MAKE_THE_BACKUP_FILE,
        encryptionCode);
}


bool UserManager::RemoveUserBackup(int userId)
{
    if (userId <= 0)
        return false;

    Encryption::Code encryptionCode{};

    if (!FindUserEncryptionCode(
        userId,
        encryptionCode))
    {
        return false;
    }

    return BackupManager::BackUp(
        GetUserFile(userId),
        GetDataDirectory() +
        std::string(1,
            std::filesystem::path::preferred_separator) +
        "Backup",
        BackupManager::Decision::
        REMOVE_THE_BACKUP_FILE,
        encryptionCode);
}

int UserManager::GetNumberOfUsers() const
{
    return settings.numberOfUsers;
}

bool UserManager::WriteNote(
    int userId,
    const std::string& title,
    const std::string& text,
    int position)
{
    if (!initialized)
        return false;

    if (userId <= 0)
        return false;

    if (title.empty() || text.empty())
        return false;

    if (position < 1)
        return false;

    Encryption::Code encryptionCode{};

    if (!FindUserEncryptionCode(
        userId,
        encryptionCode))
    {
        return false;
    }

    const std::string fileName =
        GetUserFile(userId);

    if (!std::filesystem::exists(fileName))
        return false;

    // -------------------------------------------------
    // IMPORTANT:
    //
    // The user file is ALREADY DECRYPTED because the
    // user is logged in.
    //
    // Do NOT decrypt it here.
    // -------------------------------------------------

    // -------------------------------------------------
    // Read the existing file
    // -------------------------------------------------

    std::ifstream input(
        fileName,
        std::ios::binary);

    if (!input)
        return false;

    std::int32_t usernameLength = 0;
    std::int32_t passwordLength = 0;
    std::int32_t amount = 0;

    input.read(
        reinterpret_cast<char*>(&usernameLength),
        sizeof(usernameLength));

    if (!input ||
        usernameLength < 0 ||
        usernameLength > 1000000)
    {
        input.close();
        return false;
    }

    std::string username(
        static_cast<std::size_t>(usernameLength),
        '\0');

    input.read(
        username.data(),
        usernameLength);

    if (!input)
    {
        input.close();
        return false;
    }

    input.read(
        reinterpret_cast<char*>(&passwordLength),
        sizeof(passwordLength));

    if (!input ||
        passwordLength < 0 ||
        passwordLength > 1000000)
    {
        input.close();
        return false;
    }

    std::string password(
        static_cast<std::size_t>(passwordLength),
        '\0');

    input.read(
        password.data(),
        passwordLength);

    if (!input)
    {
        input.close();
        return false;
    }

    input.read(
        reinterpret_cast<char*>(&amount),
        sizeof(amount));

    if (!input ||
        amount < 0)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Position validation
    // -------------------------------------------------

    if (position > amount + 1)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Read existing notes
    // -------------------------------------------------

    struct Note
    {
        std::string title;
        std::string text;
    };

    std::vector<Note> notes;

    for (int i = 0; i < amount; ++i)
    {
        std::int32_t titleLength = 0;

        input.read(
            reinterpret_cast<char*>(&titleLength),
            sizeof(titleLength));

        if (!input ||
            titleLength < 0 ||
            titleLength > 50)
        {
            input.close();
            return false;
        }

        std::string oldTitle(
            static_cast<std::size_t>(titleLength),
            '\0');

        input.read(
            oldTitle.data(),
            titleLength);

        if (!input)
        {
            input.close();
            return false;
        }

        std::int32_t textLength = 0;

        input.read(
            reinterpret_cast<char*>(&textLength),
            sizeof(textLength));

        if (!input ||
            textLength < 0 ||
            textLength > 10000)
        {
            input.close();
            return false;
        }

        std::string oldText(
            static_cast<std::size_t>(textLength),
            '\0');

        input.read(
            oldText.data(),
            textLength);

        if (!input)
        {
            input.close();
            return false;
        }

        notes.push_back(
            { oldTitle, oldText });
    }

    input.close();

    // -------------------------------------------------
    // Insert new note at requested position
    // -------------------------------------------------

    notes.insert(
        notes.begin() + (position - 1),
        { title, text });

    // -------------------------------------------------
    // Rewrite the DECRYPTED user file
    // -------------------------------------------------

    std::ofstream output(
        fileName,
        std::ios::binary |
        std::ios::trunc);

    if (!output)
        return false;

    const std::int32_t newUsernameLength =
        static_cast<std::int32_t>(
            username.size());

    const std::int32_t newPasswordLength =
        static_cast<std::int32_t>(
            password.size());

    const std::int32_t newAmount =
        static_cast<std::int32_t>(
            notes.size());

    // User information

    output.write(
        reinterpret_cast<const char*>(
            &newUsernameLength),
        sizeof(newUsernameLength));

    output.write(
        username.data(),
        newUsernameLength);

    output.write(
        reinterpret_cast<const char*>(
            &newPasswordLength),
        sizeof(newPasswordLength));

    output.write(
        password.data(),
        newPasswordLength);

    output.write(
        reinterpret_cast<const char*>(
            &newAmount),
        sizeof(newAmount));

    // Notes

    for (const Note& note : notes)
    {
        const std::int32_t titleLength =
            static_cast<std::int32_t>(
                note.title.size());

        const std::int32_t textLength =
            static_cast<std::int32_t>(
                note.text.size());

        output.write(
            reinterpret_cast<const char*>(
                &titleLength),
            sizeof(titleLength));

        output.write(
            note.title.data(),
            titleLength);

        output.write(
            reinterpret_cast<const char*>(
                &textLength),
            sizeof(textLength));

        output.write(
            note.text.data(),
            textLength);
    }

    if (!output.good())
    {
        output.close();
        return false;
    }

    output.close();

    // -------------------------------------------------
    // IMPORTANT:
    //
    // The original user file remains DECRYPTED.
    //
    // Create a new ENCRYPTED backup from it.
    // -------------------------------------------------

    if (!CreateUserBackup(userId))
        return false;

    return true;
}

bool UserManager::LoadNoteTitles(
    int userId,
    std::vector<std::string>& titles)
{
    titles.clear();

    if (!initialized ||
        userId <= 0)
    {
        return false;
    }

    const std::string fileName =
        GetUserFile(userId);

    if (!std::filesystem::exists(fileName))
        return false;

    // -------------------------------------------------
    // IMPORTANT:
    //
    // The user's file is already DECRYPTED because
    // the user is logged in.
    //
    // Do NOT decrypt or encrypt it here.
    // -------------------------------------------------

    std::ifstream input(
        fileName,
        std::ios::binary);

    if (!input)
        return false;

    std::int32_t usernameLength = 0;
    std::int32_t passwordLength = 0;
    std::int32_t amount = 0;

    // -------------------------------------------------
    // Username length
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&usernameLength),
        sizeof(usernameLength));

    if (!input ||
        usernameLength < 0 ||
        usernameLength > 1000000)
    {
        input.close();
        return false;
    }

    // Skip username
    input.seekg(
        usernameLength,
        std::ios::cur);

    if (!input)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Password length
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&passwordLength),
        sizeof(passwordLength));

    if (!input ||
        passwordLength < 0 ||
        passwordLength > 1000000)
    {
        input.close();
        return false;
    }

    // Skip password
    input.seekg(
        passwordLength,
        std::ios::cur);

    if (!input)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Number of notes
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&amount),
        sizeof(amount));

    if (!input ||
        amount < 0)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Read each note title
    // -------------------------------------------------

    for (std::int32_t i = 0;
        i < amount;
        ++i)
    {
        std::int32_t titleLength = 0;

        input.read(
            reinterpret_cast<char*>(&titleLength),
            sizeof(titleLength));

        if (!input ||
            titleLength < 0 ||
            titleLength > 50)
        {
            input.close();
            return false;
        }

        std::string title(
            static_cast<std::size_t>(titleLength),
            '\0');

        input.read(
            title.data(),
            titleLength);

        if (!input)
        {
            input.close();
            return false;
        }

        // -------------------------------------------------
        // Read text length and skip the text.
        // -------------------------------------------------

        std::int32_t textLength = 0;

        input.read(
            reinterpret_cast<char*>(&textLength),
            sizeof(textLength));

        if (!input ||
            textLength < 0 ||
            textLength > 10000)
        {
            input.close();
            return false;
        }

        input.seekg(
            textLength,
            std::ios::cur);

        if (!input)
        {
            input.close();
            return false;
        }

        titles.push_back(title);
    }

    input.close();

    return true;
}

bool UserManager::ReadNote(
    int userId,
    int position,
    std::string& title,
    std::string& text)
{
    title.clear();
    text.clear();

    if (!initialized ||
        userId <= 0 ||
        position < 1)
    {
        return false;
    }

    const std::string fileName =
        GetUserFile(userId);

    if (!std::filesystem::exists(fileName))
        return false;

    // -------------------------------------------------
    // IMPORTANT:
    //
    // The user's file is already DECRYPTED because
    // the user is logged in.
    //
    // Do NOT decrypt or encrypt it here.
    // -------------------------------------------------

    std::ifstream input(
        fileName,
        std::ios::binary);

    if (!input)
        return false;

    std::int32_t usernameLength = 0;
    std::int32_t passwordLength = 0;
    std::int32_t amount = 0;

    // -------------------------------------------------
    // Username
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&usernameLength),
        sizeof(usernameLength));

    if (!input ||
        usernameLength < 0 ||
        usernameLength > 1000000)
    {
        input.close();
        return false;
    }

    input.seekg(
        usernameLength,
        std::ios::cur);

    if (!input)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Password
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&passwordLength),
        sizeof(passwordLength));

    if (!input ||
        passwordLength < 0 ||
        passwordLength > 1000000)
    {
        input.close();
        return false;
    }

    input.seekg(
        passwordLength,
        std::ios::cur);

    if (!input)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Number of notes
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&amount),
        sizeof(amount));

    if (!input ||
        amount < 0 ||
        position > amount)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Read notes until requested position
    // -------------------------------------------------

    for (int i = 1; i <= amount; ++i)
    {
        std::int32_t titleLength = 0;

        input.read(
            reinterpret_cast<char*>(&titleLength),
            sizeof(titleLength));

        if (!input ||
            titleLength < 0 ||
            titleLength > 50)
        {
            input.close();
            return false;
        }

        std::string currentTitle(
            static_cast<std::size_t>(titleLength),
            '\0');

        input.read(
            currentTitle.data(),
            titleLength);

        if (!input)
        {
            input.close();
            return false;
        }

        // -------------------------------------------------
        // Text
        // -------------------------------------------------

        std::int32_t textLength = 0;

        input.read(
            reinterpret_cast<char*>(&textLength),
            sizeof(textLength));

        if (!input ||
            textLength < 0 ||
            textLength > 10000)
        {
            input.close();
            return false;
        }

        std::string currentText(
            static_cast<std::size_t>(textLength),
            '\0');

        input.read(
            currentText.data(),
            textLength);

        if (!input)
        {
            input.close();
            return false;
        }

        // -------------------------------------------------
        // Requested note found
        // -------------------------------------------------

        if (i == position)
        {
            title = currentTitle;
            text = currentText;

            input.close();

            return true;
        }
    }

    input.close();

    return false;
}

bool UserManager::AppendToNote(
    int userId,
    int position,
    const std::string& textToAppend)
{
    if (!initialized ||
        userId <= 0)
    {
        return false;
    }

    if (position < 1 ||
        textToAppend.empty())
    {
        return false;
    }

    const std::string fileName =
        GetUserFile(userId);

    if (!std::filesystem::exists(fileName))
        return false;

    // -------------------------------------------------
    // IMPORTANT:
    //
    // The user's file is ALREADY DECRYPTED because
    // the user is logged in.
    //
    // Do NOT decrypt or encrypt it here.
    // -------------------------------------------------

    std::ifstream input(
        fileName,
        std::ios::binary);

    if (!input)
        return false;

    std::int32_t usernameLength = 0;
    std::int32_t passwordLength = 0;
    std::int32_t amount = 0;

    // -------------------------------------------------
    // Username
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&usernameLength),
        sizeof(usernameLength));

    if (!input ||
        usernameLength < 0 ||
        usernameLength > 1000000)
    {
        input.close();
        return false;
    }

    std::string username(
        static_cast<std::size_t>(usernameLength),
        '\0');

    input.read(
        username.data(),
        usernameLength);

    if (!input)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Password
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&passwordLength),
        sizeof(passwordLength));

    if (!input ||
        passwordLength < 0 ||
        passwordLength > 1000000)
    {
        input.close();
        return false;
    }

    std::string password(
        static_cast<std::size_t>(passwordLength),
        '\0');

    input.read(
        password.data(),
        passwordLength);

    if (!input)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Number of notes
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&amount),
        sizeof(amount));

    if (!input ||
        amount < 0 ||
        position > amount)
    {
        input.close();
        return false;
    }

    struct Note
    {
        std::string title;
        std::string text;
    };

    std::vector<Note> notes;

    // -------------------------------------------------
    // Read all notes
    // -------------------------------------------------

    for (std::int32_t i = 0;
        i < amount;
        ++i)
    {
        std::int32_t titleLength = 0;

        input.read(
            reinterpret_cast<char*>(&titleLength),
            sizeof(titleLength));

        if (!input ||
            titleLength < 0 ||
            titleLength > 50)
        {
            input.close();
            return false;
        }

        std::string noteTitle(
            static_cast<std::size_t>(titleLength),
            '\0');

        input.read(
            noteTitle.data(),
            titleLength);

        if (!input)
        {
            input.close();
            return false;
        }

        std::int32_t textLength = 0;

        input.read(
            reinterpret_cast<char*>(&textLength),
            sizeof(textLength));

        if (!input ||
            textLength < 0 ||
            textLength > 10000)
        {
            input.close();
            return false;
        }

        std::string noteText(
            static_cast<std::size_t>(textLength),
            '\0');

        input.read(
            noteText.data(),
            textLength);

        if (!input)
        {
            input.close();
            return false;
        }

        notes.push_back(
            { noteTitle, noteText });
    }

    input.close();

    // -------------------------------------------------
    // Append to selected note
    // -------------------------------------------------

    notes[position - 1].text += textToAppend;

    // -------------------------------------------------
    // Rewrite the DECRYPTED diary file
    // -------------------------------------------------

    std::ofstream output(
        fileName,
        std::ios::binary |
        std::ios::trunc);

    if (!output)
        return false;

    const std::int32_t newUsernameLength =
        static_cast<std::int32_t>(
            username.size());

    const std::int32_t newPasswordLength =
        static_cast<std::int32_t>(
            password.size());

    output.write(
        reinterpret_cast<const char*>(
            &newUsernameLength),
        sizeof(newUsernameLength));

    output.write(
        username.data(),
        newUsernameLength);

    output.write(
        reinterpret_cast<const char*>(
            &newPasswordLength),
        sizeof(newPasswordLength));

    output.write(
        password.data(),
        newPasswordLength);

    output.write(
        reinterpret_cast<const char*>(
            &amount),
        sizeof(amount));

    // -------------------------------------------------
    // Write notes
    // -------------------------------------------------

    for (const Note& note : notes)
    {
        const std::int32_t titleLength =
            static_cast<std::int32_t>(
                note.title.size());

        const std::int32_t textLength =
            static_cast<std::int32_t>(
                note.text.size());

        output.write(
            reinterpret_cast<const char*>(
                &titleLength),
            sizeof(titleLength));

        output.write(
            note.title.data(),
            titleLength);

        output.write(
            reinterpret_cast<const char*>(
                &textLength),
            sizeof(textLength));

        output.write(
            note.text.data(),
            textLength);
    }

    if (!output.good())
    {
        output.close();
        return false;
    }

    output.close();

    // -------------------------------------------------
    // IMPORTANT:
    //
    // Original <userID>.dat remains DECRYPTED.
    //
    // Create/update the encrypted backup.
    // -------------------------------------------------

    if (!CreateUserBackup(userId))
        return false;

    return true;
}

bool UserManager::RemoveNote(
    int userId,
    int position)
{
    if (!initialized ||
        userId <= 0)
    {
        return false;
    }

    if (position < 1)
        return false;

    const std::string fileName =
        GetUserFile(userId);

    if (!std::filesystem::exists(fileName))
        return false;

    // -------------------------------------------------
    // IMPORTANT:
    //
    // The user's file is ALREADY DECRYPTED because
    // the user is logged in.
    //
    // Do NOT decrypt or encrypt it here.
    // -------------------------------------------------

    std::ifstream input(
        fileName,
        std::ios::binary);

    if (!input)
        return false;

    std::int32_t usernameLength = 0;
    std::int32_t passwordLength = 0;
    std::int32_t amount = 0;

    // -------------------------------------------------
    // Username
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&usernameLength),
        sizeof(usernameLength));

    if (!input ||
        usernameLength < 0 ||
        usernameLength > 1000000)
    {
        input.close();
        return false;
    }

    std::string username(
        static_cast<std::size_t>(usernameLength),
        '\0');

    input.read(
        username.data(),
        usernameLength);

    if (!input)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Password
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&passwordLength),
        sizeof(passwordLength));

    if (!input ||
        passwordLength < 0 ||
        passwordLength > 1000000)
    {
        input.close();
        return false;
    }

    std::string password(
        static_cast<std::size_t>(passwordLength),
        '\0');

    input.read(
        password.data(),
        passwordLength);

    if (!input)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Number of notes
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&amount),
        sizeof(amount));

    if (!input ||
        amount < 0 ||
        position > amount)
    {
        input.close();
        return false;
    }

    struct Note
    {
        std::string title;
        std::string text;
    };

    std::vector<Note> notes;

    // -------------------------------------------------
    // Read all notes
    // -------------------------------------------------

    for (std::int32_t i = 0;
        i < amount;
        ++i)
    {
        std::int32_t titleLength = 0;

        input.read(
            reinterpret_cast<char*>(&titleLength),
            sizeof(titleLength));

        if (!input ||
            titleLength < 0 ||
            titleLength > 50)
        {
            input.close();
            return false;
        }

        std::string noteTitle(
            static_cast<std::size_t>(titleLength),
            '\0');

        input.read(
            noteTitle.data(),
            titleLength);

        if (!input)
        {
            input.close();
            return false;
        }

        std::int32_t textLength = 0;

        input.read(
            reinterpret_cast<char*>(&textLength),
            sizeof(textLength));

        if (!input ||
            textLength < 0 ||
            textLength > 10000)
        {
            input.close();
            return false;
        }

        std::string noteText(
            static_cast<std::size_t>(textLength),
            '\0');

        input.read(
            noteText.data(),
            textLength);

        if (!input)
        {
            input.close();
            return false;
        }

        notes.push_back(
            { noteTitle, noteText });
    }

    input.close();

    // -------------------------------------------------
    // Remove selected note
    // -------------------------------------------------

    notes.erase(
        notes.begin() + (position - 1));

    // -------------------------------------------------
    // Rewrite the DECRYPTED diary file
    // -------------------------------------------------

    std::ofstream output(
        fileName,
        std::ios::binary |
        std::ios::trunc);

    if (!output)
        return false;

    const std::int32_t newUsernameLength =
        static_cast<std::int32_t>(
            username.size());

    const std::int32_t newPasswordLength =
        static_cast<std::int32_t>(
            password.size());

    const std::int32_t newAmount =
        static_cast<std::int32_t>(
            notes.size());

    output.write(
        reinterpret_cast<const char*>(
            &newUsernameLength),
        sizeof(newUsernameLength));

    output.write(
        username.data(),
        newUsernameLength);

    output.write(
        reinterpret_cast<const char*>(
            &newPasswordLength),
        sizeof(newPasswordLength));

    output.write(
        password.data(),
        newPasswordLength);

    output.write(
        reinterpret_cast<const char*>(
            &newAmount),
        sizeof(newAmount));

    // -------------------------------------------------
    // Write remaining notes
    // -------------------------------------------------

    for (const Note& note : notes)
    {
        const std::int32_t titleLength =
            static_cast<std::int32_t>(
                note.title.size());

        const std::int32_t textLength =
            static_cast<std::int32_t>(
                note.text.size());

        output.write(
            reinterpret_cast<const char*>(
                &titleLength),
            sizeof(titleLength));

        output.write(
            note.title.data(),
            titleLength);

        output.write(
            reinterpret_cast<const char*>(
                &textLength),
            sizeof(textLength));

        output.write(
            note.text.data(),
            textLength);
    }

    if (!output.good())
    {
        output.close();
        return false;
    }

    output.close();

    // -------------------------------------------------
    // IMPORTANT:
    //
    // Original <userID>.dat remains DECRYPTED.
    //
    // Create/update the encrypted backup.
    // -------------------------------------------------

    if (!CreateUserBackup(userId))
        return false;

    return true;
}

bool UserManager::ChangeUsernamePassword(
    int userId,
    const std::string& newUsername,
    const std::string& newPassword)
{
    if (!initialized ||
        userId <= 0)
    {
        return false;
    }

    if (newUsername.empty() ||
        newPassword.empty())
    {
        return false;
    }

    const std::string fileName =
        GetUserFile(userId);

    if (!std::filesystem::exists(fileName))
        return false;

    // -------------------------------------------------
    // IMPORTANT:
    //
    // The user's file is ALREADY DECRYPTED because
    // the user is logged in.
    //
    // Do NOT decrypt or encrypt it here.
    // -------------------------------------------------

    std::ifstream input(
        fileName,
        std::ios::binary);

    if (!input)
        return false;

    std::int32_t usernameLength = 0;
    std::int32_t passwordLength = 0;
    std::int32_t amount = 0;

    // -------------------------------------------------
    // Read current username
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&usernameLength),
        sizeof(usernameLength));

    if (!input ||
        usernameLength < 0 ||
        usernameLength > 1000000)
    {
        input.close();
        return false;
    }

    std::string username(
        static_cast<std::size_t>(usernameLength),
        '\0');

    input.read(
        username.data(),
        usernameLength);

    if (!input)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Read current password
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&passwordLength),
        sizeof(passwordLength));

    if (!input ||
        passwordLength < 0 ||
        passwordLength > 1000000)
    {
        input.close();
        return false;
    }

    std::string password(
        static_cast<std::size_t>(passwordLength),
        '\0');

    input.read(
        password.data(),
        passwordLength);

    if (!input)
    {
        input.close();
        return false;
    }

    // -------------------------------------------------
    // Read number of notes
    // -------------------------------------------------

    input.read(
        reinterpret_cast<char*>(&amount),
        sizeof(amount));

    if (!input ||
        amount < 0)
    {
        input.close();
        return false;
    }

    struct Note
    {
        std::string title;
        std::string text;
    };

    std::vector<Note> notes;

    // -------------------------------------------------
    // Read existing notes
    // -------------------------------------------------

    for (std::int32_t i = 0;
        i < amount;
        ++i)
    {
        std::int32_t titleLength = 0;

        input.read(
            reinterpret_cast<char*>(&titleLength),
            sizeof(titleLength));

        if (!input ||
            titleLength < 0 ||
            titleLength > 50)
        {
            input.close();
            return false;
        }

        std::string title(
            static_cast<std::size_t>(titleLength),
            '\0');

        input.read(
            title.data(),
            titleLength);

        if (!input)
        {
            input.close();
            return false;
        }

        std::int32_t textLength = 0;

        input.read(
            reinterpret_cast<char*>(&textLength),
            sizeof(textLength));

        if (!input ||
            textLength < 0 ||
            textLength > 10000)
        {
            input.close();
            return false;
        }

        std::string text(
            static_cast<std::size_t>(textLength),
            '\0');

        input.read(
            text.data(),
            textLength);

        if (!input)
        {
            input.close();
            return false;
        }

        notes.push_back(
            { title, text });
    }

    input.close();

    // -------------------------------------------------
    // Rewrite with new credentials
    //
    // The file remains DECRYPTED.
    // -------------------------------------------------

    std::ofstream output(
        fileName,
        std::ios::binary |
        std::ios::trunc);

    if (!output)
        return false;

    const std::int32_t newUsernameLength =
        static_cast<std::int32_t>(
            newUsername.size());

    const std::int32_t newPasswordLength =
        static_cast<std::int32_t>(
            newPassword.size());

    output.write(
        reinterpret_cast<const char*>(
            &newUsernameLength),
        sizeof(newUsernameLength));

    output.write(
        newUsername.data(),
        newUsernameLength);

    output.write(
        reinterpret_cast<const char*>(
            &newPasswordLength),
        sizeof(newPasswordLength));

    output.write(
        newPassword.data(),
        newPasswordLength);

    output.write(
        reinterpret_cast<const char*>(
            &amount),
        sizeof(amount));

    // -------------------------------------------------
    // Write notes unchanged
    // -------------------------------------------------

    for (const Note& note : notes)
    {
        const std::int32_t titleLength =
            static_cast<std::int32_t>(
                note.title.size());

        const std::int32_t textLength =
            static_cast<std::int32_t>(
                note.text.size());

        output.write(
            reinterpret_cast<const char*>(
                &titleLength),
            sizeof(titleLength));

        output.write(
            note.title.data(),
            titleLength);

        output.write(
            reinterpret_cast<const char*>(
                &textLength),
            sizeof(textLength));

        output.write(
            note.text.data(),
            textLength);
    }

    if (!output.good())
    {
        output.close();
        return false;
    }

    output.close();

    // -------------------------------------------------
    // IMPORTANT:
    //
    // Original <userID>.dat remains DECRYPTED.
    //
    // Create/update the encrypted backup.
    // -------------------------------------------------

    if (!CreateUserBackup(userId))
        return false;

    return true;
}

// ------------------------------------------------------------
// Form Colour
// ------------------------------------------------------------

bool UserManager::SetFormColours(
    std::uint8_t backgroundColour,
    std::uint8_t foregroundColour)
{
    if (!initialized)
        return false;

    if (backgroundColour > 6 ||
        foregroundColour > 6)
    {
        return false;
    }

    // Background and foreground cannot be identical.
    if (backgroundColour == foregroundColour)
        return false;

    // -------------------------------------------------
    // Open settings.dat
    //
    // settings.dat is already decrypted while the
    // application is running.
    // -------------------------------------------------

    std::fstream file(
        GetSettingsFile(),
        std::ios::binary |
        std::ios::in |
        std::ios::out);

    if (!file)
        return false;

    // -------------------------------------------------
    // Write background colour
    // -------------------------------------------------

    file.seekp(
        sizeof(settings.numberOfUsers),
        std::ios::beg);

    file.write(
        reinterpret_cast<const char*>(
            &backgroundColour),
        sizeof(backgroundColour));

    if (!file.good())
        return false;

    // -------------------------------------------------
    // Write foreground colour
    // -------------------------------------------------

    file.seekp(
        sizeof(settings.numberOfUsers) +
        sizeof(settings.backgroundColour),
        std::ios::beg);

    file.write(
        reinterpret_cast<const char*>(
            &foregroundColour),
        sizeof(foregroundColour));

    if (!file.good())
        return false;

    file.close();

    // -------------------------------------------------
    // Only update the in-memory settings after the
    // settings file has been written successfully.
    // -------------------------------------------------

    settings.backgroundColour =
        backgroundColour;

    settings.foregroundColour =
        foregroundColour;

    // -------------------------------------------------
    // Create ONE backup containing BOTH new colours.
    // -------------------------------------------------

    return CreateSettingsBackup();
}

wxColour UserManager::GetColour(
    std::uint8_t colour) const
{
    switch (colour)
    {
    case 0:
        // Grey
        return wxSystemSettings::GetColour(
            wxSYS_COLOUR_3DFACE);

    case 1:
        // Black
        return *wxBLACK;

    case 2:
        // White
        return *wxWHITE;

    case 3:
        // Red
        return *wxRED;

    case 4:
        // Green
        return *wxGREEN;

    case 5:
        // Blue
        return *wxBLUE;

    case 6:
        // Yellow
        return *wxYELLOW;

    default:
        // Safe fallback
        return wxSystemSettings::GetColour(
            wxSYS_COLOUR_3DFACE);
    }
}


std::uint8_t UserManager::GetBackgroundColour() const
{
    return settings.backgroundColour;
}


std::uint8_t UserManager::GetForegroundColour() const
{
    return settings.foregroundColour;
}