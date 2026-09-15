cmake_minimum_required(VERSION 3.16)

project(Diary
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base)

include(${wxWidgets_USE_FILE})

add_executable(Diary WIN32
    src/DiaryApp.cpp
    src/DiaryApp.h

    src/MainFrame.cpp
    src/MainFrame.h

    src/LoginFrame.cpp
    src/LoginFrame.h

    src/RegistrationFrame.cpp
    src/RegistrationFrame.h

    src/DashboardFrame.cpp
    src/DashboardFrame.h

    src/UserManager.cpp
    src/UserManager.h

    src/Encryption.cpp
    src/Encryption.h

    src/BackupManager.cpp
    src/BackupManager.h
)

target_link_libraries(Diary
    PRIVATE
    ${wxWidgets_LIBRARIES}
)

if(WIN32)
    target_sources(Diary PRIVATE
        src/Diary.manifest
    )

    set_target_properties(Diary PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()