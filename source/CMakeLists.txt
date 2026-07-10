cmake_minimum_required(VERSION 3.10)
project(MH3U_Save_Editor_GUI)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

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
    ../../../MH3U\ Save\ Editor/MH3U\ Save\ Editor/mh3u_se.cpp
    ../../../MH3U\ Save\ Editor/MH3U\ Save\ Editor/mh3u_ds.cpp
)

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
    widget/qutil.hpp
    ../../../MH3U\ Save\ Editor/MH3U\ Save\ Editor/mh3u_se.hpp
    ../../../MH3U\ Save\ Editor/MH3U\ Save\ Editor/mh3u_ds.hpp
)

add_executable(mh3u_se_gui ${SOURCES} ${HEADERS})
target_link_libraries(mh3u_se_gui Qt5::Widgets)
target_include_directories(mh3u_se_gui PRIVATE . ../../../MH3U\ Save\ Editor/MH3U\ Save\ Editor/)