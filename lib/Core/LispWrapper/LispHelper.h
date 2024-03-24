/*
 * This is a part of the Uniot project.
 * Copyright (C) 2016-2020 Uniot <contact@uniot.io>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <libminilisp.h>

#define inlinePrimitive(name, expiditor, body)            \
  #name, [](Root root, VarObject env, VarObject list) {   \
    PrimitiveExpeditor expiditor(#name, root, env, list); \
    {                                                     \
      body                                                \
    }                                                     \
    return expiditor.makeBool(false);                     \
  }

#define getPrimitiveName() (__func__)

#define exportPrimitiveNameTo(name) \
  char name[sizeof(__func__)];      \
  snprintf(name, sizeof(name), __func__)

namespace uniot {

namespace lisp {
using Object = struct Obj *;
using VarObject = struct Obj **;
using Root = void *;
}  // namespace lisp

class Lisp {
 public:
  enum Type : uint8_t {
    Unknown = 0,
    Int,
    Bool,
    BoolInt,
    Symbol,
    Cell,

    Any
  };

  static inline bool correct(Lisp::Type type) {
    return (type >= Type::Int && type <= Lisp::Type::Any);
  }

  static inline const char *str(Lisp::Type type) {
    static const char *map[] = {
        "Unknown",
        "Int",
        "Bool",
        "Bool/Int",
        "Symbol",
        "Cell",

        "Any"};
    return correct(type) ? map[type] : map[Lisp::Type::Unknown];
  }

  static inline Lisp::Type getType(lisp::Object obj) {
    if (obj == nullptr) {
        return Lisp::Unknown;
    }

    switch (obj->type) {
        case TINT:
            return Lisp::Int;
        case TNIL:
            return Lisp::Bool;
        case TTRUE:
            return Lisp::Bool;
        case TSYMBOL:
            return Lisp::Symbol;
        case TCELL:
            return Lisp::Cell;
        default:
            return Lisp::Unknown;
    }
}

};

}  // namespace uniot