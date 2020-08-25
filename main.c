
/* Led red Blink after receive data from uart channel*/

//Add libraries
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.c"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"
#include "driverlib/uart.h"
#include "driverlib/uart.c"
#include "driverlib/interrupt.h"
#include "driverlib/interrupt.c"
//Define Variables
#define RX0_PIN          GPIO_PIN_0
#define TX0_PIN          GPIO_PIN_1
char data;
char Buffer[100];
int8_t i=0;
// UART INTERRUPT HANDLER

void Receive_ISR(void)
{

    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status /value true because they're masked interrupt (they are enabled by code)
    UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
    while(UARTCharsAvail(UART0_BASE)==1)
    {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 2); //turn on led
    SysCtlDelay(SysCtlClockGet()/3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0); //turn off LED
    data=UARTCharGet(UART0_BASE);
    UARTCharPut(UART0_BASE, data); //If dont get the char, FIFO data stay and will not escape the WHILE
    Buffer[i]=data;
    i++;
    if(data=='x')
    i=0;
    }
}
//MAIN
int main(void){
// Clock configure at 50Mhz
SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
// UART configure
SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
GPIOPinConfigure(GPIO_PA0_U0RX);            //PIN mux for UART
GPIOPinConfigure(GPIO_PA1_U0TX);            //PIN mux for UART
GPIOPinTypeUART(GPIO_PORTA_BASE, RX0_PIN |TX0_PIN);
UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
// Interrupt configure
IntMasterEnable();
IntEnable(INT_UART0);
UARTIntEnable(UART0_BASE, UART_INT_RX| UART_INT_RT); //Receive and receive timeout in case of data don't come after 30 cycles
UARTIntRegister(UART0_BASE, Receive_ISR);            //If don't have timeout, system will interrupt after receive 8 bits data

// LED CONFIG
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE ,GPIO_PIN_1);
//TEST SEND



while (1)
{
    UARTCharPut(UART0_BASE, 'T');
    UARTCharPut(UART0_BASE, 'H');
    UARTCharPut(UART0_BASE, 'U');
    UARTCharPut(UART0_BASE, 'A');
    UARTCharPut(UART0_BASE, 'N');
}
}


