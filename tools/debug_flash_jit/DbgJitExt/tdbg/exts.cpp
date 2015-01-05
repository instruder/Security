/*-----------------------------------------------------------------------------
   Copyright (c) 2000  Microsoft Corporation

Module:
  exts.cpp

  Sampe file showing couple of extension examples

-----------------------------------------------------------------------------*/
#include "stdafx.h"
#include "dbgexts.h"

ULONG g_getMethodName= 0;
ULONG g_setjitCode= 0;


/*
  This gets called (by DebugExtensionNotify whentarget is halted and is accessible
*/
HRESULT
NotifyOnTargetAccessible(PDEBUG_CONTROL Control)
{
    dprintf("Extension dll detected a break");
    if (Connected) {
        dprintf(" connected to ");
        switch (TargetMachine) {
        case IMAGE_FILE_MACHINE_I386:
            dprintf("X86");
            break;
        case IMAGE_FILE_MACHINE_IA64:
            dprintf("IA64");
            break;
        default:
            dprintf("Other");
            break;
        }
    }
    dprintf("\n");

    //
    // show the top frame and execute dv to dump the locals here and return
    //
    Control->Execute(DEBUG_OUTCTL_ALL_CLIENTS |
                     DEBUG_OUTCTL_OVERRIDE_MASK |
                     DEBUG_OUTCTL_NOT_LOGGED,
                     ".frame", // Command to be executed
                     DEBUG_EXECUTE_DEFAULT );
    Control->Execute(DEBUG_OUTCTL_ALL_CLIENTS |
                     DEBUG_OUTCTL_OVERRIDE_MASK |
                     DEBUG_OUTCTL_NOT_LOGGED,
                     "dv", // Command to be executed
                     DEBUG_EXECUTE_DEFAULT );
    return S_OK;
}


/*
  A built-in help for the extension dll
*/
HRESULT CALLBACK
help(PDEBUG_CLIENT4 Client, PCSTR args)
{
    INIT_API();

    UNREFERENCED_PARAMETER(args);

    dprintf("Help for dbgexts.dll\n"
            "  help                = Shows this help\n"
            "  hookjit <flashplayer_addr> - hook set jit function\n"
			"  setbpjit <method_count>\n"
            );
    EXIT_API();

    return S_OK;
}

/***********************************************************
 * !hookjit
 *
 * Purpose: WINDBG will call this API when the user types !hookjit
 *          
 *
 *  Parameters:
 *     !hookjit or !hookjit <address/register>
 *
 *  Return Values:
 *     N/A
 *
 ***********************************************************/
struct BREAKPOINT
{
	IDebugBreakpoint* Bp;
	ULONG Id;
};

BREAKPOINT g_SetjitCodeEndBp;
BREAKPOINT g_GetMethodNameEndBp;


HRESULT
	AddBp(BREAKPOINT* Bp, ULONG addr)
{
	HRESULT Status;
	dprintf("begin AddBp \r\n");
	if ((Status = g_ExtControl->AddBreakpoint(DEBUG_BREAKPOINT_CODE, DEBUG_ANY_ID,
		&Bp->Bp)) != S_OK)
	{
		Bp->Id = DEBUG_ANY_ID;
		return Status;
	}
	dprintf("check AddBp \r\n");
	if ((Status = Bp->Bp->GetId(&Bp->Id)) != S_OK ||
		(Status = Bp->Bp->SetOffset(addr)) != S_OK ||
		(Status = Bp->Bp->AddFlags(DEBUG_BREAKPOINT_ENABLED)) != S_OK)
	{
		Bp->Bp->Release();
		Bp->Id = DEBUG_ANY_ID;
		return Status;
	}
	dprintf("edn AddBp \r\n");
	return S_OK;
}

