#pragma once

#include <wx/wx.h>
#include <cstdint>

#include "UserManager.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(
        wxWindow* parent,
        UserManager& userManager);

private:
    UserManager& userManager;

    // -------------------------------------------------
    // Main menu
    // -------------------------------------------------

    void OnButton(wxCommandEvent& event);
    void OnDeleteUser(wxCommandEvent& event);
    void OnFormColour(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

    // -------------------------------------------------
    // Form Colour
    // -------------------------------------------------

    void OnFormColourChange(wxCommandEvent& event);
    void OnBackToMainMenu(wxCommandEvent& event);

    // -------------------------------------------------
    // Colour handling
    // -------------------------------------------------

    void ApplyColours();

    // -------------------------------------------------
    // Main panel
    // -------------------------------------------------

    wxPanel* panel = nullptr;

    // -------------------------------------------------
    // Form Colour controls
    // -------------------------------------------------

    wxStaticText* backgroundColourLabel = nullptr;
    wxChoice* backgroundColourChoice = nullptr;

    wxStaticText* foregroundColourLabel = nullptr;
    wxChoice* foregroundColourChoice = nullptr;

    wxButton* changeFormColourButton = nullptr;
    wxButton* backToMainMenuButton = nullptr;

    // -------------------------------------------------
    // Main menu controls
    // -------------------------------------------------

    wxStaticText* welcome = nullptr;

    wxButton* loginButton = nullptr;
    wxButton* registerButton = nullptr;
    wxButton* deleteUserButton = nullptr;
    wxButton* formColourButton = nullptr;
    wxButton* quitButton = nullptr;

    wxDECLARE_EVENT_TABLE();
};