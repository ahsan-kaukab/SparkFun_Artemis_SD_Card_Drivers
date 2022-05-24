#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include <stdint.h>
#include "SD_SPI.h"
//#include "SD.h"
#include "ff.h"

void init_system(void)
{
  
    am_bsp_uart_printf_enable();
    //
    // Print the banner.
    //	
	spi_initialize();
	// SD_init();
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("PDM FFT example.\n\n");

}

//FAT function test: format test, file write test, file read test (basic function)
FATFS fs; //FatFs file system object
FIL fnew; //File object
FRESULT res_sd;//File operation result
UINT fnum; //Number of files successfully read and written
BYTE ReadBuffer[1024] = {0};
BYTE WriteBuffer[] = "Successfully transplanted the FatFs file system!\r\n"; //Write buffer

int test_card()
{
	spi_initialize();
	
	if(SD_init() == 0)
	{
		 am_util_stdio_printf("SD card initialization is successful, SD card will be mounted soon.\r\n");
	}
	
	// first unmount 
	f_mount(NULL, "0:", 1);
	
	 //Mount SD card
	res_sd = f_mount(&fs, "0:", 1);

	am_util_stdio_printf("Respnse to mount is %d \n", res_sd);
	 //***********************Formatting test********************** ******
	if(res_sd == FR_NO_FILESYSTEM)
	{
		 am_util_stdio_printf("SD card has no file system and will be formatted soon...\r\n");
		 //Format
		res_sd = f_mkfs("0:", 0, 0, 1024);
		
		if(res_sd == FR_OK)
		{
			 am_util_stdio_printf("SD card formatted successfully!\r\n");
			 //Unmount first after formatting
			res_sd = f_mount(NULL, "0:", 1);
			 //Remount
			res_sd = f_mount(&fs, "0:", 1);
		}
		else
		{
			 am_util_stdio_printf("File formatting failed! Error code: %d\r\n",res_sd);
			while(1);
		}
	}
	else if(res_sd != FR_OK)
	{
		 am_util_stdio_printf("Failed to mount the file system! It may be because the file initialization failed! Error code: %d\r\n", res_sd);
	}
	else
	{
		 am_util_stdio_printf("The file system is successfully mounted, read and write tests can be performed!\r\n");
	}
	
	 //***********************Write test*********************** *****
	 //Open the file, if the file does not exist, create it
	 am_util_stdio_printf("File writing test is about to be carried out...\r\n");
	 //Open the file, create it if it does not exist
	 res_sd = f_open(&fnew, "0: FatFs read and write test file.txt", FA_CREATE_ALWAYS | FA_WRITE);
	//File opened successfully
	if(res_sd == FR_OK)
	{
		 am_util_stdio_printf("Open the file successfully! Start writing data!\r\n");
		res_sd= f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
		
		if(res_sd == FR_OK)
		{
			 am_util_stdio_printf("Data written successfully!\r\n");
			 am_util_stdio_printf("Data: %s. Write a total of %d characters\r\n", WriteBuffer, fnum);
		}
		else
		{
			 am_util_stdio_printf("Failed to write data!\r\n");
		}
		
		 //Close the file
		f_close(&fnew);
	}
	
	 //*********************** Reading test ************************* *****
	 //Open the file, if the file does not exist, create it
	 am_util_stdio_printf("File reading test is about to be performed...\r\n");
	 //Open the file, create it if it does not exist
	 res_sd = f_open(&fnew, "0: FatFs read and write test file.txt", FA_OPEN_EXISTING | FA_READ);
	 //File opened successfully
	if(res_sd == FR_OK)
	{
		 am_util_stdio_printf("Open the file successfully! Start reading data!\r\n");
		res_sd= f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
		
		if(res_sd == FR_OK)
		{
			 am_util_stdio_printf("Data read successfully!\r\n");
			 am_util_stdio_printf("Data: %s\r\n", ReadBuffer);
		}
		else
		{
			 am_util_stdio_printf("Failed to read data!\r\n");
		}
		
		 //Close the file
		f_close(&fnew);
	}
	
	 // scan_files("FatFs read and write test file.txt");
	
	 //Other functional tests
	// file_check();
	
	 //Multiple functional tests
	// miscellaneous();
	
	 //Unmount the file system
	f_mount(NULL, "0:", 1);
	
	while(1);
}

void read()
{
	
	//init_system();
	
	FIL *filw;
	int check=0;
	// char fname[9]="blink.bin";
	// ///mnt/sdcard "/storage/sdcard/blink.bin"
	// check=f_open(fil0,"/mnt/sdcard/blink.bin",FA_READ);
	
	 // if( check )// Open the file in read-only mode
	 // {
		 // am_util_stdio_printf("File Error %d\n",check); 
	 // }
	 // else
	 // {
		 // am_util_stdio_printf("File can not open");
	 // }
	 
	//FA_WRITE | FA_CREATE_ALWAYS | FA_CREATE_NEW	
		
	check = f_open(filw,"hello.txt",FA_WRITE|FA_CREATE_NEW);
	
	if(check )// Open the file in read-only mode
	{
		am_util_stdio_printf("File Error %d\n",check); 
	}
	else
	{
		am_util_stdio_printf("File open successfully...\n");
	}
	
	int br;
	char buff[] = "hello";
	//int check=0; 

	check = f_write(filw, buff, sizeof(buff), &br);
	//am_util_stdio_printf("\nBytes read %d\n",check);     // send through the serial port
	//am_util_stdio_printf("Data %d\n",check); 
	if(!check)
		am_util_stdio_printf("File write successfull \n"); 
	else
	{
		check = f_read ( filw, &buff, sizeof(buff), &br);
		am_util_stdio_printf("File write status: %d\n",check); 
	}
	
			
  
}
void write()
{
	
}

int main(void)
{
	am_bsp_uart_printf_enable();
	init_system();
	spi_cs_set();
	// SD_init();
	test_card();
	read();
}

