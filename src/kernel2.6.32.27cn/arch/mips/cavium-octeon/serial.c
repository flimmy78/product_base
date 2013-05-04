/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004-2007 Cavium Networks
 */
#include <linux/console.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>
#include <linux/tty.h>

#include <asm/time.h>

#include <asm/octeon/octeon.h>

#if defined(CONFIG_CAVIUM_GDB)

static irqreturn_t interrupt_debug_char(int cpl, void *dev_id)
{
	unsigned long lsrval;
	unsigned long tmp;

	lsrval = cvmx_read_csr(CVMX_MIO_UARTX_LSR(OCTEON_DEBUG_UART));
	if (lsrval & 1) {
#ifdef CONFIG_KGDB
		/*
		 * The Cavium EJTAG bootmonitor stub is not compatible
		 * with KGDB.  We should never get here.
		 */
#error Illegal to use both CONFIG_KGDB and CONFIG_CAVIUM_GDB
#endif
		/*
		 * Pulse MCD0 signal on Ctrl-C to stop all the
		 * cores. Also set the MCD0 to be not masked by this
		 * core so we know the signal is received by
		 * someone.
		 */
		octeon_write_lcd("brk");
		asm volatile ("dmfc0 %0, $22\n\t"
			      "ori   %0, %0, 0x10\n\t"
			      "dmtc0 %0, $22" : "=r" (tmp));
		octeon_write_lcd("");
		return IRQ_HANDLED;
	}
	return IRQ_NONE;
}

/* Enable uart1 interrupts for debugger Control-C processing */

static int octeon_setup_debug_uart(void)
{
	if (request_irq(OCTEON_IRQ_UART0 + OCTEON_DEBUG_UART, interrupt_debug_char,
			IRQF_SHARED, "KGDB", interrupt_debug_char)) {
		panic("request_irq(%d) failed.", OCTEON_IRQ_UART0 + OCTEON_DEBUG_UART);
	}
	cvmx_write_csr(CVMX_MIO_UARTX_IER(OCTEON_DEBUG_UART),
		       cvmx_read_csr(CVMX_MIO_UARTX_IER(OCTEON_DEBUG_UART)) | 1);
	return 0;
}

/* Install this as early as possible to be able to debug the boot
   sequence.  */
core_initcall(octeon_setup_debug_uart);
#endif	/* CONFIG_CAVIUM_GDB */

unsigned int octeon_serial_in(struct uart_port *up, int offset)
{
	int rv = cvmx_read_csr((uint64_t)(up->membase + (offset << 3)));
	if (offset == UART_IIR && (rv & 0xf) == 7) {
		/* Busy interrupt, read the USR (39) and try again. */
		cvmx_read_csr((uint64_t)(up->membase + (39 << 3)));
		rv = cvmx_read_csr((uint64_t)(up->membase + (offset << 3)));
	}
	return rv;
}

void octeon_serial_out(struct uart_port *up, int offset, int value)
{
	/*
	 * If bits 6 or 7 of the OCTEON UART's LCR are set, it quits
	 * working.
	 */
	if (offset == UART_LCR)
		value &= 0x9f;
	cvmx_write_csr((uint64_t)(up->membase + (offset << 3)), (u8)value);
}

static int __devinit octeon_serial_probe(struct platform_device *pdev)
{
	int irq, res;
	struct resource *res_mem;
	struct uart_port port;

	/* All adaptors have an irq.  */
	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	memset(&port, 0, sizeof(port));

	port.flags = ASYNC_SKIP_TEST | UPF_SHARE_IRQ | UPF_FIXED_TYPE;
	port.type = PORT_OCTEON;
	port.iotype = UPIO_MEM;
	port.regshift = 3;
	port.dev = &pdev->dev;

	if (octeon_is_simulation())
		/* Make simulator output fast*/
		port.uartclk = 115200 * 16;
	else
		port.uartclk = octeon_get_io_clock_rate();

	port.serial_in = octeon_serial_in;
	port.serial_out = octeon_serial_out;
	port.irq = irq;

	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res_mem == NULL) {
		dev_err(&pdev->dev, "found no memory resource\n");
		return -ENXIO;
	}

	if (res_mem->start == 0x1180000000800ull && (octeon_serial_uartdisable_mask & 1)) {
		pr_notice("UART0 disabled on command line\n");
		return 0;
	}
	if (res_mem->start == 0x1180000000c00ull && (octeon_serial_uartdisable_mask & 2)) {
		pr_notice("UART1 disabled on command line\n");
		return 0;
	}
	if (res_mem->start == 0x1180000000400ull && (octeon_serial_uartdisable_mask & 4)) {
		pr_notice("UART2 disabled on command line\n");
		return 0;
	}

	port.mapbase = res_mem->start;
	port.membase = ioremap(res_mem->start, resource_size(res_mem));

	res = serial8250_register_port(&port);

	return res >= 0 ? 0 : res;
}

static struct of_device_id octeon_serial_match[] = {
	{
		.compatible = "cavium,octeon-3860-uart",
	},
	{},
};
MODULE_DEVICE_TABLE(of, octeon_serial_match);

static struct platform_driver octeon_serial_driver = {
	.probe		= octeon_serial_probe,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "octeon_serial",
		.of_match_table = octeon_serial_match,
	},
};

static int __init octeon_serial_init(void)
{
	return platform_driver_register(&octeon_serial_driver);
}
late_initcall(octeon_serial_init);
