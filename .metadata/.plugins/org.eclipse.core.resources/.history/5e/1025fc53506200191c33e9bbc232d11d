/*
===============================================================================
 Name        : uart_usb.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

/*==================[inlcusiones]============================================*/
#include <stdarg.h>
#include <stdio.h>
#include "chip.h"

#include "conf_uart_usb.h"
#include "_ring_buffer.h"
#include "uart_usb.h"

/*==================[definiciones y macros]==================================*/

/**
* @def INACCURATE_TO_MS
* @brief Numero de ciclos en 1ms del lpc4337 a 204MHz
*/
#define INACCURATE_TO_MS       20400

/*==================[definiciones de datos internos]=========================*/
/**
* @var static bool_t gInitFlag
* @brief Indicador del estado de inicializacion del driver
*/
static bool gInitFlag;

/**
* @var static uint8_t gRxBuff[UART_USB_RX_BUFFER_SIZE]
* @brief Espacio de memoria a ser utilizado como buffer circular de recepcion de la UART
*/
static uint8_t gRxBuff[UART_USB_RX_BUFFER_SIZE];

/**
* @var static struct ring_buffer gRxBuffHandler
* @brief Buffer circular de recepcion de la UART
*/
static struct ring_buffer gRxBuffHandler;

/**
* @var static uint8_t gTxBuff[UART_USB_TX_BUFFER_SIZE]
* @brief Espacio de memoria a ser utilizado como buffer circular de transmision de la UART
*/
static uint8_t gTxBuff[UART_USB_TX_BUFFER_SIZE];

/**
* @var static struct ring_buffer gTxBuffHandler
* @brief Buffer circular de transmision de la UART
*/
static struct ring_buffer  gTxBuffHandler;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

/*==================[definiciones de funciones internas]=====================*/

/**
* @fn void UART2_IRQHandler(void)
* @brief Manejador de interrupcion de la  UART2
* @param Ninguno
* @return Nada
* @note 0x2a 0x000000A8 - Handler for ISR UART2 (IRQ 26)
*/
void UART2_IRQHandler(void){

   /* Handle transmit interrupt if enabled */
   if (LPC_USART2->IER & UART_IER_THREINT) {

         /* Fill FIFO until full or until TX ring buffer is empty */
         if(!ringBufferIsEmpty(&gTxBuffHandler)) {
            if(Chip_UART_ReadLineStatus(LPC_USART2) & UART_LSR_THRE) {
               uint8_t ch = ringBufferGet(&gTxBuffHandler);

               Chip_UART_SendByte(LPC_USART2, ch);
            }
         }
         else {
            /* Shut down transmit */
            Chip_UART_IntDisable(LPC_USART2, UART_IER_THREINT);
         }
   }

   /* Handle receive interrupt if enabled */
   if (LPC_USART2->IER & UART_IER_RBRINT) {
      if (Chip_UART_ReadLineStatus(LPC_USART2) & UART_LSR_RDR) {

         uint8_t ch = Chip_UART_ReadByte(LPC_USART2);

         if(!ringBufferIsFull(&gRxBuffHandler)) {

            ringBufferPut(&gRxBuffHandler, ch);
         }
      }
   }
}

/** 
 * @fn      static void delayInaccurate(tick_t delay_ms)
 * @brief   Delay inpreciso en milisegundos
 * @param   delay_ms : Tiempo en milisegundos de delay
 * @return  Nada
 */
static void delayInaccurate(uint32_t delay_ms)
{
   volatile uint32_t i;
   volatile uint32_t delay;

   delay = INACCURATE_TO_MS * delay_ms;

   for( i=delay; i>0; i-- );
}

