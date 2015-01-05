// tdbg.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "tdbg.h"


#include <windows.h>

#include <imagehlp.h>
#include <wdbgexts.h>

/***********************************************************
 * Global Variable Needed For Functions
 ***********************************************************/              
//WINDBG_EXTENSION_APIS ExtensionApis = {0};

                      
/***********************************************************
 * Global Variable Needed For Versioning
 ***********************************************************/              
EXT_API_VERSION g_ExtApiVersion = {
         5 ,
         5 ,
         EXT_API_VERSION_NUMBER ,
         0
     } ;


/***********************************************************
 * ExtensionApiVersion
 *
 * Purpose: WINDBG will call this function to get the version
 *          of the API
 *
 *  Parameters:
 *     Void
 *
 *  Return Values:
 *     Pointer to a EXT_API_VERSION structure.
 *
 ***********************************************************/              
LPEXT_API_VERSION WDBGAPI ExtensionApiVersion (void)
{
    return &g_ExtApiVersion;
}


/***********************************************************
 * WinDbgExtensionDllInit
 *
 * Purpose: WINDBG will call this function to initialize
 *          the API
 *
 *  Parameters:
 *     Pointer to the API functions, Major Version, Minor Version
 *
 *  Return Values:
 *     Nothing
 *
 ***********************************************************/              
//VOID WDBGAPI WinDbgExtensionDllInit (PWINDBG_EXTENSION_APIS lpExtensionApis, USHORT usMajorVersion, USHORT usMinorVersion)
//{
     //ExtensionApis = *lpExtensionApis;
//}

/***********************************************************
 * !help
 *
 * Purpose: WINDBG will call this API when the user types !help
 *          
 *
 *  Parameters:
 *     N/A
 *
 *  Return Values:
 *     N/A
 *
 ***********************************************************/