```
Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000
Mem abort info:
  ESR = 0x96000045
  EC = 0x25: DABT (current EL), IL = 32 bits
  SET = 0, FnV = 0
  EA = 0, S1PTW = 0
  FSC = 0x05: level 1 translation fault
Data abort info:
  ISV = 0, ISS = 0x00000045
  CM = 0, WnR = 1
user pgtable: 4k pages, 39-bit VAs, pgdp=00000000420e1000
[0000000000000000] pgd=0000000000000000, p4d=0000000000000000, pud=0000000000000000
Internal error: Oops: 96000045 [#1] SMP
Modules linked in: hello(O) scull(O) faulty(O)
CPU: 0 PID: 158 Comm: sh Tainted: G           O      5.15.18 #1
Hardware name: linux,dummy-virt (DT)
pstate: 80000005 (Nzcv daif -PAN -UAO -TCO -DIT -SSBS BTYPE=--)
pc : faulty_write+0x14/0x20 [faulty]
lr : vfs_write+0xa8/0x2b0
sp : ffffffc008c83d80
x29: ffffffc008c83d80 x28: ffffff80020d2640 x27: 0000000000000000
x26: 0000000000000000 x25: 0000000000000000 x24: 0000000000000000
x23: 0000000040001000 x22: 000000000000000c x21: 00000055608a2a70
x20: 00000055608a2a70 x19: ffffff800209f700 x18: 0000000000000000
x17: 0000000000000000 x16: 0000000000000000 x15: 0000000000000000
x14: 0000000000000000 x13: 0000000000000000 x12: 0000000000000000
x11: 0000000000000000 x10: 0000000000000000 x9 : 0000000000000000
x8 : 0000000000000000 x7 : 0000000000000000 x6 : 0000000000000000
x5 : 0000000000000001 x4 : ffffffc0006f0000 x3 : ffffffc008c83df0
x2 : 000000000000000c x1 : 0000000000000000 x0 : 0000000000000000
Call trace:
 faulty_write+0x14/0x20 [faulty]
 ksys_write+0x68/0x100
 __arm64_sys_write+0x20/0x30
 invoke_syscall+0x54/0x130
 el0_svc_common.constprop.0+0x44/0xf0
 do_el0_svc+0x40/0xa0
 el0_svc+0x20/0x60
 el0t_64_sync_handler+0xe8/0xf0
 el0t_64_sync+0x1a0/0x1a4
Code: d2800001 d2800000 d503233f d50323bf (b900003f)
---[ end trace 4c0dbacc198ec62c ]---
```

Analysis

- From lecture video - Oops messages leave the system in generally unusable state. May need to reboot the system and may not be enough to unload/reload the driver. This is the reason we are asked to enter user and password when the code is run.
- Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000- Dereferencing the NULL pointer fails. This is because we are trying to de-reference a NULL pointer which is a segmentation fault.
- The ESR (Exception Syndrome Register) value is a 64-bit register is a register in ARM processor. It holds syndrome information for an exception taken to EL1.  
[Reference 1](https://patchwork.kernel.org/project/linux-arm-kernel/patch/1497446043-50944-1-git-send-email-julien.thierry@arm.com/)
[Reference 2](https://developer.arm.com/documentation/ddi0601/2022-03/AArch64-Registers/ESR-EL1--Exception-Syndrome-Register--EL1-)
[Reference 3](https://www.embien.com/blog/arm-architecture-registers-exception-model/)
- In ARM architecture, "DABT" stands for Data Abort,this is an exception that occurs when a memory access operation cannot be completed. The "EC = 0x25" value indicates that the exception was caused by a translation fault. The "FSC" value indicates the value is 0x05, which means that a level 1 translation fault occurred. The "ISV" value indicates whether the abort was caused by an instruction or a data access. A value of 0 indicates that the abort was caused by a data access. The "CM" value indicates whether the abort was caused by a cache maintenance operation. A value of 0 indicates that the abort was not caused by a cache maintenance operation. The "WnR" value indicates whether the abort was caused by a write or a read operation. A value of 1 indicates that the abort was caused by a write operation.
- The oops message further shows the register state at the point of the fault, PC points to the function faulty_write of the faulty module. The function offset is 0x14 and the function size is 0x20. We can also see the call trace of the system at the point of the fault. The last line of the call trace provides a code dump of the instructions that were executing when the exception occurred. These instructions are represented in hexadecimal format.
- The dump also shows the trace of calls. This is similar to the bt command in GDB