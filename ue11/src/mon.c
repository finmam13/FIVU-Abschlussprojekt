#include "global.h"

#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "mon.h"
#include "app.h"
#include "sys.h"

#ifdef GLOBAL_MONITOR

// defines
// ...

// declarations and definations

int8_t mon_cmd_info (uint8_t argc, char *argv[]);
int8_t mon_cmd_test (uint8_t argc, char *argv[]);
int8_t mon_cmd_adc (uint8_t argc, char *argv[]);

const char MON_LINE_WELCOME[] PROGMEM = "Line-Mode: CTRL-X, CTRL-Y, CTRL-C, Return  \n\r";
const char MON_PMEM_CMD_INFO[] PROGMEM = "info\0Application infos\0info";
const char MON_PMEM_CMD_TEST[] PROGMEM = "test\0commando for test\0test";
const char MON_PMEM_CMD_ADC[] PROGMEM = "kalibrieren\0Eingabe der Kalibrierung\0kalibrierung <Kalibrierwert>";

const struct Sys_MonCmdInfo MON_PMEMSTR_CMDS[] PROGMEM =
{
    { MON_PMEM_CMD_INFO, mon_cmd_info }
  , { MON_PMEM_CMD_TEST, mon_cmd_test }
  , { MON_PMEM_CMD_ADC, mon_cmd_adc }
};

volatile struct Mon mon;

// functions

void mon_init (void)
{
  memset((void *)&mon, 0, sizeof(mon));
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
  while(EECR & (1<<EEWE))
   ;
  EEAR = 0;
  EEDR = ucData;
  EECR |= (1<<EEMWE);
  EECR |= (1<<EEWE);
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
  while(EECR & (1<<EEWE));
  EEAR = uiAddress;
  EECR |= (1<<EERE);
  return EEDR;
}
//--------------------------------------------------------

inline void mon_main (void)
{
}

inline uint8_t mon_getCmdCount (void)
{
  return sizeof(MON_PMEMSTR_CMDS)/sizeof(struct Sys_MonCmdInfo);
}


// --------------------------------------------------------
// Monitor commands of the application
// --------------------------------------------------------

int8_t mon_cmd_info (uint8_t argc, char *argv[])
{
  printf("app.flags_u8  : ");
  sys_printHexBin8(sys.flags_u8);
  sys_newline();
  return 0;
}


int8_t mon_cmd_test (uint8_t argc, char *argv[])
{
  uint8_t i;

  for (i = 0; i<argc; i++)
    printf("%u: %s\n\r", i, argv[i]);

  return 0;
}



int8_t mon_cmd_adc (uint8_t argc, char *argv[])
{
  float kalibrieren;
  sscanf(argv[1], "%f",kalibrieren);
  printf("Die richtige Spannung beträgt: %.2f",kalibrieren);
  
  uint8_t adc = app.adc_voltage;
  float vcc = 256*2.56/adc;
  char kalibrier_wert = (char)kalibrieren - vcc;
  EEPROM_write(0,kalibrier_wert);
}

// --------------------------------------------------------
// Monitor-Line for continues output
// --------------------------------------------------------

int8_t mon_printLineHeader (uint8_t lineIndex)
{
  if (lineIndex==0)
    sys_printString_P(MON_LINE_WELCOME);
  
  switch (lineIndex)
  {
    case 0: printf("L0 | app.flags_u8"); return 20;
    case 1: printf("L1 | counter  (press 'c' for reset)"); return 40;
    case 2: printf("L2 | ADC Wert"); return 15;
    default: return -1; // this line index is not valid
  }
}

int8_t mon_printLine   (uint8_t lineIndex, char keyPressed)
{

  switch (lineIndex)
  {
    case 0:
      printf("   | "); sys_printBin(app.flags_u8, 0);
      return 2;

    case 1:
      {
        static uint16_t cnt = 0;
        if (keyPressed=='c')
        cnt = 0;
        printf("   |  0x%04x   ", cnt++);
      }
      return 2;
     
    case 2:
    {
      uint8_t adc = app.adc_voltage;
      float vcc = 256*2.56/adc;
      printf("ADC= %3u -> %.2f \r",adc, vcc);
      char kalibrierung = EEPROM_read(0);
      printf("%.2f",vcc+(double)kalibrierung);
    }

    default: return -1;
  }
}

#endif // GLOBAL_MONITOR



