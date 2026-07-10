cmake_minimum_required(VERSION 3.10)
project(MH3U_Save_Editor_GUI)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(HEADERS
    main.hpp
    mh3u_sv.hpp
    widget/qcharacter.hpp
    widget/qinventory.hpp
    widget/qpouch.hpp
    widget/qchest.hpp
    widget/qbox.hpp
    widget/qitem.hpp
    widget/qequipment.hpp
    widget/qarmor.hpp
    widget/qcharm.hpp
    widget/qweapon.hpp
    widget/qoption.hpp
)

qt5_wrap_cpp(MOC_SOURCES ${HEADERS})

set(SOURCES
    main.cpp
    mh3u_sv.cpp
    widget/qcharacter.cpp
    widget/qinventory.cpp
    widget/qpouch.cpp
    widget/qchest.cpp
    widget/qbox.cpp
    widget/qitem.cpp
    widget/qequipment.cpp
    widget/qarmor.cpp
    widget/qcharm.cpp
    widget/qweapon.cpp
    widget/qoption.cpp
    widget/qutil.cpp
    "../../../../source/app/MH3U_Save_Editor/mh3u_se.cpp"
    "../../../../source/app/MH3U_Save_Editor/mh3u_ds.cpp"
)

add_executable(mh3u_se_gui ${SOURCES} ${MOC_SOURCES})
target_link_libraries(mh3u_se_gui Qt5::Widgets)
target_include_directories(mh3u_se_gui PRIVATE . "../../../../source/app/MH3U_Save_Editor/")