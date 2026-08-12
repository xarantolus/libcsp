#include <csp/csp_hooks.h>

#include <unistd.h>
/* sysinfo(2) doesn't exist on macOS: free memory is reported via mach host
 * statistics there. reboot(2) exists on both, but takes BSD RB_* commands on
 * macOS instead of the LINUX_REBOOT_* ones (either way the call is a no-op
 * without root privileges, failing with EPERM). Linux is untouched. */
#if defined(__APPLE__)
#include <mach/mach.h>
#include <sys/reboot.h>
#define CSP_REBOOT_CMD_RESTART RB_AUTOBOOT
#define CSP_REBOOT_CMD_HALT    RB_HALT
#elif defined(__CYGWIN__)
#include <sys/sysinfo.h>
#include <csp/csp_debug.h>
#else
#include <sys/sysinfo.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#define CSP_REBOOT_CMD_RESTART LINUX_REBOOT_CMD_RESTART
#define CSP_REBOOT_CMD_HALT    LINUX_REBOOT_CMD_HALT
#endif

uint32_t csp_memfree_hook(void) {
#if defined(__APPLE__)
	vm_size_t page_size;
	vm_statistics64_data_t vm_stats;
	mach_port_t host = mach_host_self();
	mach_msg_type_number_t count = sizeof(vm_stats) / sizeof(natural_t);
	if (host_page_size(host, &page_size) == KERN_SUCCESS &&
		host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm_stats, &count) == KERN_SUCCESS) {
		return (uint32_t)((uint64_t)vm_stats.free_count * page_size);
	}
	return 0;
#else
	uint32_t total = 0;
	struct sysinfo info;
	sysinfo(&info);
	total = info.freeram * info.mem_unit;
	return total;
#endif
}

unsigned int csp_ps_hook(csp_packet_t * packet) {
	(void)packet; /* Avoid compiler warnings about unused parameter */
	return 0;
}

void csp_reboot_hook(void) {
#if defined(__CYGWIN__)
    csp_print("HALTED - Please reboot\n");
    while (true)
        sleep(1);
#else
	sync();
	reboot(CSP_REBOOT_CMD_RESTART);
#endif
}

void csp_shutdown_hook(void) {
#if defined(__CYGWIN__)
    csp_print("HALTED - Please power off\n");
    while (true)
        sleep(1);
#else
	sync();
	reboot(CSP_REBOOT_CMD_HALT);
#endif
}
