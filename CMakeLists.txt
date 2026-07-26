cmake_minimum_required(VERSION 3.10)

project(myschedule_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(COMMON_SOURCES
    src/Task.cpp
    src/TaskManager.cpp
    src/UserManager.cpp
    src/Utils.cpp
    src/Reminder.cpp
    src/SoundReminder.cpp
    src/VoiceTaskParser.cpp
)

add_executable(myschedule
    src/main.cpp
    ${COMMON_SOURCES}
)

target_include_directories(myschedule PRIVATE
    ${PROJECT_INCLUDE_DIR}
)

target_link_libraries(myschedule PRIVATE
    Threads::Threads
)

if (WIN32)
    target_link_libraries(myschedule PRIVATE winmm)
endif()

# Copy runtime resources to build output directory.
# This makes commands such as:
#   ./myschedule user1 123456 voiceadd --file assets/voice_task.wav
# easier to run from the build directory.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/scripts")
    add_custom_command(
        TARGET myschedule POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/scripts"
                "$<TARGET_FILE_DIR:myschedule>/scripts"
    )
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(
        TARGET myschedule POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/assets"
                "$<TARGET_FILE_DIR:myschedule>/assets"
    )
endif()

# Optional Qt GUI target.
# Build GUI only when Qt is found AND GUI source files exist.
set(GUI_SOURCES
    gui/main_gui.cpp
    gui/MainWindow.cpp
    gui/MainWindow.h
)

set(GUI_FILES_EXIST TRUE)

foreach(GUI_FILE ${GUI_SOURCES})
    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${GUI_FILE}")
        set(GUI_FILES_EXIST FALSE)
    endif()
endforeach()

if (GUI_FILES_EXIST)
    find_package(Qt6 COMPONENTS Widgets QUIET)
    find_package(Qt5 COMPONENTS Widgets QUIET)

    if (Qt6_FOUND)
        set(CMAKE_AUTOMOC ON)

        add_executable(myschedule_gui
            ${GUI_SOURCES}
            ${COMMON_SOURCES}
        )

        target_include_directories(myschedule_gui PRIVATE
            ${PROJECT_INCLUDE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/gui
        )

        target_link_libraries(myschedule_gui PRIVATE
            Qt6::Widgets
            Threads::Threads
        )

        if (WIN32)
            target_link_libraries(myschedule_gui PRIVATE winmm)
        endif()

        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/scripts")
            add_custom_command(
                TARGET myschedule_gui POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${CMAKE_CURRENT_SOURCE_DIR}/scripts"
                        "$<TARGET_FILE_DIR:myschedule_gui>/scripts"
            )
        endif()

        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
            add_custom_command(
                TARGET myschedule_gui POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
                        "$<TARGET_FILE_DIR:myschedule_gui>/assets"
            )
        endif()

        message(STATUS "Qt6 found: building myschedule_gui")

    elseif (Qt5_FOUND)
        set(CMAKE_AUTOMOC ON)

        add_executable(myschedule_gui
            ${GUI_SOURCES}
            ${COMMON_SOURCES}
        )

        target_include_directories(myschedule_gui PRIVATE
            ${PROJECT_INCLUDE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/gui
        )

        target_link_libraries(myschedule_gui PRIVATE
            Qt5::Widgets
            Threads::Threads
        )

        if (WIN32)
            target_link_libraries(myschedule_gui PRIVATE winmm)
        endif()

        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/scripts")
            add_custom_command(
                TARGET myschedule_gui POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${CMAKE_CURRENT_SOURCE_DIR}/scripts"
                        "$<TARGET_FILE_DIR:myschedule_gui>/scripts"
            )
        endif()

        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
            add_custom_command(
                TARGET myschedule_gui POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
                        "$<TARGET_FILE_DIR:myschedule_gui>/assets"
            )
        endif()

        message(STATUS "Qt5 found: building myschedule_gui")

    else()
        message(STATUS "Qt not found: only building command-line myschedule")
    endif()

else()
    message(STATUS "GUI source files not found: only building command-line myschedule")
endif()
