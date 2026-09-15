#include "MainFrame.h"

#include "LoginFrame.h"
#include "RegistrationFrame.h"

#include <wx/textdlg.h>
#include <wx/settings.h>

enum
{
    ID_MAIN_LOGIN = wxID_HIGHEST + 1,
    ID_MAIN_REGISTER,
    ID_MAIN_DELETE_USER,
    ID_MAIN_FORM_COLOUR,
    ID_MAIN_QUIT,

    ID_FORM_COLOUR_CHANGE,
    ID_FORM_COLOUR_BACK
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)

EVT_BUTTON(
    ID_MAIN_LOGIN,
    MainFrame::OnButton)

    EVT_BUTTON(
        ID_MAIN_REGISTER,
        MainFrame::OnButton)

    EVT_BUTTON(
        ID_MAIN_DELETE_USER,
        MainFrame::OnDeleteUser)

    EVT_BUTTON(
        ID_MAIN_FORM_COLOUR,
        MainFrame::OnFormColour)

    EVT_BUTTON(
        ID_MAIN_QUIT,
        MainFrame::OnButton)

    EVT_BUTTON(
        ID_FORM_COLOUR_CHANGE,
        MainFrame::OnFormColourChange)

    EVT_BUTTON(
        ID_FORM_COLOUR_BACK,
        MainFrame::OnBackToMainMenu)

    wxEND_EVENT_TABLE()


    // =========================================================
    // CONSTRUCTOR
    // =========================================================

    MainFrame::MainFrame(
        wxWindow* parent,
        UserManager& userManager)
    : wxFrame(
        parent,
        wxID_ANY,
        "Diary",
        wxDefaultPosition,
        wxSize(640, 480)),
    userManager(userManager)
{
    // -----------------------------------------------------
    // Main panel
    // -----------------------------------------------------

    panel = new wxPanel(this);

    // =====================================================
    // MAIN MENU
    // =====================================================

    welcome = new wxStaticText(
        panel,
        wxID_ANY,
        "Welcome to Diary",
        wxPoint(180, 35),
        wxSize(300, 40));

    wxFont welcomeFont(
        20,
        wxFONTFAMILY_SWISS,
        wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL);

    welcome->SetFont(welcomeFont);

    // -----------------------------------------------------
    // Log In
    // -----------------------------------------------------

    loginButton = new wxButton(
        panel,
        ID_MAIN_LOGIN,
        "Log In",
        wxPoint(230, 120),
        wxSize(180, 40));

    // -----------------------------------------------------
    // Register
    // -----------------------------------------------------

    registerButton = new wxButton(
        panel,
        ID_MAIN_REGISTER,
        "Register a User",
        wxPoint(230, 175),
        wxSize(180, 40));

    // -----------------------------------------------------
    // Delete User
    // -----------------------------------------------------

    deleteUserButton = new wxButton(
        panel,
        ID_MAIN_DELETE_USER,
        "Delete a User",
        wxPoint(230, 230),
        wxSize(180, 40));

    // -----------------------------------------------------
    // Form Colour
    // -----------------------------------------------------

    formColourButton = new wxButton(
        panel,
        ID_MAIN_FORM_COLOUR,
        "Form Colour",
        wxPoint(230, 285),
        wxSize(180, 40));

    // -----------------------------------------------------
    // Quit
    // -----------------------------------------------------

    quitButton = new wxButton(
        panel,
        ID_MAIN_QUIT,
        "Quit",
        wxPoint(230, 340),
        wxSize(180, 40));

    // =====================================================
    // FORM COLOUR SCREEN
    // =====================================================

    backgroundColourLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "Background Colour",
        wxPoint(150, 110),
        wxSize(150, 30));

    const wxString colourChoices[] =
    {
        "Grey",
        "Black",
        "White",
        "Red",
        "Green",
        "Blue",
        "Yellow"
    };

    backgroundColourChoice = new wxChoice(
        panel,
        wxID_ANY,
        wxPoint(320, 105),
        wxSize(150, 35),
        7,
        colourChoices);

    foregroundColourLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "Foreground Colour",
        wxPoint(150, 175),
        wxSize(150, 30));

    foregroundColourChoice = new wxChoice(
        panel,
        wxID_ANY,
        wxPoint(320, 170),
        wxSize(150, 35),
        7,
        colourChoices);

    changeFormColourButton = new wxButton(
        panel,
        ID_FORM_COLOUR_CHANGE,
        "Change",
        wxPoint(230, 250),
        wxSize(180, 40));

    backToMainMenuButton = new wxButton(
        panel,
        ID_FORM_COLOUR_BACK,
        "Back to Main Menu",
        wxPoint(230, 305),
        wxSize(180, 40));

    // -----------------------------------------------------
    // Hide Form Colour screen initially
    // -----------------------------------------------------

    backgroundColourLabel->Hide();
    backgroundColourChoice->Hide();

    foregroundColourLabel->Hide();
    foregroundColourChoice->Hide();

    changeFormColourButton->Hide();
    backToMainMenuButton->Hide();

    // -----------------------------------------------------
    // Apply saved colours
    // -----------------------------------------------------

    ApplyColours();

    Centre();
}


