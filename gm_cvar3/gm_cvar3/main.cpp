#undef _UNICODE

#pragma comment (linker, "/NODEFAULTLIB:libcmt")

#ifdef _LINUX
	#define VSTD_LIB "vstdlib.so"
	#define ENGINE_LIB "engine.so"
#else
	#define VSTD_LIB "vstdlib.dll"
	#define ENGINE_LIB "engine.dll"
#endif

#define SERVER

#include "sigscan.h"

//Lua module interface
#include "ILuaModuleManager.h"

// CVar
#include "tier1/iconvar.h"
#include "tier1/tier1.h"
#include "vstdlib/cvar.h"
#include "iconvar.h"
#include "convar.h"

#include "eiface.h"
#include "netadr.h"
#include "inetmsghandler.h"

#include "tmpserver.h"
#include "tmpclient.h"

#include "igameevents.h"
#include "inetchannel.h"

GMOD_MODULE( Open, Close );

ICvar *g_ICvar = NULL;
CBaseServer *pServer = NULL;

int GetPlayerMethodInt( lua_State* L, int i, char *methodName )
{
	ILuaObject *ply = Lua()->GetObject( i );
	ILuaObject *plyMeta = Lua()->GetMetaTable( "Player", Type::ENTITY );
    ILuaObject *method = plyMeta->GetMember( methodName );
     
    Lua()->Push( method );
    Lua()->Push( ply );
    Lua()->Call( 1, 1 );
     
    ILuaObject *retrn = Lua()->GetReturn( 0 );

    int retrnNum = retrn->GetInt();
     
    ply->UnReference();
    plyMeta->UnReference();
    method->UnReference();
    retrn->UnReference();

	return retrnNum;
}

inline ILuaObject* NewConVarObject( lua_State* L, const char *name )
{
	ILuaInterface* gLua = Lua();

	ILuaObject* func = gLua->GetGlobal( "GetConVar" );

		gLua->Push( func );
			gLua->Push( name );
		gLua->Call( 1, 1 );

	func->UnReference();

	return gLua->GetReturn( 0 );
}

LUA_FUNCTION( GetAllCvars )
{
	ILuaInterface* gLua = Lua();

	ConCommandBase *conCmds = g_ICvar->GetCommands();

	ILuaObject* conVarTable = gLua->GetNewTable();

	int i = 1;
	while( conCmds )
	{
		if ( !conCmds->IsCommand() ) {

			/*
			ConVar *cvar = ( ConVar* ) g_ICvar->FindVar( conCmds->GetName() );

			ILuaObject *metaT = gLua->GetMetaTable( "ConVar", Type::CONVAR );
				gLua->PushUserData( metaT, cvar );
			metaT->UnReference();
			*/

			/*ILuaObject* luaCvar = NewConVarObject( L, conCmds->GetName() );
				conVarTable->SetMember( i, luaCvar ); // attempt to index a userdata value?
			luaCvar->UnReference();*/

			conVarTable->SetMember( i, conCmds->GetName() );
			i++;
		}
		conCmds = conCmds->GetNext();
	}

	gLua->Push( conVarTable );

	conVarTable->UnReference();

	return 1;
}

LUA_FUNCTION( GetAllCmds )
{
	ILuaInterface* gLua = Lua();
	ConCommandBase *conCmds = g_ICvar->GetCommands();

	ILuaObject* conVarTable = gLua->GetNewTable();

	int i = 1;
	while( conCmds )
	{
		if ( conCmds->IsCommand() ) {
			conVarTable->SetMember( i, conCmds->GetName() );
			i++;
		}
		conCmds = conCmds->GetNext();
	}

	gLua->Push( conVarTable );

	conVarTable->UnReference();

	return 1;
}

LUA_FUNCTION( SetConVarValue )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, Type::CONVAR );

	int argType = gLua->GetType(2);

	ConVar *cvar = (ConVar*) gLua->GetUserData( 1 );

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	switch( argType ) // From haza's cvar2..
	{
		case Type::NUMBER:
			cvar->SetValue( (float) gLua->GetDouble( 2 ) );
			break;
		case Type::BOOL:
			cvar->SetValue( gLua->GetBool( 2 ) ? 1 : 0 );
			break;
		case Type::STRING:
			cvar->SetValue( gLua->GetString( 2 ) );
			break;
		default:
			gLua->Error("Argument #2 invalid!\n");
			break;
	}

	return 0;
}

