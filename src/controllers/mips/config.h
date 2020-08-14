#pragma once
#ifndef MIPS_CONFIG_H
#define MIPS_CONFIG_H

#include <stdint.h>

#include "spim.h"

struct MemConfig {
    // Starting Config details
    int32_t text_size, data_size, stack_size, k_text_size, k_data_size;

    int32_t data_limit, stack_limit, k_data_limit;
    // Hard limits
};

struct CPUConfig {
    MemConfig memory;

    // IO Config
    port message_out;
    port console_out;
    port console_in;

    bool mapped_io; /* => activate memory-mapped IO */

    bool bare_machine;                   /* => simulate bare machine */
    bool accept_pseudo_insts;            /* => parse pseudo instructions  */
    bool delayed_branches;               /* => simulate delayed branches */
    bool delayed_loads;                  /* => simulate delayed loads */
    bool quiet;                          /* => no warning messages */
    char* exception_file_name = nullptr; /* The path from which to load the exception handler, if desired */
    int spim_return_value;               /* Value returned when spim exits */
};

#endif