// =========================================================
// APPLY COLOURS
// =========================================================

void MainFrame::ApplyColours()
{
    const wxColour background =
        userManager.GetColour(
            userManager.GetBackgroundColour());

    const wxColour foreground =
        userManager.GetColour(
            userManager.GetForegroundColour());

    // -------------------------------------------------
    // Form background
    // -------------------------------------------------

    SetBackgroundColour(background);

    if (panel != nullptr)
    {
        panel->SetBackgroundColour(background);
    }

    // -------------------------------------------------
    // Static text foreground
    // -------------------------------------------------

    if (welcome != nullptr)
        welcome->SetForegroundColour(foreground);

    if (backgroundColourLabel != nullptr)
    {
        backgroundColourLabel->SetForegroundColour(
            foreground);
    }

    if (foregroundColourLabel != nullptr)
    {
        foregroundColourLabel->SetForegroundColour(
            foreground);
    }

    // -------------------------------------------------
    // Buttons are deliberately NOT modified.
    //
    // Text controls are deliberately NOT modified.
    //
    // List boxes are deliberately NOT modified.
    // -------------------------------------------------

    Refresh();

    if (panel != nullptr)
        panel->Refresh();
}


// =========================================================
// MAIN BUTTON HANDLER
// =========================================================

void MainFrame::OnButton(
    wxCommandEvent& event)
{
    const int id = event.GetId();

    if (id == ID_MAIN_LOGIN)
    {
        auto* login =
            new LoginFrame(
                this,
                userManager);

        login->Show();
    }
    else if (id == ID_MAIN_REGISTER)
    {
        auto* registration =
            new RegistrationFrame(
                this,
                userManager);

        registration->Show();
    }
    else if (id == ID_MAIN_QUIT)
    {
        Close(true);
    }
}


// =========================================================
// DELETE USER
// =========================================================

void MainFrame::OnDeleteUser(
    wxCommandEvent& event)
{
    wxTextEntryDialog idDialog(
        this,
        "Enter the User ID:",
        "Delete a User");

    if (idDialog.ShowModal() != wxID_OK)
        return;

    long enteredId = 0;

    if (!idDialog.GetValue().ToLong(&enteredId) ||
        enteredId <= 0)
    {
        wxMessageBox(
            "Please enter a valid User ID.",
            "Delete User",
            wxOK | wxICON_WARNING);

        return;
    }

    wxTextEntryDialog usernameDialog(
        this,
        "Enter the username:",
        "Delete a User");

    if (usernameDialog.ShowModal() != wxID_OK)
        return;

    const wxString username =
        usernameDialog.GetValue();

    if (username.IsEmpty())
    {
        wxMessageBox(
            "Username is required.",
            "Delete User",
            wxOK | wxICON_WARNING);

        return;
    }

    wxTextEntryDialog passwordDialog(
        this,
        "Enter the password:",
        "Delete a User",
        wxEmptyString,
        wxOK | wxCANCEL | wxTE_PASSWORD);

    if (passwordDialog.ShowModal() != wxID_OK)
        return;

    const wxString password =
        passwordDialog.GetValue();

    if (password.IsEmpty())
    {
        wxMessageBox(
            "Password is required.",
            "Delete User",
            wxOK | wxICON_WARNING);

        return;
    }

    const int userId =
        static_cast<int>(enteredId);

    const int answer =
        wxMessageBox(
            wxString::Format(
                "Are you sure you want to permanently "
                "delete User ID %d?\n\n"
                "All diary notes belonging to this "
                "user will also be deleted.",
                userId),
            "Confirm User Deletion",
            wxYES_NO | wxICON_WARNING);

    if (answer != wxYES)
        return;

    if (!userManager.DeleteUser(
        userId,
        username.ToStdString(),
        password.ToStdString()))
    {
        wxMessageBox(
            "Unable to delete the user.\n\n"
            "The User ID, username, or password "
            "may be incorrect.",
            "Delete User",
            wxOK | wxICON_ERROR);

        return;
    }

    wxMessageBox(
        wxString::Format(
            "User ID %d was deleted successfully.",
            userId),
        "Delete User",
        wxOK | wxICON_INFORMATION);
}