LUA_FUNCTION( GetConVarBool )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, Type::CONVAR );

	ConVar *cvar = (ConVar*) gLua->GetUserData( 1 );

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	gLua->Push( (bool) cvar->GetBool() );

	return 1;
}

LUA_FUNCTION( GetConVarDefault )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, Type::CONVAR );

	ConVar *cvar = (ConVar*) gLua->GetUserData( 1 );

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	gLua->Push( cvar->GetDefault() );

	return 1;
}

LUA_FUNCTION( GetConVarFloat )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, Type::CONVAR );

	ConVar *cvar = (ConVar*) gLua->GetUserData( 1 );

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	gLua->Push( (float) cvar->GetFloat() );

	return 1;
}

LUA_FUNCTION( GetConVarInt )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, Type::CONVAR );

	ConVar *cvar = (ConVar*) gLua->GetUserData( 1 );

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	gLua->Push( (float) cvar->GetInt() );

	return 1;
}

LUA_FUNCTION( GetConVarName )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, Type::CONVAR );

	ConVar *cvar = (ConVar*) gLua->GetUserData( 1 );

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	gLua->Push( cvar->GetName() );

	return 1;
}

LUA_FUNCTION( SetConVarName )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::CONVAR);
	gLua->CheckType(2, Type::STRING);

	ConVar *cvar = (ConVar*) gLua->GetUserData(1);

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	cvar->m_pszName = new char[50];
	strcpy( (char*)cvar->m_pszName, gLua->GetString(2) );

	return 0;
}

LUA_FUNCTION( GetConVarString )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, Type::CONVAR );

	ConVar *cvar = (ConVar*) gLua->GetUserData( 1 );

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	gLua->Push( cvar->GetString() );

	return 1;
}

LUA_FUNCTION( SetConVarFlags )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::CONVAR);
	gLua->CheckType(2, Type::NUMBER);

	ConVar *cvar = (ConVar*) gLua->GetUserData(1);

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	cvar->m_nFlags = gLua->GetInteger(2);

	return 0;
}

LUA_FUNCTION( GetConVarFlags )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::CONVAR);

	ConVar *cvar = (ConVar*) gLua->GetUserData(1);

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	gLua->Push((float)cvar->m_nFlags);

	return 1;
}

LUA_FUNCTION( ConVarHasFlag )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::CONVAR);
	gLua->CheckType(2, Type::CONVAR);

	ConVar *cvar = (ConVar*) gLua->GetUserData(1);

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	gLua->Push( (bool) cvar->IsFlagSet( gLua->GetInteger( 2 ) ) );

	return 1;
}

LUA_FUNCTION( SetConVarHelpText )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::CONVAR);
	gLua->CheckType(2, Type::CONVAR);

	ConVar *cvar = (ConVar*) gLua->GetUserData(1);

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	cvar->m_pszHelpString = new char[100];
	strcpy( (char*)cvar->m_pszHelpString, gLua->GetString(2) );

	return 0;
}

LUA_FUNCTION( GetConVarHelpText )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::CONVAR);

	ConVar *cvar = (ConVar*) gLua->GetUserData(1);

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	gLua->Push( cvar->m_pszHelpString );

	return 1;
}

LUA_FUNCTION( ResetConVarValue )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::CONVAR);

	ConVar *cvar = (ConVar*) gLua->GetUserData(1);

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	cvar->Revert();

	return 0;
}

LUA_FUNCTION( GetConVarMin )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::CONVAR);

	ConVar *cvar = (ConVar*) gLua->GetUserData(1);

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	float min;
	bool hasMin = cvar->GetMin( min );

	if ( hasMin ) {
		gLua->Push( min );
		return 1;
	}

	return 0;
}

LUA_FUNCTION( GetConVarMax )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::CONVAR);

	ConVar *cvar = (ConVar*) gLua->GetUserData(1);

	if (!cvar)
		gLua->Error("Invalid ConVar handle!\n");

	float max;
	bool hasMax = cvar->GetMax( max );

	if ( hasMax ) {
		gLua->Push(max);
		return 1;
	}

	return 0;
}

