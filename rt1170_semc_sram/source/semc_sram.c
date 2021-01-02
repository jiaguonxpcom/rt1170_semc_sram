/*
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"
#include "pin_mux.h"
#include "fsl_semc.h"

#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_SEMC_CLK_FREQ CLOCK_GetRootClockFreq(kCLOCK_Root_Semc)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/


/*******************************************************************************
 * Code
 ******************************************************************************/

#define SRAM_BASE 0x90000000
status_t BOARD_InitSRAM(void)
{
    semc_config_t config;
    semc_sram_config_t sram_config;
    uint32_t clockFrq = EXAMPLE_SEMC_CLK_FREQ;

    /* Initializes the MAC configure structure to zero. */
    memset(&config, 0, sizeof(semc_config_t));
    memset(&sram_config, 0, sizeof(semc_sram_config_t));

    /* Initialize SEMC. */
    SEMC_GetDefaultConfig(&config);
    SEMC_Init(SEMC, &config);

    // config SRAM
    sram_config.cePinMux          = kSEMC_MUXCSX2;
    sram_config.address           = SRAM_BASE;
    sram_config.memsize_kbytes    = 1024*1;
    sram_config.addrPortWidth     = 20;
    sram_config.advActivePolarity = kSEMC_AdvActiveLow;
    sram_config.addrMode          = kSEMC_AddrDataNonMux;
    sram_config.burstLen          = kSEMC_Nor_BurstLen1;
    sram_config.portSize          = kSEMC_PortSize8Bit;
    sram_config.syncMode          = kSEMC_AsyncMode;
    sram_config.waitEnable        = false;
    sram_config.waitSample        = 0;
    sram_config.advLevelCtrl      = kSEMC_AdvHigh;
    
    sram_config.tCeSetup_Ns        = 20; // CS setup and hold 
    sram_config.tCeHold_Ns         = 20;
    sram_config.tCeInterval_Ns     = 20; // CS interval
    sram_config.readHoldTime_Ns    = 20; // Only for sync mode
    sram_config.tAddrSetup_Ns      = 20; // ADDR setup and hold
    sram_config.tAddrHold_Ns       = 20;
    sram_config.tWeLow_Ns          = 20; // WRITE feature
    sram_config.tWeHigh_Ns         = 20; 
    sram_config.tReLow_Ns          = 20; // READ feature
    sram_config.tReHigh_Ns         = 20;
    sram_config.tTurnAround_Ns     = 20; // TA, do not set it to be 0
    sram_config.tAddr2WriteHold_Ns = 20; // ?
    sram_config.tWriteSetup_Ns     = 20; // For sync mode.
    sram_config.tWriteHold_Ns      = 20; // For sync mode.
    sram_config.latencyCount       = 20; // For sync mode.
    sram_config.readCycle          = 20; // For sync mode.
    sram_config.delayChain         = 20; // typically not used in SRAM.
    
    return SEMC_ConfigureSRAM(SEMC, &sram_config, clockFrq);
}

void test_sram(void)
{
    char * p0;
    p0 = (char *)0x91000000;
    
    p0[0] = 0; for(int i=0; i<1000*1000; i++); 
    // The follow code is used for timeout error process, just for test purpose.
    SEMC->INTR = 0xffffffff;
    SEMC->MCR  |= 1;
    asm(" dsb");
    while(SEMC->MCR & 1)
        ;
        
    p0[1] = 0; for(int i=0; i<1000*1000; i++);
    SEMC->INTR = 0xffffffff;
    SEMC->MCR  |= 1;
    asm(" dsb");
    while(SEMC->MCR & 1)
        ;
    
    p0[0] = 0; for(int i=0; i<1000*1000; i++); 
    SEMC->INTR = 0xffffffff;
    SEMC->MCR  |= 1;
    asm(" dsb");
    while(SEMC->MCR & 1)
        ;
        
    p0[1] = 0; for(int i=0; i<1000*1000; i++); 
    SEMC->INTR = 0xffffffff;
    SEMC->MCR  |= 1;
    asm(" dsb");
    while(SEMC->MCR & 1)
        ;
}

int main(void)
{
    /* Hardware initialize. */
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    
    BOARD_InitSRAM();    
    test_sram();
    while (1)
    {
    }
}
