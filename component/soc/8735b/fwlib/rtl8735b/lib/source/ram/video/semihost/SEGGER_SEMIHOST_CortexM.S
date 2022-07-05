/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*            (c) 2019 SEGGER Microcontroller GmbH                    *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* Redistribution and use in source and binary forms, with or         *
* without modification, are permitted provided that the following    *
* condition is met:                                                  *
*                                                                    *
* o Redistributions of source code must retain the above copyright   *
*   notice, this condition and the following disclaimer.             *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND             *
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,        *
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF           *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
* DISCLAIMED. IN NO EVENT SHALL SEGGER Microcontroller BE LIABLE FOR *
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT  *
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;    *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF      *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT          *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE  *
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
* DAMAGE.                                                            *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : SEGGER_SEMIHOST_CortexM.S
Purpose : Semihosting handler implementation for Cortex-M devices,
          using software breakpoints (BKPT)
*/

        .syntax unified
        .thumb

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
        .global SEGGER_SEMIHOST_X_Request
        .type   SEGGER_SEMIHOST_X_Request, function

/*********************************************************************
*
*       SEGGER_SEMIHOST_X_Request()
*
*  Function description
*    Notify the debugger host about the semihosting request
*
*  Parameters
*    R0: Op:         Operation.
*    R1: pPara:      Pointer to parameter block.
*
*  Return value
*    Operation-specific return value, set by the debugger in R0.
*
*  Note
*    Return value in case debugger is not connected is unfortunately 
*    the same as <Op>, since the same register is used.
*
*/
        .balign 4
        .text

SEGGER_SEMIHOST_X_Request:   // int SEGGER_SEMIHOST_X_Request(int Op, void* pPara) {
        bkpt    0xAB         // Option 0xAB indicates to debugger that this is a semihosting BP and the debugger shall automatically continue program execution
        bx      LR           // return
        .end

/*************************** End of file ****************************/
