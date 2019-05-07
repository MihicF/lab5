/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 *
 *
 *
 */


#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xio.h"
#include "vga_periph_mem.h"
#include "xintc.h"
#include "xil_exception.h"

XIntc Intc;
unsigned int offset = 0, clock = 0, ind = 0;

void print(char *str);

void my_timer_interrupt_handler(void * baseaddr_p) {
	draw_circle(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR+offset*4);
	    	if(clock==50){
	    		if(offset==17){
	    			ind = 1;
				}

	    		if(offset == 0){
					ind = 0;
				}

				if(ind){
					offset--;
				}else{
					offset++;
				}

	    		clock = 0;
	    	}else{
	    		clock++;
	    	}
}

int main()
{
    init_platform();
    unsigned char string_s[] = "LPRS 2\n";
    XStatus Status;
    Xuint32 value1, value2, value3;

    xil_printf("Interrupt example\n\r");

    VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x00, 0x0);// direct mode   0
    VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x04, 0x3);// display_mode  1
    VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x08, 0x1);// show frame      2

    //VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x0C, 0x1);// font size       3
    set_font(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x0C,0x1);

    //VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x10, 0xFFFFFF);// foreground 4
    set_foreground_color(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x10, 0xFF0000);

    //VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x14, 0x0000FF);// background color 5
    set_background_color(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x14, 0x000000);
    VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x18, 0xFF0000);// frame color      6

    VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x2C, 0x0001DF);// terminal count      7
    VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x30, 0x000001);// enable      8



    print("Hello World\n\r");

    value1 = XIo_In32(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x2C);
    xil_printf("\n\rvalue1 = %x.", value1);

    //clear_text_screen(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR);
    //clear_graphics_screen(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR);
    clear_screen(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR);


    //draw_square(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR);
    set_cursor(350);
	print_string(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR, string_s, 6);
	print_char(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 40, 'A');


	Status = XIntc_Initialize (&Intc, XPAR_INTC_0_DEVICE_ID);
	 if (Status != XST_SUCCESS) xil_printf ("\r\nInterrupt controller initialization failure");
	 else xil_printf("\r\nInterrupt controller initialized");

	 microblaze_enable_interrupts();

    for(;;){
    	// Connect my_timer_interrupt_handler
    		 Status = XIntc_Connect (&Intc, XPAR_AXI_INTC_0_VGA_PERIPH_MEM_0_IRQ_O_INTR,
    				 (XInterruptHandler) my_timer_interrupt_handler,(void *)XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x2C);


    		 if (Status != XST_SUCCESS) xil_printf ("\r\nRegistering MY_TIMER Interrupt Failed");
    		  else xil_printf("\r\nMY_TIMER Interrupt registered");
    		  //start the interrupt controller in real mode
    		  Status = XIntc_Start(&Intc, XIN_REAL_MODE);
    		  //enable interrupt controller
    		  XIntc_Enable (&Intc, XPAR_AXI_INTC_0_VGA_PERIPH_MEM_0_IRQ_O_INTR);
    }

    cleanup_platform();

    return 0;
}