LUA_FUNCTION( RemoveConVar )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, Type::CONVAR );

	ConVar *cvar = (ConVar*) gLua->GetUserData( 1 );

	if (!cvar)
		gLua->Error("Invalid ConVar!\n");

	g_ICvar->UnregisterConCommand( cvar );
	delete cvar;
	return 0;
}

LUA_FUNCTION( ExecuteCommandOnClient )
{
	ILuaInterface* gLua = Lua();

	if ( !pServer )
		gLua->Error("CVar3: pServer is NULL!");
	
	gLua->CheckType(1, Type::ENTITY);
	gLua->CheckType(2, Type::STRING);
	
	int clientID = GetPlayerMethodInt(L,1,"UserID");
	
	for (int i=0; i < pServer->GetClientCount(); i++) {

		IClient* client = pServer->GetClient(i);

		if( client->GetUserID() == clientID ) {
			client->ExecuteStringCommand( gLua->GetString(2) );
			break;
		}
	}

	return 0;
}

LUA_FUNCTION( GetUserSetting )
{
	ILuaInterface* gLua = Lua();

	if ( !pServer )
		gLua->Error("CVar3: pServer is NULL!");

	gLua->CheckType(1, Type::ENTITY);
	gLua->CheckType(2, Type::STRING);

	int clientID = GetPlayerMethodInt(L,1,"UserID");
	
	for (int i=0; i < pServer->GetClientCount(); i++) {
		IClient* client = pServer->GetClient(i);

		if( client->GetUserID() == clientID ) {
			gLua->Push( client->GetUserSetting( gLua->GetString(2) ) );
			return 1;
		}
	}

	return 0;
}

LUA_FUNCTION( ReplicateData )
{
	ILuaInterface* gLua = Lua();

	if ( !pServer )
		gLua->Error("CVar3: pServer is NULL!");

	gLua->CheckType(1, Type::ENTITY);
	gLua->CheckType(2, Type::STRING);
	gLua->CheckType(3, Type::STRING);

	int clientID = GetPlayerMethodInt(L,1,"UserID");
	
	for (int i=0; i < pServer->GetClientCount(); i++) {

		IClient* client = pServer->GetClient(i);

		if( client->GetUserID() == clientID ) {

			INetChannel *netChan = client->GetNetChannel();

			char pckBuf[256];
			bf_write pck( pckBuf, sizeof( pckBuf ) );
			
			pck.WriteUBitLong( 5, 5 );
			pck.WriteByte( 0x01 );
			pck.WriteString( gLua->GetString(2) );
			pck.WriteString( gLua->GetString(3) );

			netChan->SendData (pck );
			break;
		}		
	}

	return 0;
}

LUA_FUNCTION( __eq )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, Type::CONVAR );
	gLua->CheckType( 2, Type::CONVAR );

	ConVar *cvar1 = ( ConVar* ) gLua->GetUserData(1);
	ConVar *cvar2 = ( ConVar* ) gLua->GetUserData(2);
	
	if ( cvar1 == cvar2 )
		gLua->Push( (bool) true );
	else
		gLua->Push( (bool) false );

	return 1;
}

LUA_FUNCTION( __tostring )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType( 1, Type::CONVAR );

	ConVar *cvar = ( ConVar* ) gLua->GetUserData(1);

	if ( !cvar )
		gLua->Error("Invalid ConVar!\n");
	
	gLua->PushVA( "ConVar: %p", cvar );

	return 1;
}

LUA_FUNCTION( GetConVar )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::STRING);

	ConVar *ConVar = g_ICvar->FindVar( gLua->GetString( 1 ) );

	ILuaObject *metaT = gLua->GetMetaTable( "ConVar", Type::CONVAR );
		gLua->PushUserData( metaT, cvar );
	metaT->UnReference();

	return 1;
}

LUA_FUNCTION( GetCommand )
{
	ILuaInterface* gLua = Lua();
	gLua->CheckType(1, Type::STRING);

	ConCommand *cvar = g_ICvar->FindCommand( gLua->GetString( 1 ) );

	ILuaObject *metaT = gLua->GetMetaTable( "ConVar", Type::CONVAR );
		gLua->PushUserData( metaT, cvar );
	metaT->UnReference();

	return 1;
}

