// NOT STARTED: Require some major refactoring to deal with local and global hash tables.

/* SPIM S20 MIPS simulator.
   Code to maintain symbol table to resolve symbolic labels.

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

#include "sym-tbl.h"

#include <sstream>

#include "data.h"
#include "inst.h"
#include "mem.h"
#include "parser.h"
#include "parser_yacc.h"
#include "reg.h"
#include "spim-utils.h"
#include "spim.h"
#include "string-stream.h"

void SymbolTable::initialize_symbol_table(bool free_labels) {
    this->complete_table.clear();
    this->local_labels.clear();
}

/* Lookup label with NAME.  Either return its symbol table entry or NULL
   if it is not in the table. */

label *SymbolTable::label_is_defined(const std::string &name) {
    auto it = this->complete_table.find(name);
    return it == this->complete_table.end() ? nullptr : &it->second;
}

/* Return a label with a given NAME.  If an label with that name has
   previously been looked-up, the same node is returned this time.  */

label *SymbolTable::lookup_label(const std::string &name) {
    auto &&result = this->complete_table.find(name);

    if (result != this->complete_table.end()) {
        return &(result->second);
    }

    /* Not found, create one, create one */
    auto &&label = this->complete_table[name];
    label.name = name;
    return &label;
}

/* Make the label named NAME global.  Return its symbol. */

label *SymbolTable::make_label_global(const std::string &name) {
    label *l = lookup_label(name);
    l->global_flag = true;
    return l;
}

/* Record that an INSTRUCTION uses the as-yet undefined SYMBOL. */

void SymbolTable::record_inst_uses_symbol(instruction *inst, label *sym) {
    label_use u;

    if (data_dir) { /* Want to free up original instruction */
        u.inst = copy_inst(inst);
        u.addr = current_data_pc();
    } else {
        u.inst = inst;
        u.addr = current_text_pc();
    }
    sym->uses.push_front(u);
}

/* Record that a memory LOCATION uses the as-yet undefined SYMBOL. */

void SymbolTable::record_data_uses_symbol(mem_addr location, label *sym) {
    label_use u;
    u.inst = nullptr;
    u.addr = location;
    sym->uses.push_front(u);
}

/* Remove all local (non-global) label from the table. */

void SymbolTable::flush_local_labels(bool issue_undef_warnings) {
    label *l;

    while (!this->local_labels.empty()) {
        label &l = this->local_labels.front();
        this->local_labels.pop_front();

        if (issue_undef_warnings && l.addr == 0 && !l.const_flag) {
            error("Warning: local symbol %s was not defined\n", l.name);
        }

        // Delete label (owned by hashmap)
        this->complete_table.erase(l.name);
    }

    this->local_labels.clear();
}

/* Return the address of SYMBOL or 0 if it is undefined. */

mem_addr SymbolTable::find_symbol_address(const std::string &symbol) {
    label *l = lookup_label(symbol);

    if (l == nullptr || l->addr == 0) {
        return 0;
    } else {
        return l->addr;
    }
}

/* Print all symbols in the table. */

void SymbolTable::print_symbols(port message_out) const {
    for (const auto &it : this->complete_table) {
        const label &l = it.second;

        write_output(message_out, "%s%s at 0x%08x\n", l.global_flag ? "g\t" : "\t", l.name, l.addr);
    }
}

/* Print all undefined symbols in the table. */

void SymbolTable::print_undefined_symbols(port message_out) const {
    for (const auto &it : this->complete_table) {
        const label &l = it.second;

        if (l.addr == 0) {
            write_output(message_out, "%s\n", l.name);
        }
    }
}

/* Return a string containing the names of all undefined symbols in the
   table, seperated by a newline character.  Return NULL if no symbols
   are undefined. */

std::string SymbolTable::undefined_symbol_string() {
    std::stringstream stream;

    for (const auto &it : this->complete_table) {
        const label &l = it.second;
        if (l.addr == 0) {
            stream << l.name << '\n';
        }
    }
    return stream.str();
}
