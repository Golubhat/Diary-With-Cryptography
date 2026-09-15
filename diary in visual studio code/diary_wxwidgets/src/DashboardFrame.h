#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "UserManager.h"

class DashboardFrame : public wxFrame
{
public:
    DashboardFrame(
        wxWindow* parent,
        UserManager& userManager,
        int userId,
        const std::string& username);

private:
    enum Operation
    {
        NOTHING,
        WRITE,
        APPEND,
        READ,
        REMOVE,
        USERPASS
    };

    Operation mode = NOTHING;

    UserManager& userManager;
    int userId = 0;
    std::string username;

    int amountOfContent = 0;

    // -------------------------------------------------
    // Main panel
    // -------------------------------------------------

    wxPanel* panel = nullptr;

    // -------------------------------------------------
    // Main Dashboard
    // -------------------------------------------------

    wxStaticText* hello = nullptr;

    wxButton* writeANote = nullptr;
    wxButton* appendInNote = nullptr;
    wxButton* readANote = nullptr;
    wxButton* removeANote = nullptr;
    wxButton* changeUsernamePassword = nullptr;
    wxButton* logOut = nullptr;

    // -------------------------------------------------
    // Common
    // -------------------------------------------------

    wxButton* backToDashboard = nullptr;

    // -------------------------------------------------
    // Write a Note
    // -------------------------------------------------

    wxStaticText* newTitle = nullptr;
    wxTextCtrl* newTitleValue = nullptr;

    wxStaticText* newText = nullptr;
    wxTextCtrl* newTextValue = nullptr;

    wxButton* writeButton = nullptr;

    wxListCtrl* writeWhere = nullptr;

    wxStaticText* slotPick = nullptr;
    wxTextCtrl* slotPickValue = nullptr;

    wxButton* readSelectedNoteButton = nullptr;
    wxButton* appendSelectedNoteButton = nullptr;
    wxButton* removeSelectedNoteButton = nullptr;

    // -------------------------------------------------
    // Change Username / Password
    // -------------------------------------------------

    wxButton* changeCredentialsButton = nullptr;

    wxStaticText* newUsernameLabel = nullptr;
    wxStaticText* newPasswordLabel = nullptr;
    wxStaticText* confirmPasswordLabel = nullptr;

    wxTextCtrl* newUsernameValue = nullptr;
    wxTextCtrl* newPasswordValue = nullptr;
    wxTextCtrl* confirmPasswordValue = nullptr;

    // -------------------------------------------------
    // Event handlers
    // -------------------------------------------------

    void OnWriteANote(wxCommandEvent& event);
    void OnAppendInNote(wxCommandEvent& event);
    void OnReadANote(wxCommandEvent& event);
    void OnRemoveANote(wxCommandEvent& event);
    void OnChangeUsernamePassword(wxCommandEvent& event);
    void OnLogOut(wxCommandEvent& event);

    void OnBackToDashboard(wxCommandEvent& event);

    void OnWrite(wxCommandEvent& event);

    void OnReadSelectedNote(wxCommandEvent& event);
    void OnAppendSelectedNote(wxCommandEvent& event);
    void OnRemoveSelectedNote(wxCommandEvent& event);
    long GetSelectedNoteRow() const;

    void OnChangeCredentials(wxCommandEvent& event);

    // -------------------------------------------------
    // Screen management
    // -------------------------------------------------

    void ShowDashboard();
    void HideDashboardButtons();
    void HideWriteControls();

    // -------------------------------------------------
    // Colour management
    // -------------------------------------------------

    void ApplyColours();

    wxDECLARE_EVENT_TABLE();
};