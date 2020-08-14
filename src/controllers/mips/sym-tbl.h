// NOT STARTED: Require some major refactoring to deal with local and global hash tables.

/* SPIM S20 MIPS simulator.
   Data structures for symbolic addresses.

   Copyright (c) 1990-2010, James R. Larus.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

   Neither the name of the James R. Larus nor the names of its contributors may be
   used to endorse or promote products derived from this software without specific
   prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <forward_list>
#include <functional>
#include <unordered_map>
#include <vector>

#include "inst.h"

struct label_use {
    instruction *inst; /* NULL => Data, not code */
    mem_addr addr;

    inline label_use() {
        inst = nullptr;
        addr = 0;
    }
};

/* Symbol table information on a label. */

struct label {
    // char *name;
    std::string name;     /* Name of label */
    mem_addr addr;        /* Address of label or 0 if not yet defined */
    bool global_flag : 1; /* Non-zero => declared global */
    bool gp_flag : 1;     /* Non-zero => referenced off gp */
    bool const_flag : 1;  /* Non-zero => constant value (in addr) */
    // label *next_local;                 /* Link in list of local labels */
    std::forward_list<label_use> uses; /* List of instructions that reference */

    inline label() {
        name = "";
        addr = 0;
        global_flag = false;
        gp_flag = false;
        const_flag = false;
    }
}; /* label that has not yet been defined */

/**
 * WARNING:
 * The following code relies extremely heavily on the fact that:
 *
 * > If rehashing occurs due to the insertion, all iterators are invalidated.
 * > Otherwise iterators are not affected. References are not invalidated.
 * >
 * > Source C++ Standard
 * > URL: http://eel.is/c++draft/unord.req#9
 *
 * We use this to keep track of references in the local table and maintain forward pointers
 * for the linked list. There is probably going to be a significant effort in updating this
 * code to modern C++.
 */
class SymbolTable {
    /**
     * Map from name of a label to a label structure.
     *
     * Try to see if we can convert this to a smart pointer with actual ownership
     */
    std::unordered_map<std::string, label> complete_table;

    /**
     * Keep track of the memory location that a label represents.  If we
     * see a reference to a label that is not yet defined, then record the
     * reference so that we can patch up the instruction when the label is
     * defined.
     *
     * At the end of a file, we flush the hash table of all non-global
     * labels so they can't be seen in other files.
     */

    // label *local_labels;  // Try to make this a C++ linked list, but this might require a major
    //                       // refactoring

    std::forward_list<std::reference_wrapper<label>> local_labels;

   public:
    inline SymbolTable() { complete_table.reserve(8191); }

    /**
     * Return the address of SYMBOL or 0 if it is undefined.
     */
    mem_addr find_symbol_address(const std::string &symbol);

    /**
     * Remove all local (non-global) label from the table.
     */
    void flush_local_labels(bool issue_undef_warnings);

    /**
     * Initialize the symbol table by removing and freeing old entries.
     */
    void initialize_symbol_table(bool free_labels);

    /**
     * Lookup label with NAME.  Either return its symbol table entry or NULL
     * if it is not in the table.
     */
    label *label_is_defined(const std::string &name);

    /**
     * Return a label with a given NAME.  If an label with that name has
     * previously been looked-up, the same node is returned this time.
     */
    label *lookup_label(const std::string &name);

    /**
     * Make the label named NAME global.
     * Return its symbol.
     */
    label *make_label_global(const std::string &name);

    /**
     * Print all symbols in the table.
     */
    void print_symbols(port message_out) const;

    /**
     * Print all undefined symbols in the table.
     */
    void print_undefined_symbols(port message_out) const;

    /**
     * Record that a memory LOCATION uses the as-yet undefined SYMBOL.
     */
    void record_data_uses_symbol(mem_addr location, label *sym);

    /**
     * Record that an INSTRUCTION uses the as-yet undefined SYMBOL.
     */
    void record_inst_uses_symbol(instruction *inst, label *sym);

    /**
     * Return a string containing the names of all undefined symbols in the
     * table, seperated by a newline character.  Return NULL if no symbols
     * are undefined.
     */
    std::string undefined_symbol_string();

    friend class CPU;
};

inline bool SYMBOL_IS_DEFINED(label *SYM) { return SYM->addr != 0; }

/* Exported functions: */
