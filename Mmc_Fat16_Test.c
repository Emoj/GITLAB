/*
 * Project name:
     Mmc_Fat16_Test (Demonstration on usage of Mmc_Fat16 library)
 * Copyright:
     (c) MikroElektronika, 2008
  * Revision History:
      20080930:
        - initial release;
 * Description:
     This project consists of several blocks that demonstrate various aspects of
     usage of the Mmc_Fat16 library. These are:
     - Creation of new file and writing down to it;
     - Opening existing file and re-writing it (writing from start-of-file);
     - Opening existing file and appending data to it (writing from end-of-file);
     - Opening a file and reading data from it (sending it to USART terminal);
     - Creating and modifying several files at once;
     - Reading file contents;
     - Deleting file(s);
     - Creating the swap file (see Help for details);
 * Test configuration:
     MCU:             ATmega128
     Dev.Board:       BigAVR2
     Oscillator:      External clock, 08.00000 MHz
     Ext. Modules:    on-board MMC/SD module
     SW:              mikroC PRO for AVR
 * NOTES:
     - Please make sure that MMC card is properly formatted (to FAT16 or just FAT)
       before testing it on this example!
     - Be sure to connect appropriate SPI pinout using switches SW2 (board specific)
     - This example expects MMC card to be inserted before reset, otherwise,
       the FAT_ERROR message is displayed!!!
 */

#include <built_in.h>

sbit Mmc_Chip_Select at PORTG.B1;
sbit Mmc_Chip_Select_Direction at DDRG.B1;

char
 fat_txt[20] = "FAT16 not found",
 file_contents[50] = "XX MMC/SD FAT16 library by Anton Rieckert\n";

char
 filename[14] = "MIKRO00xTXT";          // File names
unsigned short
 tmp, caracter, loop, loop2;
unsigned long
 i, size;
 
char Buffer[512];

//I-I-I--------- Writes string to USART
void I_Write_Str(char *ostr) {
  unsigned short i;

  i = 0;
  while (ostr[i]) {
    UART1_Write(ostr[i++]);
  }
  UART1_Write(0x0A);
}

//M-M-M--------- Creates new file and writes some data to it
void M_Create_New_File() {
  filename[7] = 'A';
  Mmc_Fat_Assign(&filename, 0xA0);      // Will not find file and then create file
  Mmc_Fat_Rewrite();                    // To clear file and start with new data
  for(loop = 1; loop <= 99; loop++) {   //  We want 5 files on the MMC card
    UART1_Write('.');
    file_contents[0] = loop / 10 + 48;
    file_contents[1] = loop % 10 + 48;
    Mmc_Fat_Write(file_contents, 42);   // write data to the assigned file
  }
}

//M-M-M--------- Creates many new files and writes data to them
void M_Create_Multiple_Files() {
  for(loop2 = 'B'; loop2 <= 'Z'; loop2++) {
    UART1_Write(loop2);             // signal the progress
    filename[7] = loop2;                 // set filename
    Mmc_Fat_Assign(&filename, 0xA0);        // find existing file or create a new one
    Mmc_Fat_Rewrite();                   // To clear file and start with new data
    for(loop = 1; loop <= 44; loop++) {
      file_contents[0] = loop / 10 + 48;
      file_contents[1] = loop % 10 + 48;
      Mmc_Fat_Write(file_contents, 42);  // write data to the assigned file
    }
  }
}

//M-M-M--------- Opens an existing file and rewrites it
void M_Open_File_Rewrite() {
  filename[16] = 'C';
  Mmc_Fat_Assign(&filename, 0);
  Mmc_Fat_Rewrite();
  for(loop = 1; loop <= 55; loop++) {
    file_contents[0] = loop / 10 + 64;
    file_contents[1] = loop % 10 + 64;
    Mmc_Fat_Write(file_contents, 42);    // write data to the assigned file
  }
}

//M-M-M--------- Opens an existing file and appends data to it
//               (and alters the date/time stamp)
void M_Open_File_Append() {
     filename[7] = 'B';
     Mmc_Fat_Assign(&filename, 0);
     Mmc_Fat_Set_File_Date(2005,6,21,10,35,0);
     Mmc_Fat_Append();                                    // Prepare file for append
     Mmc_Fat_Write(" for mikroElektronika 2005\n", 27);   // Write data to assigned file
}//~