//----------------------------------------------------------------------------
//
// Event callbacks.
//
//----------------------------------------------------------------------------
class EventCallbacks : public DebugBaseEventCallbacks
{
public:
	// IUnknown.
	STDMETHOD_(ULONG, AddRef)(
		THIS
		);
	STDMETHOD_(ULONG, Release)(
		THIS
		);

	// IDebugEventCallbacks.
	STDMETHOD(GetInterestMask)(
		THIS_
		OUT PULONG Mask
		);

	STDMETHOD(Breakpoint)(
		THIS_
		IN PDEBUG_BREAKPOINT Bp
		);
};

STDMETHODIMP_(ULONG)
	EventCallbacks::AddRef(
	THIS
	)
{
	// This class is designed to be static so
	// there's no true refcount.
	return 1;
}

STDMETHODIMP_(ULONG)
	EventCallbacks::Release(
	THIS
	)
{
	// This class is designed to be static so
	// there's no true refcount.
	return 0;
}

STDMETHODIMP
	EventCallbacks::GetInterestMask(
	THIS_
	OUT PULONG Mask
	)
{
	*Mask =DEBUG_EVENT_BREAKPOINT ;
	return S_OK;
}

STDMETHODIMP
	EventCallbacks::Breakpoint(
	THIS_
	IN PDEBUG_BREAKPOINT Bp
	)
{
	ULONG Id;
	ULONG64 ReturnOffset;
	CONTEXT	context; 
	ULONG	eipIndex;

	DEBUG_VALUE	eipValue; 

	if (Bp->GetId(&Id) != S_OK)
	{
		return DEBUG_STATUS_BREAK;
	}

	if (Id == g_SetjitCodeEndBp.Id)
	{
		// Set a breakpoint on the return address of the call
		// so that we can patch up any returned information.
		//if (g_Control->GetReturnOffset(&ReturnOffset) != S_OK ||
		//	g_GetVersionRetBp.Bp->SetOffset(ReturnOffset) != S_OK ||
		///	g_GetVersionRetBp.Bp->AddFlags(DEBUG_BREAKPOINT_ENABLED) != S_OK)
		//{
		//	return DEBUG_STATUS_BREAK;
		//}
		g_ExtControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS |
			DEBUG_OUTCTL_OVERRIDE_MASK |
			DEBUG_OUTCTL_NOT_LOGGED,
			".printf \"method id[%d]\\t jit code address:\\t%08x\\n\",poi(esi+0x24),poi(esi+4)", // Command to be executed
			DEBUG_EXECUTE_DEFAULT );
		
		//GetThreadContext
		g_ExtAdanced->GetThreadContext(&context, sizeof(context));

		CONTEXT newcontext = context;
		newcontext.Ecx = context.Esi;

		g_ExtAdanced->SetThreadContext(&newcontext, sizeof(context)); 

		//g_ExtRegister->GetIndexByName("eip", &eipIndex); 

		//g_ExtRegister->GetValue(eipIndex, &eipValue); 

		//eipValue.Type = DEBUG_VALUE_INT32; 
		//eipValue.I32 = (ULONG) g_getMethodName; 

		//g_ExtRegister->SetValue(eipIndex,&eipValue);

		//g_ExtControl->SetExecutionStatus(DEBUG_STATUS_GO); 

		//g_ExtControl->WaitForEvent(0, INFINITE); 

		//  //GET eax return value
		//g_ExtControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS |
		//	  DEBUG_OUTCTL_OVERRIDE_MASK |
		//	  DEBUG_OUTCTL_NOT_LOGGED,
		//	  ".printf \"return eax\\t%08x\\n\",eax", // Command to be executed
		//	  DEBUG_EXECUTE_DEFAULT );

		////SetThreadContext
		//g_ExtAdanced->SetThreadContext(&context, sizeof(context)); 

		return DEBUG_STATUS_GO;
	}
	else if (Id == g_GetMethodNameEndBp.Id)
	{
		g_ExtControl->Execute(DEBUG_OUTCTL_ALL_CLIENTS |
			DEBUG_OUTCTL_OVERRIDE_MASK |
			DEBUG_OUTCTL_NOT_LOGGED,
			".echo method name ;da poi(eax+0x8)", // Command to be executed
			DEBUG_EXECUTE_DEFAULT );
		return DEBUG_STATUS_BREAK;
	}
	else
	{
		return DEBUG_STATUS_NO_CHANGE;
	}

	return DEBUG_STATUS_GO;
}