int Open( lua_State *L ) {

	ILuaInterface* gLua = Lua();
	
	CreateInterfaceFn VSTDLibFactory = Sys_GetFactory( VSTD_LIB );
	if ( !VSTDLibFactory )
		gLua->Error( "gm_cvar3: Error getting " VSTD_LIB " factory.\n" );

	g_ICvar = ( ICvar* )VSTDLibFactory( CVAR_INTERFACE_VERSION, NULL );
	if ( !g_ICvar )
		gLua->Error( "gm_cvar3: Error getting ICvar interface.\n" );

	//if ( gLua->IsServer() ) {
#ifdef SERVER
		// rip from gatekeeper
		CSigScan::sigscan_dllfunc = Sys_GetFactory( ENGINE_LIB );
	
		if ( !CSigScan::GetDllMemInfo() )
			gLua->Error("gm_cvar3: CSigScan::GetDllMemInfo failed!");

		CSigScan sigBaseServer;
		
		sigBaseServer.Init( (unsigned char *)
		"\x00\x00\x00\x00" "\x88\x1D\x00\x00\x00\x00" "\xE8\x00\x00\x00\x00" "\xD9\x1D",
		"????xx????x????xx",
		17 );

		if ( !sigBaseServer.is_set )
			gLua->Error("gm_cvar3: CBaseServer signature failed!");
		else {
			pServer = *(CBaseServer**) sigBaseServer.sig_addr;

			ILuaObject* playerMeta = gLua->GetMetaTable( "Player", Type::ENTITY );
			if( playerMeta ) {
				playerMeta->SetMember( "ExecuteCommandOnClient", ExecuteCommandOnClient );
				playerMeta->SetMember( "GetUserSetting", GetUserSetting );
				playerMeta->SetMember( "ReplicateData", ReplicateData );
			}
			playerMeta->UnReference();
		}
	//}
#endif
	
	gLua->SetGlobal( "FCVAR_DEVELOPMENTONLY", (float) FCVAR_DEVELOPMENTONLY );
	gLua->SetGlobal( "FCVAR_HIDDEN", (float) FCVAR_HIDDEN );
	
	ILuaObject* cvars = gLua->GetNewTable();
		cvars->SetMember( "GetAllConVars", GetAllCvars );
		cvars->SetMember( "GetAllCommands", GetAllCmds );
		cvars->SetMember( "GetCommand", GetCommand );
	gLua->SetGlobal( "cvars", cvars );

	cvars->UnReference();
	
	ILuaObject* conVarMeta = gLua->GetMetaTable( "ConVar", Type::CONVAR );
	if( conVarMeta ) {
		conVarMeta->SetMember( "SetValue", SetConVarValue );
		conVarMeta->SetMember( "GetBool", GetConVarBool );
		conVarMeta->SetMember( "GetDefault", GetConVarDefault );
		conVarMeta->SetMember( "GetFloat", GetConVarFloat );
		conVarMeta->SetMember( "GetInt", GetConVarInt );
		conVarMeta->SetMember( "GetName", GetConVarName );
		conVarMeta->SetMember( "SetName", SetConVarName );
		conVarMeta->SetMember( "GetString", GetConVarString );
		conVarMeta->SetMember( "SetFlags", SetConVarFlags );
		conVarMeta->SetMember( "GetFlags", GetConVarFlags );
		conVarMeta->SetMember( "HasFlag", ConVarHasFlag );
		conVarMeta->SetMember( "SetHelpText", SetConVarHelpText );
		conVarMeta->SetMember( "GetHelpText", GetConVarHelpText );
		conVarMeta->SetMember( "ResetValue", ResetConVarValue );
		conVarMeta->SetMember( "Remove", RemoveConVar );
		conVarMeta->SetMember( "__tostring", __tostring );
		conVarMeta->SetMember( "__eq", __eq );
		conVarMeta->SetMember( "__index", conVarMeta );
	}
	conVarMeta->UnReference();
	return 0;
}

int Close(lua_State *L)
{
	return 0;
}