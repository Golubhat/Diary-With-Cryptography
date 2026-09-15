#pragma once

#include <wx/wx.h>
#include "UserManager.h"

class DiaryApp : public wxApp
{
public:
    virtual bool OnInit() override;
    virtual int OnExit() override;

private:
    UserManager userManager;
};

wxDECLARE_APP(DiaryApp);