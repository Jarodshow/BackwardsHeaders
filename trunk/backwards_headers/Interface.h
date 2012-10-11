#ifndef GARRYSMOD_LUA_INTERFACE_H
#define GARRYSMOD_LUA_INTERFACE_H

#include "Types.h"
#include "LuaBase.h"

	struct lua_State
	{
		unsigned char				_ignore_this_common_lua_header_[6];
		GarrysMod::Lua::ILuaBase*	luabase;
	};

	#ifdef _WIN32
		#define DLL_EXPORT extern "C" __declspec( dllexport )
	#else
		#define DLL_EXPORT extern "C" __attribute__( ( visibility("default") ) )	
	#endif

	#define GMOD_MODULE_OPEN()	DLL_EXPORT int gmod13_open( lua_State* L )
	#define GMOD_MODULE_CLOSE()	DLL_EXPORT int gmod13_close( lua_State* L )

	#define LUA L->luabase

#endif 