// =========================================================
// FORM COLOUR SCREEN
// =========================================================

void MainFrame::OnFormColour(
    wxCommandEvent& event)
{
    // -----------------------------------------------------
    // Hide main menu
    // -----------------------------------------------------

    welcome->Hide();
    loginButton->Hide();
    registerButton->Hide();
    deleteUserButton->Hide();
    formColourButton->Hide();
    quitButton->Hide();

    // -----------------------------------------------------
    // Load currently saved colours
    // -----------------------------------------------------

    const int background =
        static_cast<int>(
            userManager.GetBackgroundColour());

    const int foreground =
        static_cast<int>(
            userManager.GetForegroundColour());

    backgroundColourChoice->SetSelection(
        background);

    foregroundColourChoice->SetSelection(
        foreground);

    // -----------------------------------------------------
    // Show Form Colour controls
    // -----------------------------------------------------

    backgroundColourLabel->Show();
    backgroundColourChoice->Show();

    foregroundColourLabel->Show();
    foregroundColourChoice->Show();

    changeFormColourButton->Show();
    backToMainMenuButton->Show();

    Layout();
}


// =========================================================
// CHANGE FORM COLOUR
// =========================================================

void MainFrame::OnFormColourChange(
    wxCommandEvent& event)
{
    const int background =
        backgroundColourChoice->GetSelection();

    const int foreground =
        foregroundColourChoice->GetSelection();

    // -----------------------------------------------------
    // Make sure both colours were selected
    // -----------------------------------------------------

    if (background == wxNOT_FOUND ||
        foreground == wxNOT_FOUND)
    {
        wxMessageBox(
            "Please select both colours.",
            "Form Colour",
            wxOK | wxICON_WARNING);

        return;
    }

    // -----------------------------------------------------
    // Background and foreground cannot be identical
    // -----------------------------------------------------

    if (background == foreground)
    {
        wxMessageBox(
            "Colours cannot be same.",
            "Error",
            wxOK | wxICON_WARNING);

        return;
    }

    // -----------------------------------------------------
    // Save form colour
    // -----------------------------------------------------

    if (!userManager.SetFormColours(
        static_cast<std::uint8_t>(background),
        static_cast<std::uint8_t>(foreground)))
    {
        wxMessageBox(
            "Unable to change the form colours.",
            "Error",
            wxOK | wxICON_ERROR);

        return;
    }

    // -----------------------------------------------------
    // Apply immediately
    // -----------------------------------------------------

    ApplyColours();

    wxMessageBox(
        "Colours are changed !!!",
        "Message",
        wxOK | wxICON_INFORMATION);
}


// =========================================================
// BACK TO MAIN MENU
// =========================================================

void MainFrame::OnBackToMainMenu(
    wxCommandEvent& event)
{
    // -----------------------------------------------------
    // Hide Form Colour controls
    // -----------------------------------------------------

    backgroundColourLabel->Hide();
    backgroundColourChoice->Hide();

    foregroundColourLabel->Hide();
    foregroundColourChoice->Hide();

    changeFormColourButton->Hide();
    backToMainMenuButton->Hide();

    // -----------------------------------------------------
    // Show main menu
    // -----------------------------------------------------

    welcome->Show();
    loginButton->Show();
    registerButton->Show();
    deleteUserButton->Show();
    formColourButton->Show();
    quitButton->Show();

    Layout();
}


// =========================================================
// QUIT
// =========================================================

void MainFrame::OnQuit(
    wxCommandEvent& event)
{
    Close(true);
}