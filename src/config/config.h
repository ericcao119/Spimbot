/**
 * Configuration with information relevant to the Spimbot version of the code.
 * Hopefully, prefer using this as opposed to globals, which are currently is major use
 * throughout the program. These config are meant to direct the initialization and setup of code,
 * but not the overall layout like the QtApplication.
 * 
 * Note that this is hopefully that the start of a major refactoring of the entire project.
 * 
 * TODO: Deprecate spimbot_testing
 * TODO: Create Wrapper class around MIPS Emulation to further abstract away the code.
 */

#pragma once

#ifndef SPIMBOT_CONFIG_H_
#define SPIMBOT_CONFIG_H_

#include <QString>

enum StartingStateType {
    Lab9Part1,
    Lab9Part2,
    LabSpimbotPart1,
    OfficialSpimbot,
    TestSpimbot
};

struct GuiConfig {
    double drawing_scaling = 2.0;
    bool map_enabled = true;  // nomap != false
    int repaint_cycles = 8192;
};

struct IOConfig {
    bool print_scores = true;
    bool debug_prints_enabled = false;
};

/**
 * Dump config for things that should be logged periodically. It is recommended to
 * back these with buffers in order to prevent to process from being IO bound.
 */
struct PerioicDumpConfig {
    QString component_log_path = "";
    int log_period = 8192;  // Should be stored in a buffer before logging
};

/**
 * The structure of the dump is mostly up to you, but try to keep it as minimal as possible
 * or in well known formats like JSON. These are event driven and the cycle that things were initiated on,
 * any relevant arguments, and if the action was successful should be logged out.
 */
struct EventDumpConfig {
    QString component_log_path = "";
};

/**
 * Struct for containing info dumps by the application. This is mostly used for
 * application and display purposes by SpimArena.
 */
struct DumpConfig {
    QString screenshot_dir = "";

    // Relevant dumps of information for things like game object locations
    //
    PerioicDumpConfig location = {"", 100};

    // Create an event dump for every bot initiated interrupt or MMIO
    EventDumpConfig puzzle_solve = {""};
};

// Quick initialization config to allow for tests to be done quickly
struct StartingState {
    // bot 1 scenario state
    // bot 2 scenario state

    // Constant information like map layout
};

struct SpimbotConfig {
    // Top level config items that permiate everything
    bool debug = false;    // from spimbot_debug
    bool grading = false;  // from spimbot_grading
    bool exit_on_error = false;
    StartingState starting_state;

    int total_cycle_count = 10'000'000;  // most languages have different literals to help with long ints. For some reason C++ uses '

    GuiConfig gui;
    IOConfig io;
    DumpConfig log_dump;
};

// Initialize some constant configurations down here for testing purposes.

class SpimbotConfigBuilder {
    // Should we use this
};

#endif