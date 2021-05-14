#include "AEConfig.h"

#ifndef AE_OS_WIN
	#include <AE_General.r>
#endif
resource 'PiPL' (16000) {
	{	/* array properties: 7 elements */
		/* [1] */
		Kind {
			AEGP
		},
		/* [2] */
		Name {
			"renderBeamer for AE CC v.19"
		},
		/* [3] */
		Category {
			"General Plugin"
		},
		/* [4] */
		Version {
			196608
		},
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EntryPointFunction"},
	#else
		CodeWin32X86 {"EntryPointFunction"},
	#endif
#else
	#ifdef AE_OS_MAC
		/* CodeMachOPowerPC {"EntryPointFunction"},*/
		/*CodeMacIntel32 {"EntryPointFunction"},*/
		CodeMacIntel64 {"EntryPointFunction"},
	#endif
#endif
	}
};
