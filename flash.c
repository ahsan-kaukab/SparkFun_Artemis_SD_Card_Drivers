//*****************************************************************************
//
//! @file flash_write.c
//!
//! @brief Flash write example.
//!
//! Purpose: This example shows how to modify the internal Flash using HAL flash helper
//! functions.
//!
//! This example works on instance 1 of the Flash, i.e. the portion of the
//! Flash above 256KB/512KB.
//!
//! Printing takes place over the ITM at 1M Baud.
//!
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2020, Ambiq Micro
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 2.4.2 of the AmbiqSuite Development Package.
//
//*****************************************************************************

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "ff.h"
#include "flash.h"

//! Arbitrary page address in flash instance 1.
#define ARB_PAGE_ADDRESS (AM_HAL_FLASH_INSTANCE_SIZE + (2 * AM_HAL_FLASH_PAGE_SIZE))
#define USERCODE_OFFSET (0xC000 + 0x4000) // location in flash to begin storing user's code (Linker script needs to be adjusted to offset user's flash to this address)
static int instance =0;

static uint32_t ui32Source[512];



int32_t i32ReturnCode;
    int32_t i32ErrorFlag = 0;
    uint32_t *pui32Src;
    uint32_t *pui32Dst;
    int32_t ix;
   
int flash_init(void)
{
    ui32PrgmAddr = ARB_PAGE_ADDRESS;

    //
    // Erase the whole block of FLASH instance 1.
    //
    #ifdef OWN_PRINT
    am_util_stdio_printf("  ... erasing all of flash instance %d.\n", AM_HAL_FLASH_ADDR2INST(ui32PrgmAddr) );
    #endif
    i32ReturnCode = am_hal_flash_mass_erase(AM_HAL_FLASH_PROGRAM_KEY, 1);

    //
    // Check for an error from the HAL.
    //
    if (i32ReturnCode)
    {
        #ifdef OWN_PRINT
        am_util_stdio_printf("FLASH_MASS_ERASE i32ReturnCode =  0x%x.\n",
                             i32ReturnCode);
        #endif                     
        i32ErrorFlag++;
    }

    //
    // Setup a pattern to write to the FLASH.
    //
    #ifdef OWN_PRINT
    am_util_stdio_printf("  ... programming flash instance %d, page %d.\n",
                          AM_HAL_FLASH_ADDR2INST(ui32PrgmAddr),
                          AM_HAL_FLASH_ADDR2PAGE(ui32PrgmAddr) );
    #endif



    return i32ErrorFlag;
	
}
uint16_t flash_read(uint32_t read_size)
{
    #ifdef OWN_PRINT
	am_util_stdio_printf("  ... verifying the page just programmed.\n");
    #endif
	
    for ( ix = 0; ix <read_size; ix++ )
    {
       // if ( *(uint32_t*)(ui32PrgmAddr + (ix*4)) != ui32Source[ix] )
       // {
		   int num=0;
           #ifdef OWN_PRINT
            am_util_stdio_printf("Flash address 0x%08x address :\n"
								"Expected value = 0x%08x, programmed value = 0x%08x.\n",
                                 ui32PrgmAddr + (ix * 4) -0x00084000,
                                 ui32Source[ix],
                                 *(uint32_t*)(ui32PrgmAddr + (ix * 4)) );
           #endif                      
			
			//f_write(fwrite, (uint32_t*)(ui32PrgmAddr + (ix * 4)) ,32, &num);
       // }
    }

   
	return (ui32PrgmAddr);
}
int flash_write(uint32_t *ui32Source_t, uint32_t size_wr)
{
	memset(ui32Source, 0x00, size_wr);

	 // 
	 // {
		 // ui32PrgmAddr += 0x0800; //-0x1800
		 // instance=0;
	 // }
	
	i32ErrorFlag = 0;
		///////             Write from SD Card /////////////////////////////////////////////

	 pui32Src = ui32Source;
     for (ix = 0; ix < (size_wr ); ix++)
     {
		   
		*pui32Src++ = (uint32_t)ui32Source_t[ix];
     }

    //
    // Program a few words in a page in the main block of instance 1.
    //
    pui32Dst = (uint32_t *) ui32PrgmAddr;


    i32ReturnCode = am_hal_flash_program_main(AM_HAL_FLASH_PROGRAM_KEY,
                                              ui32Source,
                                              pui32Dst,
                                              size_wr);


    if (i32ReturnCode)
    {
        #ifdef OWN_PRINT
          am_util_stdio_printf("FLASH program page at 0x%08x "
                             "i32ReturnCode = 0x%x.\n",
                             ui32PrgmAddr,
                             i32ReturnCode);
        #endif      

        i32ErrorFlag++;
    }
	uint16_t addr = flash_read(size_wr);
	am_util_stdio_printf("Check: ");
	
	 //instance ++;
	 	ui32PrgmAddr += 0x200;
	return i32ErrorFlag;
	
}

int erase_flash()
{
	i32ErrorFlag=0;

	#ifdef OWN_PRINT
    am_util_stdio_printf("  ... erasing the page just programmed.\n");
    i32ReturnCode = am_hal_flash_page_erase(AM_HAL_FLASH_PROGRAM_KEY,
                                            AM_HAL_FLASH_ADDR2INST(ui32PrgmAddr),
                                            AM_HAL_FLASH_ADDR2PAGE(ui32PrgmAddr) );
    #endif

    if (i32ReturnCode)
    {
        #ifdef OWN_PRINT
        am_util_stdio_printf(" FLASH erase page at 0x%08x "
                             "i32ReturnCode =  0x%x.\n",
                             ARB_PAGE_ADDRESS, i32ReturnCode);
        #endif        

        i32ErrorFlag++;
    }

    #ifdef OWN_PRINT
    am_util_stdio_printf("  ... verifying the page just erased.\n");
    #endif

    for ( ix = 0; ix < 512; ix++ )
    {
        if ( *(uint32_t*)(ui32PrgmAddr + (ix*4)) != 0xFFFFFFFF )
        {
            #ifdef OWN_PRINT
            am_util_stdio_printf("ERROR: flash address 0x%08x did not ERASE properly:\n"
                                 "  Expected value = 0xFFFFFFFF, programmed value = 0x%08x.\n",
                                 ui32PrgmAddr + (ix*4),
                                 *(uint32_t*)(ui32PrgmAddr + (ix * 4)) );
            #endif   

        }
    }

    if (i32ErrorFlag)
    {
        #ifdef OWN_PRINT
        am_util_stdio_printf("ERROR: FLASH Write example failure %d\n",
                             i32ErrorFlag);
        #endif                     
    }
    else
    {
        #ifdef OWN_PRINT
        am_util_stdio_printf("FLASH Write example successful \n");
        #endif
        
    }

    am_hal_itm_not_busy();
	
	return i32ErrorFlag;
}