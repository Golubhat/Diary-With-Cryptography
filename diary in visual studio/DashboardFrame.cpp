#include "DashboardFrame.h"

enum
{
    ID_WRITE_NOTE = wxID_HIGHEST + 100,
    ID_APPEND_NOTE,
    ID_READ_NOTE,
    ID_REMOVE_NOTE,
    ID_CHANGE_USERPASS,
    ID_LOGOUT,

    ID_BACK_DASHBOARD,
    ID_WRITE,
    ID_READ_SELECTED_NOTE,
    ID_APPEND_SELECTED_NOTE,
    ID_REMOVE_SELECTED_NOTE,
    ID_CHANGE_CREDENTIALS
};

wxBEGIN_EVENT_TABLE(DashboardFrame, wxFrame)

EVT_BUTTON(ID_WRITE_NOTE,
    DashboardFrame::OnWriteANote)

    EVT_BUTTON(ID_APPEND_NOTE,
        DashboardFrame::OnAppendInNote)

    EVT_BUTTON(ID_READ_NOTE,
        DashboardFrame::OnReadANote)

    EVT_BUTTON(ID_REMOVE_NOTE,
        DashboardFrame::OnRemoveANote)

    EVT_BUTTON(ID_CHANGE_USERPASS,
        DashboardFrame::OnChangeUsernamePassword)

    EVT_BUTTON(ID_LOGOUT,
        DashboardFrame::OnLogOut)

    EVT_BUTTON(ID_BACK_DASHBOARD,
        DashboardFrame::OnBackToDashboard)

    EVT_BUTTON(ID_WRITE,
        DashboardFrame::OnWrite)

    EVT_BUTTON(
        ID_READ_SELECTED_NOTE,
        DashboardFrame::OnReadSelectedNote)

    EVT_BUTTON(
        ID_APPEND_SELECTED_NOTE,
        DashboardFrame::OnAppendSelectedNote)

    EVT_BUTTON(
        ID_REMOVE_SELECTED_NOTE,
        DashboardFrame::OnRemoveSelectedNote)

    EVT_BUTTON(
        ID_CHANGE_CREDENTIALS,
        DashboardFrame::OnChangeCredentials)
    
    wxEND_EVENT_TABLE()


    // =====================================================
    // Constructor
    // =====================================================

    DashboardFrame::DashboardFrame(
        wxWindow* parent,
        UserManager& userManager,
        int userId,
        const std::string& username)
    : wxFrame(
        parent,
        wxID_ANY,
        "Diary - Dashboard",
        wxDefaultPosition,
        wxSize(640, 480)),
    userManager(userManager),
    userId(userId),
    username(username)
{
    panel = new wxPanel(this);

    // =================================================
    // Welcome message
    // =================================================

    hello = new wxStaticText(
        panel,
        wxID_ANY,
        wxString::Format(
            "Welcome %s :)",
            username),
        wxPoint(0, 20),
        wxSize(640, 55),
        wxALIGN_CENTER);

    wxFont helloFont(
        16,
        wxFONTFAMILY_SWISS,
        wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_NORMAL);

    hello->SetFont(helloFont);

    // =================================================
    // Main Dashboard buttons
    // =================================================

    writeANote = new wxButton(
        panel,
        ID_WRITE_NOTE,
        "Write a Note",
        wxPoint(230, 100),
        wxSize(180, 35));

    appendInNote = new wxButton(
        panel,
        ID_APPEND_NOTE,
        "Append in Note",
        wxPoint(230, 145),
        wxSize(180, 35));

    readANote = new wxButton(
        panel,
        ID_READ_NOTE,
        "Read a Note",
        wxPoint(230, 190),
        wxSize(180, 35));

    removeANote = new wxButton(
        panel,
        ID_REMOVE_NOTE,
        "Remove a Note",
        wxPoint(230, 235),
        wxSize(180, 35));

    changeUsernamePassword = new wxButton(
        panel,
        ID_CHANGE_USERPASS,
        "Change Username/Password",
        wxPoint(210, 285),
        wxSize(220, 35));

    logOut = new wxButton(
        panel,
        ID_LOGOUT,
        "Log Out",
        wxPoint(230, 335),
        wxSize(180, 35));

    // =================================================
    // Back to Dashboard
    // =================================================

    backToDashboard = new wxButton(
        panel,
        ID_BACK_DASHBOARD,
        "Go Back",
        wxPoint(20, 15),
        wxSize(100, 35));

    // =================================================
    // WRITE SCREEN
    // =================================================

    newTitle = new wxStaticText(
        panel,
        wxID_ANY,
        "Enter Title:",
        wxPoint(30, 80),
        wxSize(100, 30));

    newTitleValue = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(140, 75),
        wxSize(500, 35));

    newText = new wxStaticText(
        panel,
        wxID_ANY,
        "Enter Text:",
        wxPoint(30, 130),
        wxSize(100, 30));

    newTextValue = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(140, 125),
        wxSize(500, 210),
        wxTE_MULTILINE |
        wxTE_WORDWRAP);

    writeButton = new wxButton(
        panel,
        ID_WRITE,
        "Write",
        wxPoint(140, 355),
        wxSize(140, 40));

    // =================================================
    // List of existing notes
    // =================================================

    writeWhere = new wxListCtrl(
        panel,
        wxID_ANY,
        wxPoint(680, 75),
        wxSize(280, 220),
        wxLC_REPORT |
        wxLC_SINGLE_SEL |
        wxLC_HRULES |
        wxLC_VRULES);

    writeWhere->InsertColumn(
        0,
        "No.",
        wxLIST_FORMAT_LEFT,
        40);

    writeWhere->InsertColumn(
        1,
        "Title",
        wxLIST_FORMAT_CENTER,
        240);

    std::vector<std::string> titles;

    if (userManager.LoadNoteTitles(
        userId,
        titles))
    {
        amountOfContent =
            static_cast<int>(titles.size());

        for (std::size_t i = 0;
            i < titles.size();
            ++i)
        {
            const long row =
                writeWhere->InsertItem(
                    static_cast<long>(i),
                    wxString::Format(
                        "%zu",
                        i + 1));

            writeWhere->SetItem(
                row,
                1,
                wxString::FromUTF8(
                    titles[i]));
        }
    }

    slotPick = new wxStaticText(
        panel,
        wxID_ANY,
        "Where to insert\n"
        "from 1 to N:",
        wxPoint(680, 315),
        wxSize(150, 50));

    slotPickValue = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(840, 315),
        wxSize(70, 35));

    readSelectedNoteButton = new wxButton(
        panel,
        ID_READ_SELECTED_NOTE,
        "Read",
        wxPoint(680, 315),
        wxSize(230, 40));

    readSelectedNoteButton->Hide();

    appendSelectedNoteButton = new wxButton(
        panel,
        ID_APPEND_SELECTED_NOTE,
        "Append",
        wxPoint(680, 315),
        wxSize(230, 40));

    appendSelectedNoteButton->Hide();

    removeSelectedNoteButton = new wxButton(
        panel,
        ID_REMOVE_SELECTED_NOTE,
        "Remove",
        wxPoint(205, 325),
        wxSize(230, 40));

    removeSelectedNoteButton->Hide();

    newUsernameLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "New Username:",
        wxPoint(125, 110));

    newPasswordLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "New Password:",
        wxPoint(125, 165));

    confirmPasswordLabel = new wxStaticText(
        panel,
        wxID_ANY,
        "Confirm Password:",
        wxPoint(125, 220));

    newUsernameLabel->Hide();
    newPasswordLabel->Hide();
    confirmPasswordLabel->Hide();

    newUsernameValue = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(275, 105),
        wxSize(220, 30));

    newPasswordValue = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(275, 160),
        wxSize(220, 30),
        wxTE_PASSWORD);

    confirmPasswordValue = new wxTextCtrl(
        panel,
        wxID_ANY,
        "",
        wxPoint(275, 215),
        wxSize(220, 30),
        wxTE_PASSWORD);

    changeCredentialsButton = new wxButton(
        panel,
        ID_CHANGE_CREDENTIALS,
        "Change",
        wxPoint(225, 290),
        wxSize(180, 40));

    newUsernameValue->Hide();
    newPasswordValue->Hide();
    confirmPasswordValue->Hide();

    changeCredentialsButton->Hide();


    // =================================================
    // Initially hide operation controls
    // =================================================

    backToDashboard->Hide();

    HideWriteControls();

    ApplyColours();

    Centre();
}

