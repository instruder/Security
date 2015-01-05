usage:

use windbg attach flashplayer or browser process

0:005> !load DbgJitExt

0:005> !hookjit  0x00310000  //flashplayer base address or flash dll base addr


0:013> !SetBpJit 0x1a		//method id
add method_id:26 breakpoint success

