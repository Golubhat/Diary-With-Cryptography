#include "DiaryApp.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(DiaryApp);

bool DiaryApp::OnInit()
{
    if (!userManager.Initialize())
    {
        wxMessageBox(
            "Unable to initialize Diary data.",
            "Diary",
            wxOK | wxICON_ERROR);

        return false;
    }

    auto* mainFrame =
        new MainFrame(nullptr, userManager);

    mainFrame->Show(true);

    return true;
}

int DiaryApp::OnExit()
{
    if (!userManager.EncryptSettings())
    {
        wxMessageBox(
            "Warning: settings.dat could not be encrypted.",
            "Diary",
            wxOK | wxICON_WARNING);
    }

    if (!userManager.RemoveSettingsBackup())
    {
        wxMessageBox(
            "Warning: settings.dat backup could not be removed.",
            "Diary",
            wxOK | wxICON_WARNING);
    }

    return wxApp::OnExit();
}