// =====================================================
// APPLY FORM COLOURS
// =====================================================

void DashboardFrame::ApplyColours()
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
    // Foreground colour = text colour.
    // -------------------------------------------------

    if (hello != nullptr)
    {
        hello->SetForegroundColour(foreground);
    }

    if (newTitle != nullptr)
    {
        newTitle->SetForegroundColour(foreground);
    }

    if (newText != nullptr)
    {
        newText->SetForegroundColour(foreground);
    }

    if (slotPick != nullptr)
    {
        slotPick->SetForegroundColour(foreground);
    }

    if (newUsernameLabel != nullptr)
    {
        newUsernameLabel->SetForegroundColour(foreground);
    }

    if (newPasswordLabel != nullptr)
    {
        newPasswordLabel->SetForegroundColour(foreground);
    }

    if (confirmPasswordLabel != nullptr)
    {
        confirmPasswordLabel->SetForegroundColour(
            foreground);
    }

    // -------------------------------------------------
    // IMPORTANT:
    //
    // Do NOT modify button colours.
    // Do NOT modify text-control colours.
    // Do NOT modify list-box colours.
    // -------------------------------------------------

    Refresh();

    if (panel != nullptr)
    {
        panel->Refresh();
    }
}

// =====================================================
// Show Dashboard
// =====================================================

