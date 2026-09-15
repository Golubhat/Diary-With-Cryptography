#pragma once

#include <wx/wx.h>
#include "UserManager.h"

class RegistrationFrame : public wxFrame
{
public:
    RegistrationFrame(
        wxWindow* parent,
        UserManager& userManager);

private:
    void OnRegister(wxCommandEvent& event);
    void OnBack(wxCommandEvent& event);

    void ApplyColours();

    UserManager& userManager;

    wxPanel* panel = nullptr;

    wxStaticText* usernameLabel = nullptr;
    wxStaticText* passwordLabel = nullptr;
    wxStaticText* confirmPasswordLabel = nullptr;

    wxTextCtrl* usernameCtrl = nullptr;
    wxTextCtrl* passwordCtrl = nullptr;
    wxTextCtrl* confirmPasswordCtrl = nullptr;

    wxDECLARE_EVENT_TABLE();
};