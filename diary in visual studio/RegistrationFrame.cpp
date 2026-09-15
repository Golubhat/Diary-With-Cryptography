#include "RegistrationFrame.h"

enum
{
    ID_REGISTER = wxID_HIGHEST + 1,
    ID_BACK
};

wxBEGIN_EVENT_TABLE(RegistrationFrame, wxFrame)

EVT_BUTTON(
    ID_REGISTER,
    RegistrationFrame::OnRegister)

    EVT_BUTTON(
        ID_BACK,
        RegistrationFrame::OnBack)

    wxEND_EVENT_TABLE()


    RegistrationFrame::RegistrationFrame(
        wxWindow* parent,
        UserManager& userManager)
    : wxFrame(
        parent,
        wxID_ANY,
        "Diary - Register",
        wxDefaultPosition,
        wxSize(640, 480)),
    userManager(userManager)
{
    // -------------------------------------------------
    // Panel
    // -------------------------------------------------

    panel = new wxPanel(this);

    // -------------------------------------------------
    // Username
    // -------------------------------------------------

    usernameLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "Username:",
        wxPoint(150, 80));

    usernameCtrl = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(280, 75),
        wxSize(220, 30));

    // -------------------------------------------------
    // Password
    // -------------------------------------------------

    passwordLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "Password:",
        wxPoint(150, 135));

    passwordCtrl = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(280, 130),
        wxSize(220, 30),
        wxTE_PASSWORD);

    // -------------------------------------------------
    // Confirm password
    // -------------------------------------------------

    confirmPasswordLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "Confirm Password:",
        wxPoint(150, 190));

    confirmPasswordCtrl = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(280, 185),
        wxSize(220, 30),
        wxTE_PASSWORD);

    // -------------------------------------------------
    // Register button
    //
    // Native button colours are deliberately preserved.
    // -------------------------------------------------

    new wxButton(
        panel,
        ID_REGISTER,
        "Register",
        wxPoint(220, 260),
        wxSize(180, 40));

    // -------------------------------------------------
    // Back button
    //
    // Native button colours are deliberately preserved.
    // -------------------------------------------------

    new wxButton(
        panel,
        ID_BACK,
        "Go Back",
        wxPoint(220, 320),
        wxSize(180, 40));

    // -------------------------------------------------
    // Apply saved form colours
    // -------------------------------------------------

    ApplyColours();

    Centre();
}


// =====================================================
// APPLY FORM COLOURS
// =====================================================

void RegistrationFrame::ApplyColours()
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
    //
    // Foreground colour is used ONLY for labels.
    // -------------------------------------------------

    if (usernameLabel != nullptr)
    {
        usernameLabel->SetForegroundColour(foreground);
    }

    if (passwordLabel != nullptr)
    {
        passwordLabel->SetForegroundColour(foreground);
    }

    if (confirmPasswordLabel != nullptr)
    {
        confirmPasswordLabel->SetForegroundColour(
            foreground);
    }

    // -------------------------------------------------
    // Deliberately do NOT change:
    //
    // wxButton colours
    // wxTextCtrl colours
    // -------------------------------------------------

    Refresh();

    if (panel != nullptr)
    {
        panel->Refresh();
    }
}


// =====================================================
// REGISTER
// =====================================================

void RegistrationFrame::OnRegister(
    wxCommandEvent& event)
{
    const wxString username =
        usernameCtrl->GetValue();

    const wxString password =
        passwordCtrl->GetValue();

    const wxString confirmPassword =
        confirmPasswordCtrl->GetValue();

    if (username.IsEmpty() ||
        password.IsEmpty() ||
        confirmPassword.IsEmpty())
    {
        wxMessageBox(
            "Please enter all registration details.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    if (password != confirmPassword)
    {
        wxMessageBox(
            "Passwords do not match.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    int newUserId = 0;

    if (!userManager.RegisterUser(
        username.ToStdString(),
        password.ToStdString(),
        newUserId))
    {
        wxMessageBox(
            "Unable to register the user.",
            "Diary",
            wxOK | wxICON_ERROR);

        return;
    }

    wxMessageBox(
        wxString::Format(
            "User registered successfully.\n\n"
            "Your User ID is: %d",
            newUserId),
        "Diary",
        wxOK | wxICON_INFORMATION);

    Close(true);
}


// =====================================================
// GO BACK
// =====================================================

void RegistrationFrame::OnBack(
    wxCommandEvent& event)
{
    Close(true);
}