#ifndef _AMEBA_FAULT_HANDLE_H_
#define _AMEBA_FAULT_HANDLE_H_

#define HARDFAULT_ID    0
#define USAGEFAULT_ID   1
#define BUSFAULT_ID     2
#define MEMFAULT_ID     3
#define SECUREFAULT_ID  4

/* Redefine secure fault handler and replace old INT_SecureFault in rom code. */
/* Redefine secure fault handler and replace old INT_SecureFault in rom code. */
void HANDLER_SecureFault(void);

void HANDLER_MemFault(void);

void HANDLER_BusFault(void);

void HANDLER_UsageFault(void);

void HANDLER_HardFault(void);

void Fault_Handler(uint32_t mstack[], uint32_t pstack[], uint32_t lr_value, uint32_t fault_id);

void Fault_Hanlder_Redirect(crash_on_task crash_on_task_func);


#endif