/*==================[definiciones de funciones externas]=====================*/
void uartUsbInit(void) {

   /* Configurar la uart USB por interrupcion para recibir y enviar mensajes */
   Chip_UART_Init(LPC_USART2);
   Chip_UART_SetBaud(LPC_USART2, UART_USB_BAUDRATE);                       /* Set Baud rate */
   Chip_UART_SetupFIFOS(LPC_USART2, UART_FCR_FIFO_EN | UART_FCR_TRG_LEV0); /* Modify FCR (FIFO Control Register)*/
   Chip_UART_TXEnable(LPC_USART2);                                         /* Enable UART Transmission */
   Chip_SCU_PinMux(7, 1, MD_PDN, FUNC6);                                   /* P7_1,FUNC6: UART2_TXD */
   Chip_SCU_PinMux(7, 2, MD_PLN|MD_EZI|MD_ZI, FUNC6);                      /* P7_2,FUNC6: UART2_RXD */

   /* Inicializar buffer circular de recepcion */
   gRxBuffHandler = ringBufferInit(gRxBuff, sizeof(gRxBuff));

   /* Inicializar buffer circular de envio */
   gTxBuffHandler = ringBufferInit(gTxBuff, sizeof(gTxBuff));

   //Enable UART Rx Interrupt
   Chip_UART_IntEnable(LPC_USART2, UART_IER_RBRINT);                       /* Receiver Buffer Register Interrupt */

   // Enable UART line status interrupt
   Chip_UART_IntEnable(LPC_USART2, UART_IER_RLSINT);                       /* LPC43xx User manual page 1118 */
   NVIC_SetPriority(USART2_IRQn, 6);

   // Enable Interrupt for UART channel
   NVIC_EnableIRQ(USART2_IRQn);

   /* Marcar como inicializado el driver uart usb */
   gInitFlag = TRUE;
}

bool uartUsbReadByte(uint8_t* byte) {

   if(gInitFlag && byte) {

      if(!ringBufferIsEmpty(&gRxBuffHandler)) {
         *byte = ringBufferGet(&gRxBuffHandler);

         return TRUE;
      }
   }

   return FALSE;
}

uint8_t uartUsbReadData(uint8_t* buff, const uint8_t expBytesLen, const uint32_t timeout) {

   if(gInitFlag && buff && expBytesLen && timeout) {

      uint32_t remainingBytes = expBytesLen;
      uint32_t remainingLoops = timeout;

      do
      {
         if(!uartUsbReadByte(buff))
         {
            delayInaccurate(1);

            if(remainingLoops)
               --remainingLoops;

            continue;
         }

         --remainingBytes;
         buff++;
      } while(remainingLoops && remainingBytes);

      return expBytesLen - remainingBytes;
   }

   return 0;
}

bool uartUsbWriteByte(const uint8_t byte) {

   /* Wait until buffer has space to insert more items */
   while(ringBufferIsFull(&gTxBuffHandler))
      ;

   ringBufferPut(&gTxBuffHandler, byte);

   if(!(Chip_UART_GetIntsEnabled(LPC_USART2) & UART_IER_THREINT)) {
      uint8_t ch = ringBufferGet(&gTxBuffHandler);

      Chip_UART_SendByte(LPC_USART2, ch);

      /* Habilitar interrupción de transmision de la UART */
      Chip_UART_IntEnable(LPC_USART2, UART_IER_THREINT);
   }

   return TRUE;
}

uint8_t uartUsbWriteData(const uint8_t* buff, const uint32_t len) {

   uint8_t ret = 0;

   if(len && buff) {

      uint32_t i;

      for(i = 0;i < len;i++) {

         if(!uartUsbWriteByte(buff[i])) {
            ret = i;
            break;
         }
      }

      if(len == i)
         ret = len;
   }

   return ret;
}

uint8_t uartUsbWriteString(const uint8_t* string) {

   uint8_t ret = 0;

   if(string) {

      uint32_t i;

      for(i = 0;0 != string[i];i++) {

         if(!uartUsbWriteByte(string[i])) {
            ret = i;
            break;
         }
      }

      if(0 == string[i])
         ret = i;
   }

   return ret;
}

uint8_t uartUsbWriteVariadicString(const uint8_t* format, ...) {

   int32_t size, ret = 0;
   uint8_t string[255];
   va_list arg;

   va_start(arg, format);
   size = vsnprintf((char *) string, sizeof(string), (const char *) format, arg);
   va_end(arg);

   if(size > 0) {

      uint32_t i;

      for(i = 0;0 != string[i];i++) {

         if(!uartUsbWriteByte(string[i])) {
            ret = i;
            break;
         }
      }

      if(0 == string[i])
         ret = i;
   }

   return ret;
}

/*==================[fin del archivo]========================================*/

