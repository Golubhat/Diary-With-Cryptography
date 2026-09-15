#pragma once

#include <wx/wx.h>
#include "UserManager.h"

class LoginFrame : public wxFrame
{
public:
    LoginFrame(
        wxWindow* parent,
        UserManager& userManager);

private:
    void OnLogin(wxCommandEvent& event);
    void OnBack(wxCommandEvent& event);

    void ApplyColours();

    UserManager& userManager;

    wxPanel* panel = nullptr;

    wxStaticText* idLabel = nullptr;
    wxStaticText* usernameLabel = nullptr;
    wxStaticText* passwordLabel = nullptr;

    wxTextCtrl* idCtrl = nullptr;
    wxTextCtrl* usernameCtrl = nullptr;
    wxTextCtrl* passwordCtrl = nullptr;

    wxDECLARE_EVENT_TABLE();
};