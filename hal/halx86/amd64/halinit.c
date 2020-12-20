/*
 * PROJECT:         ReactOS HAL
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            hal/halx86/amd64/halinit.c
 * PURPOSE:         HAL Entrypoint and Initialization
 * PROGRAMMERS:
 */

/* INCLUDES ******************************************************************/

#include <hal.h>
#define NDEBUG
#include <debug.h>

/* GLOBALS *******************************************************************/

BOOLEAN HalpPciLockSettings;

/* PRIVATE FUNCTIONS *********************************************************/

CODE_SEG("INIT")
VOID
NTAPI
HalpGetParameters(IN PLOADER_PARAMETER_BLOCK LoaderBlock)
{
    PCHAR CommandLine;

    /* Make sure we have a loader block and command line */
    if ((LoaderBlock) && (LoaderBlock->LoadOptions))
    {
        /* Read the command line */
        CommandLine = LoaderBlock->LoadOptions;

        /* Check if PCI is locked */
        if (strstr(CommandLine, "PCILOCK")) HalpPciLockSettings = TRUE;

        /* Check for initial breakpoint */
        if (strstr(CommandLine, "BREAK")) DbgBreakPoint();
    }
}

/* FUNCTIONS *****************************************************************/
#if 0
VOID
NTAPI
HalpInitProcessor(
    IN ULONG ProcessorNumber,
    IN PLOADER_PARAMETER_BLOCK LoaderBlock)
{
    DPRINT1("ApicInitializeProcessor(%ld)\n", ProcessorNumber);

    /* Initialize the local APIC for this cpu */
    ApicInitializeLocalApic(ProcessorNumber);

    /* Initialize the timer */
    //ApicInitializeTimer(ProcessorNumber);

}


VOID
HalpInitPhase0(IN PLOADER_PARAMETER_BLOCK LoaderBlock)
{

}

VOID
HalpInitPhase1(VOID)
{

}

#endif