//M-M-M--------- Opens an existing file, reads data from it and puts it to USART
void M_Open_File_Read() {
  filename[16] = 'B';
  Mmc_Fat_Assign(&filename, 0);
  Mmc_Fat_Reset(&size);                 // To read file, procedure returns size of file
  for (i = 1; i <= size; i++) {
    Mmc_Fat_Read(&caracter);
    UART1_Write(caracter);         // Write data to USART
  }
}

//M-M-M--------- Deletes a file. If file doesn't exist, it will first be created
//               and then deleted.
void M_Delete_File() {
  filename[7] = 'F';
  Mmc_Fat_Assign(filename, 0);
  Mmc_Fat_Delete();
}

//M-M-M--------- Tests whether file exists, and if so sends its creation date
//               and file size via USART
void M_Test_File_Exist() {
  unsigned long fsize;
  unsigned int year;
  unsigned short month, day, hour, minute;
  unsigned char outstr[12];

  filename[7] = 'B';       //uncomment this line to search for file that DOES exists
//  filename[7] = 'F';       //uncomment this line to search for file that DOES NOT exist
  if (Mmc_Fat_Assign(filename, 0)) {
    //--- file has been found - get its date
    Mmc_Fat_Get_File_Date(&year, &month, &day, &hour, &minute);
    WordToStr(year, outstr);
    I_Write_Str(outstr);
    ByteToStr(month, outstr);
    I_Write_Str(outstr);
    WordToStr(day, outstr);
    I_Write_Str(outstr);
    WordToStr(hour, outstr);
    I_Write_Str(outstr);
    WordToStr(minute, outstr);
    I_Write_Str(outstr);
    //--- get file size
    fsize = Mmc_Fat_Get_File_Size();
    LongToStr((signed long)fsize, outstr);
    I_Write_Str(outstr);
  }
  else {
    //--- file was not found - signal it
    UART1_Write(0x55);
    Delay_ms(1000);
    UART1_Write(0x55);
  }
}


//-------------- Tries to create a swap file, whose size will be at least 100
//               sectors (see Help for details)
void M_Create_Swap_File() {
  unsigned int i;

  for(i=0; i<512; i++)
    Buffer[i] = i;

  size = Mmc_Fat_Get_Swap_File(5000, "mikroE.txt", 0x20);   // see help on this function for details

  if (size) {
    LongToStr((signed long)size, fat_txt);
    I_Write_Str(fat_txt);

    for(i=0; i<5000; i++) {
      Mmc_Write_Sector(size++, Buffer);
      UART1_Write('.');
    }
  }
}

//-------------- Main. Uncomment the function(s) to test the desired operation(s)
void main() {
     // we will use PORTC to signal test end
     DDRC  = 0xFF;
<<<<<<< HEAD
     PORTC = 0xF0;  
     //--- set up USART for the file read
     UART1_Init(4800);
=======
     PORTC = 0x0F;  
     //--- set up USART for the file read
     UART1_Init(960000);
>>>>>>> d896bf17839506999267f31c95147fa12b72e598
     Delay_100ms();
     //--- init the FAT library
     SPI1_Init_Advanced(_SPI_MASTER, _SPI_FCY_DIV128, _SPI_CLK_LO_LEADING);
     Spi_Rd_Ptr = SPI1_Read;
     // use fat16 quick format instead of init routine if a formatting is needed
     if (!Mmc_Fat_Init()) {
       // reinitialize spi at higher speed
       SPI1_Init_Advanced(_SPI_MASTER, _SPI_FCY_DIV2, _SPI_CLK_LO_LEADING);
       //--- Test start
       //--- Test routines. Uncomment them one-by-one to test certain features
       M_Create_New_File();
       M_Create_Multiple_Files();

       M_Open_File_Rewrite();
       M_Open_File_Append();
       M_Open_File_Read();
       M_Delete_File();
       M_Test_File_Exist();      
       M_Create_Swap_File();
       UART1_Write('e');     
     }
     else {
       I_Write_Str(fat_txt);
     }
     //--- Test termination
     PORTC = 0x0F;
}
