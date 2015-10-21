/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef ROCKETCORELUAELEMENTINSTANCER_H
#define ROCKETCORELUAELEMENTINSTANCER_H

#include "../../Include/Rocket/Core/Lua/lua.hpp"
#include "../../Include/Rocket/Core/Lua/LuaType.h"
#include "LuaElementInstancer.h"

namespace Rocket {
namespace Core {
namespace Lua {
template<> void ExtraInit<ElementInstancer>(lua_State* L, int metatable_index);
//method
int ElementInstancernew(lua_State* L);
//setter
int ElementInstancerSetAttrInstanceElement(lua_State* L);

extern RegType<ElementInstancer> ElementInstancerMethods[];
extern luaL_Reg ElementInstancerGetters[];
extern luaL_Reg ElementInstancerSetters[];

LUACORETYPEDECLARE(ElementInstancer)
}
}
}


#endif
