#include "LoginFrame.h"
#include "DashboardFrame.h"

enum
{
    ID_LOGIN = wxID_HIGHEST + 1,
    ID_BACK
};

wxBEGIN_EVENT_TABLE(LoginFrame, wxFrame)

EVT_BUTTON(
    ID_LOGIN,
    LoginFrame::OnLogin)

    EVT_BUTTON(
        ID_BACK,
        LoginFrame::OnBack)

    wxEND_EVENT_TABLE()


    LoginFrame::LoginFrame(
        wxWindow* parent,
        UserManager& userManager)
    : wxFrame(
        parent,
        wxID_ANY,
        "Diary - Log In",
        wxDefaultPosition,
        wxSize(640, 480)),
    userManager(userManager)
{
    // -------------------------------------------------
    // Panel
    // -------------------------------------------------

    panel = new wxPanel(this);

    // -------------------------------------------------
    // ID
    // -------------------------------------------------

    idLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "ID:",
        wxPoint(150, 80));

    idCtrl = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(280, 75),
        wxSize(220, 30));

    // -------------------------------------------------
    // Username
    // -------------------------------------------------

    usernameLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "Username:",
        wxPoint(150, 135));

    usernameCtrl = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(280, 130),
        wxSize(220, 30));

    // -------------------------------------------------
    // Password
    // -------------------------------------------------

    passwordLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "Password:",
        wxPoint(150, 190));

    passwordCtrl = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(280, 185),
        wxSize(220, 30),
        wxTE_PASSWORD);

    // -------------------------------------------------
    // Log In button
    //
    // Native button colours are deliberately preserved.
    // -------------------------------------------------

    new wxButton(
        panel,
        ID_LOGIN,
        "Log In",
        wxPoint(220, 260),
        wxSize(180, 40));

    // -------------------------------------------------
    // Go Back button
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

void LoginFrame::ApplyColours()
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
    // Foreground colour is used ONLY for text labels.
    // -------------------------------------------------

    if (idLabel != nullptr)
    {
        idLabel->SetForegroundColour(foreground);
    }

    if (usernameLabel != nullptr)
    {
        usernameLabel->SetForegroundColour(foreground);
    }

    if (passwordLabel != nullptr)
    {
        passwordLabel->SetForegroundColour(foreground);
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
// LOGIN
// =====================================================

void LoginFrame::OnLogin(
    wxCommandEvent& event)
{
    const wxString idText =
        idCtrl->GetValue();

    const wxString username =
        usernameCtrl->GetValue();

    const wxString password =
        passwordCtrl->GetValue();

    if (idText.IsEmpty() ||
        username.IsEmpty() ||
        password.IsEmpty())
    {
        wxMessageBox(
            "Please enter all login details.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    long idValue = 0;

    if (!idText.ToLong(&idValue))
    {
        wxMessageBox(
            "ID must be a number.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    const int userId =
        static_cast<int>(idValue);

    if (!userManager.Login(
        userId,
        username.ToStdString(),
        password.ToStdString()))
    {
        wxMessageBox(
            "Login unsuccessful.",
            "Diary",
            wxOK | wxICON_ERROR);

        return;
    }

    // -------------------------------------------------
    // Login successful
    // -------------------------------------------------

    auto* dashboard =
        new DashboardFrame(
            this,
            userManager,
            userId,
            username.ToStdString());

    dashboard->Show();

    Hide();
}


// =====================================================
// GO BACK
// =====================================================

void LoginFrame::OnBack(
    wxCommandEvent& event)
{
    Close(true);
}