void DashboardFrame::ShowDashboard()
{
    mode = NOTHING;

    SetSize(640, 480);

    HideWriteControls();

    hello->Show();

    writeANote->Show();
    appendInNote->Show();
    readANote->Show();
    removeANote->Show();
    changeUsernamePassword->Show();
    logOut->Show();

    backToDashboard->Hide();

    Layout();
    Centre();
}


// =====================================================
// Hide Dashboard buttons
// =====================================================

void DashboardFrame::HideDashboardButtons()
{
    hello->Hide();

    writeANote->Hide();
    appendInNote->Hide();
    readANote->Hide();
    removeANote->Hide();
    changeUsernamePassword->Hide();
    logOut->Hide();
}


// =====================================================
// Hide Write controls
// =====================================================

void DashboardFrame::HideWriteControls()
{
    newTitle->Hide();
    newTitleValue->Hide();

    newText->Hide();
    newTextValue->Hide();

    writeButton->Hide();

    writeWhere->Hide();

    slotPick->Hide();
    slotPickValue->Hide();

    readSelectedNoteButton->Hide();
    appendSelectedNoteButton->Hide();

    removeSelectedNoteButton->Hide();

    newUsernameLabel->Hide();
    newPasswordLabel->Hide();
    confirmPasswordLabel->Hide();

    newUsernameValue->Hide();
    newPasswordValue->Hide();
    confirmPasswordValue->Hide();

    changeCredentialsButton->Hide();
}


// =====================================================
// WRITE A NOTE
// =====================================================

void DashboardFrame::OnWriteANote(
    wxCommandEvent& event)
{
    mode = WRITE;

    HideDashboardButtons();

    SetSize(1000, 480);

    backToDashboard->Show();

    // ---------------------------------------------
    // Hide anything belonging to Read mode first.
    // ---------------------------------------------

    readSelectedNoteButton->Hide();

    // ---------------------------------------------
    // Write controls
    // ---------------------------------------------

    newTitle->Show();
    newTitleValue->Show();

    newText->Show();
    newTextValue->Show();
    newText->SetPosition(wxPoint(30, 130));
    newTextValue->SetPosition(wxPoint(140, 125));
    newTextValue->SetSize(wxSize(500, 210));

    writeButton->Show();

    writeWhere->SetPosition(wxPoint(680, 75));
    writeWhere->Show();

    slotPick->Show();
    slotPickValue->Show();

    // Make the fields editable again.
    newTitleValue->SetEditable(true);
    newTextValue->SetEditable(true);

    // Clear previous Read Note contents.
    newTitleValue->Clear();
    newTextValue->Clear();

    Layout();
    Centre();
}