EventCallbacks g_EventCb;


void hook_set_jit(ULONG hookaddress,ULONG proc)
{
	BYTE WriteBuf[1]={0};
	BYTE Tempb[4]={0};
	ULONG bytw;

	WriteBuf[0] = 0xe8;
	Tempb[0] = 0x5e;
	Tempb[1] = 0xc2;
	Tempb[2] = 0x08;
	Tempb[3] = 0x00;

	dprintf("hook_set_jit begin \n");
	ULONG offs = (ULONG)g_getMethodName - hookaddress -5;

	WriteMemory(hookaddress,WriteBuf,sizeof(WriteBuf),&bytw);

	WriteMemory(hookaddress+1,&offs,4,&bytw);

	//WriteMemory(g_getMethodName-1,&WriteBuf[1],1,&bytw);

	WriteMemory(hookaddress+5,&Tempb,sizeof(Tempb),&bytw);

}
HRESULT CALLBACK
	hookjit(PDEBUG_CLIENT4 Client, PCSTR args)
//DECLARE_API (hookjit)
{
	static ULONG BaseAddress = 0,EndAddress=0;
	ULONG  module_address;
	WCHAR MyString[51] = {0};


	module_address = GetExpression(args);

	if(module_address != 0)
	{
		BaseAddress = module_address;
	}
	else
	{
		dprintf("must give me the flash module baseaddrss \n");
		return 0;
	}
	//search jit failed ascii
	ULONG64 searched = 0;
	char Pattern [] = {'J','I','T',' ','f','a','i','l','e','d',0x00};

	SearchMemory(BaseAddress,0xfff000,sizeof(Pattern),Pattern,&searched);

	dprintf("search jit failed address :%08x\r\n", searched);

	if(searched==0)
	{
		return 0;
	}

	ULONG push_jit = searched;

	searched = 0;

	//01bdaa04
	BYTE low1 = push_jit & 0xff;
	BYTE low2 = ((push_jit & 0xff00)>>8) & 0xff; 
	BYTE low3 = (push_jit & 0xff0000)>>16;
	BYTE low4 = (push_jit & 0xff000000)>>24;

	//searh push jit_failed
	char Pattern1 [] = {0x6a,0xff,0x68,0x00,0x00,0x00,0x00,0xe8};

	Pattern1[3] = low1;
	Pattern1[4] = low2;
	Pattern1[5] = low3;
	Pattern1[6] = low4;

	dprintf("search push jit pattern :0x6a 0xff 0x%x 0x%x 0x%x 0x%x 0xe8\r\n", Pattern1[3],Pattern1[4],Pattern1[5],Pattern1[6]);

	SearchMemory(BaseAddress,0xfff000,sizeof(Pattern1),Pattern1,&searched);

	dprintf("search address :%08x\r\n", searched);

	if (searched==0)
	{
		return 0;
	}
	//find BaseExecMgr__verifyJit function 's setjitcode function 
	ULONG funcstart = searched - 0xff/2;

	BYTE readbuf[6];
	ULONG Byter=0;
	BYTE calladdress[4];
	ULONG setjitfunc = 0;
	for (unsigned int i =funcstart;i<searched;i++)
	{
		ReadMemory(i,readbuf,sizeof(readbuf),&Byter);
		//call xx jmp xx 
		if (readbuf[0]==0xe8 && readbuf[5]==0xe9)
		{
			
			calladdress[0] = readbuf[1];
			calladdress[1] = readbuf[2];
			calladdress[2] = readbuf[3];
			calladdress[3] = readbuf[4];
			ULONG off = *(ULONG *)calladdress;

			dprintf("find setjitcode address :%08x %08x  %08x\r\n", i + off+5,i,off);
			setjitfunc=i + off+5;

			break;
		}
		
	}
	/************************************************************************/
	/* .text:00A3F070 sub_A3F070      proc near               ; CODE XREF: sub_A3F0B0+61p
	.text:00A3F070
	.text:00A3F070 arg_0           = dword ptr  4
	.text:00A3F070 arg_4           = dword ptr  8
	.text:00A3F070
	.text:00A3F070                 mov     ecx, [esp+arg_4]
	.text:00A3F074                 push    esi
	.text:00A3F075                 mov     esi, [esp+4+arg_0]
	.text:00A3F079                 mov     eax, [esi+30h]
	.text:00A3F07C                 and     eax, 0FF7FFFFFh
.....
	.text:00A3F0A3
	.text:00A3F0A3 loc_A3F0A3:                             ; CODE XREF: sub_A3F070+2Cj
	.text:00A3F0A3                 mov     [esi+8], eax
	.text:00A3F0A6                 pop     esi
	.text:00A3F0A7                 retn    8
	.text:00A3F0A7 sub_A3F070      endp
                                                                     */
	/************************************************************************/
	
	//find setjitcode end 
	BYTE readbuf1[3];
	ULONG setjitfunc_pop = 0;
	for (unsigned int i =setjitfunc;i<setjitfunc+0xff;i++)
	{
		ReadMemory(i,readbuf1,sizeof(readbuf1),&Byter);
		//pop xx  retn xx
		if (readbuf1[1]==0xc2)
		{
			dprintf("find setjitcode func end address :%08x\r\n", i);
			setjitfunc_pop=i ;

			break;
		}
	}
	//hook it
	if (setjitfunc_pop==0)
	{
		return 0;
	}
	searched = 0;
	
	//find get_method_name func
	char Pattern4 [] = {0x8b,0x41,0x10,0xa8,0x01,0x74,0x13,0x83,0xe0,0xfe,0x74,0x0c,0x8b};

	SearchMemory(BaseAddress,0xfff000,sizeof(Pattern4),Pattern4,&searched);

	dprintf("avmplus__MethodInfo__getMethodName search address :%08x\r\n", searched);

	if (searched==0)
	{
		return 0;
	}

	g_getMethodName = searched;

	//hook_set_jit(setjitfunc_pop,(ULONG)set_jit_code_proc);
	//PDEBUG_BREAKPOINT BP;
	//INIT_API();
	HRESULT hret;// = g_ExtControl->AddBreakpoint(DEBUG_BREAKPOINT_CODE,DEBUG_ANY_ID,&BP);
	dprintf("ready AddBp \r\n");
	if(AddBp(&g_SetjitCodeEndBp, setjitfunc_pop) != S_OK)
	{
		dprintf("AddBp setjit code failed \r\n");
		goto exit;
	}
	if ((hret = g_ExtClient->SetEventCallbacks(&g_EventCb)) != S_OK)
	{
		dprintf("SetEventCallbacks failed, 0x%X\n", hret);
		goto exit;
	}

	hook_set_jit(setjitfunc_pop,0);

	if (AddBp(&g_GetMethodNameEndBp,setjitfunc_pop+5)!=S_OK)
	{
		dprintf("AddBp get method name failed \r\n");
		goto exit;
	}

exit:
	return 0;
}

HRESULT CALLBACK
	setbojit(PDEBUG_CLIENT4 Client, PCSTR args)
{
	static ULONG BaseAddress = 0,EndAddress=0;
	ULONG  method_id;
	WCHAR MyString[51] = {0};


	method_id = GetExpression(args);



	return 0;
}