// =====================================================
// WRITE button
// =====================================================

void DashboardFrame::OnWrite(wxCommandEvent& event)
{
    const wxString title =
        newTitleValue->GetValue();

    const wxString text =
        newTextValue->GetValue();

    if (title.IsEmpty())
    {
        wxMessageBox(
            "Please enter a title.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    if (text.IsEmpty())
    {
        wxMessageBox(
            "Please enter some text.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    long positionValue = 1;

    // If there are already notes, the user must
    // specify where the new note should go.
    if (amountOfContent > 0)
    {
        const wxString positionText =
            slotPickValue->GetValue();

        if (positionText.IsEmpty() ||
            !positionText.ToLong(&positionValue))
        {
            wxMessageBox(
                "Please enter a valid position.",
                "Diary",
                wxOK | wxICON_WARNING);

            return;
        }
    }

    if (positionValue < 1 ||
        positionValue > amountOfContent + 1)
    {
        wxMessageBox(
            wxString::Format(
                "Position must be between 1 and %d.",
                amountOfContent + 1),
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    // Use the EXISTING UserManager reference.
    // Do NOT create another UserManager here.
    if (!userManager.WriteNote(
        userId,
        title.ToStdString(),
        text.ToStdString(),
        static_cast<int>(positionValue)))
    {
        wxMessageBox(
            "Unable to write the note.",
            "Diary",
            wxOK | wxICON_ERROR);

        return;
    }

    amountOfContent++;

    // Add the newly written note's title to the list.
    const long row =
        writeWhere->InsertItem(
            positionValue - 1,
            wxString::Format(
                "%ld",
                positionValue));

    writeWhere->SetItem(
        row,
        1,
        title);

    for (long i = 0;
        i < writeWhere->GetItemCount();
        ++i)
    {
        writeWhere->SetItem(
            i,
            0,
            wxString::Format(
                "%ld",
                i + 1));
    }

    wxMessageBox(
        "Note written successfully.",
        "Diary",
        wxOK | wxICON_INFORMATION);

    newTitleValue->Clear();
    newTextValue->Clear();
    slotPickValue->Clear();
}

long DashboardFrame::GetSelectedNoteRow() const
{
    if (writeWhere == nullptr)
        return -1;

    return writeWhere->GetNextItem(
        -1,
        wxLIST_NEXT_ALL,
        wxLIST_STATE_SELECTED);
}


// =====================================================
// APPEND
// =====================================================

void DashboardFrame::OnAppendInNote(
    wxCommandEvent& event)
{
    if (amountOfContent == 0)
    {
        wxMessageBox(
            "There are no notes to append to.",
            "Diary",
            wxOK | wxICON_INFORMATION);

        return;
    }

    mode = APPEND;

    HideDashboardButtons();

    SetSize(1000, 520);

    backToDashboard->Show();

    // ---------------------------------------------
    // Text area
    // ---------------------------------------------

    newText->Show();
    newTextValue->Show();
    newText->SetPosition(wxPoint(30, 80));
    newTextValue->SetPosition(wxPoint(140, 75));
    newTextValue->SetSize(wxSize(500, 280));

    newTextValue->Clear();
    newTextValue->SetEditable(true);

    // ---------------------------------------------
    // Note list
    // ---------------------------------------------

    writeWhere->SetPosition(wxPoint(680, 75));
    writeWhere->Show();

    // ---------------------------------------------
    // Append button
    // ---------------------------------------------

    appendSelectedNoteButton->Show();

    // ---------------------------------------------
    // Hide everything not needed in Append mode
    // ---------------------------------------------

    newTitle->Hide();
    newTitleValue->Hide();

    writeButton->Hide();
    readSelectedNoteButton->Hide();

    slotPick->Hide();
    slotPickValue->Hide();

    Layout();
    Centre();
}

void DashboardFrame::OnAppendSelectedNote(
    wxCommandEvent& event)
{
    const long selection =
        GetSelectedNoteRow();

    if (selection == wxNOT_FOUND)
    {
        wxMessageBox(
            "Please select a note from the list.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    const wxString text =
        newTextValue->GetValue();

    if (text.IsEmpty())
    {
        wxMessageBox(
            "Please enter text to append.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    const int position =
        selection + 1;

    if (!userManager.AppendToNote(
        userId,
        position,
        text.ToStdString()))
    {
        wxMessageBox(
            "Unable to append to the selected note.",
            "Diary",
            wxOK | wxICON_ERROR);

        return;
    }

    wxMessageBox(
        "Text appended successfully.",
        "Diary",
        wxOK | wxICON_INFORMATION);

    newTextValue->Clear();
}


// =====================================================
// READ
// =====================================================

void DashboardFrame::OnReadANote(
    wxCommandEvent& event)
{
    if (amountOfContent == 0)
    {
        wxMessageBox(
            "There are no notes to read.",
            "Diary",
            wxOK | wxICON_INFORMATION);

        return;
    }

    mode = READ;

    HideDashboardButtons();

    SetSize(1000, 480);

    backToDashboard->Show();

    // ---------------------------------------------
    // Read controls
    // ---------------------------------------------

    writeWhere->SetPosition(wxPoint(680, 75));
    writeWhere->Show();

    readSelectedNoteButton->Show();

    newTitle->Show();
    newTitleValue->Show();

    newText->Show();
    newTextValue->Show();
    newText->SetPosition(wxPoint(30, 130));
    newTextValue->SetPosition(wxPoint(140, 125));
    newTextValue->SetSize(wxSize(500, 210));

    // ---------------------------------------------
    // Hide Write-only controls
    // ---------------------------------------------

    writeButton->Hide();

    slotPick->Hide();
    slotPickValue->Hide();

    // Make fields display-only.
    newTitleValue->SetEditable(false);
    newTextValue->SetEditable(false);

    newTitleValue->Clear();
    newTextValue->Clear();

    Layout();
    Centre();
}

void DashboardFrame::OnReadSelectedNote(
    wxCommandEvent& event)
{
    const long selection =
        GetSelectedNoteRow();

    if (selection == wxNOT_FOUND)
    {
        wxMessageBox(
            "Please select a note from the list.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    const int position =
        selection + 1;

    std::string title;
    std::string text;

    if (!userManager.ReadNote(
        userId,
        position,
        title,
        text))
    {
        wxMessageBox(
            "Unable to read the selected note.",
            "Diary",
            wxOK | wxICON_ERROR);

        return;
    }

    // Put the note title into the title field.
    newTitleValue->SetValue(
        wxString::FromUTF8(title));

    // Put the note text into the large text area.
    newTextValue->SetValue(
        wxString::FromUTF8(text));
}


// =====================================================
// REMOVE
// =====================================================

void DashboardFrame::OnRemoveANote(
    wxCommandEvent& event)
{
    if (amountOfContent == 0)
    {
        wxMessageBox(
            "There are no notes to remove.",
            "Diary",
            wxOK | wxICON_INFORMATION);

        return;
    }

    mode = REMOVE;

    HideDashboardButtons();

    SetSize(640, 480);

    backToDashboard->Show();

    // Show note list.
    writeWhere->SetPosition(wxPoint(180, 75));
    writeWhere->Show();

    // Show Remove button.
    removeSelectedNoteButton->Show();

    // Hide everything else.
    newTitle->Hide();
    newTitleValue->Hide();

    newText->Hide();
    newTextValue->Hide();

    writeButton->Hide();
    readSelectedNoteButton->Hide();
    appendSelectedNoteButton->Hide();

    slotPick->Hide();
    slotPickValue->Hide();

    Layout();
    Centre();
}

void DashboardFrame::OnRemoveSelectedNote(
    wxCommandEvent& event)
{
    const long selection =
        GetSelectedNoteRow();

    if (selection == wxNOT_FOUND)
    {
        wxMessageBox(
            "Please select a note to remove.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    const int position =
        selection + 1;

    const wxString title =
        writeWhere->GetItemText(
            selection,
            1);

    // Confirmation.
    const int answer =
        wxMessageBox(
            wxString::Format(
                "Are you sure you want to remove:\n\n"
                "\"%s\"?",
                title),
            "Remove Note",
            wxYES_NO | wxICON_WARNING);

    if (answer != wxYES)
        return;

    if (!userManager.RemoveNote(
        userId,
        position))
    {
        wxMessageBox(
            "Unable to remove the selected note.",
            "Diary",
            wxOK | wxICON_ERROR);

        return;
    }

    amountOfContent--;

    // Remove from the visible list.
    writeWhere->DeleteItem(selection);

    for (long i = 0;
        i < writeWhere->GetItemCount();
        ++i)
    {
        writeWhere->SetItem(
            i,
            0,
            wxString::Format(
                "%ld",
                i + 1));
    }

    wxMessageBox(
        "Note removed successfully.",
        "Diary",
        wxOK | wxICON_INFORMATION);

    // If there are no notes left, return to Dashboard.
    if (amountOfContent == 0)
    {
        ShowDashboard();
    }
}


// =====================================================
// CHANGE USERNAME / PASSWORD
// =====================================================

void DashboardFrame::OnChangeUsernamePassword(
    wxCommandEvent& event)
{
    HideDashboardButtons();

    HideWriteControls();

    SetSize(640, 480);

    backToDashboard->Show();

    newUsernameLabel->Show();
    newPasswordLabel->Show();
    confirmPasswordLabel->Show();

    newUsernameValue->Show();
    newPasswordValue->Show();
    confirmPasswordValue->Show();

    changeCredentialsButton->Show();

    newUsernameValue->Clear();
    newPasswordValue->Clear();
    confirmPasswordValue->Clear();

    Layout();
    Centre();
}

void DashboardFrame::OnChangeCredentials(
    wxCommandEvent& event)
{
    const wxString newUsername =
        newUsernameValue->GetValue();

    const wxString newPassword =
        newPasswordValue->GetValue();

    const wxString confirmPassword =
        confirmPasswordValue->GetValue();

    if (newUsername.IsEmpty() ||
        newPassword.IsEmpty() ||
        confirmPassword.IsEmpty())
    {
        wxMessageBox(
            "Please fill in all fields.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    if (newPassword != confirmPassword)
    {
        wxMessageBox(
            "Passwords don't match.",
            "Diary",
            wxOK | wxICON_WARNING);

        return;
    }

    if (!userManager.ChangeUsernamePassword(
        userId,
        newUsername.ToStdString(),
        newPassword.ToStdString()))
    {
        wxMessageBox(
            "Unable to change username and password.",
            "Diary",
            wxOK | wxICON_ERROR);

        return;
    }

    wxMessageBox(
        "Username and password changed successfully.",
        "Diary",
        wxOK | wxICON_INFORMATION);

    newUsernameValue->Clear();
    newPasswordValue->Clear();
    confirmPasswordValue->Clear();

    ShowDashboard();
}


// =====================================================
// LOG OUT
// =====================================================

void DashboardFrame::OnLogOut(
    wxCommandEvent& event)
{
    if (!userManager.Logout(userId))
    {
        wxMessageBox(
            "Unable to log out safely.",
            "Logout Error",
            wxOK | wxICON_ERROR);

        return;
    }

    ShowDashboard();

    Close(true);
}


// =====================================================
// BACK TO DASHBOARD
// =====================================================

void DashboardFrame::OnBackToDashboard(
    wxCommandEvent& event)
{
    ShowDashboard();
}