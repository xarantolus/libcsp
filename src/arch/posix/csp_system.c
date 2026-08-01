#include <csp/csp_hooks.h>

#include <unistd.h>
/* sysinfo(2) and Linux reboot(2) don't exist on macOS. There, free memory is
 * reported via mach host statistics, and reboot/shutdown behave like the
 * Cygwin fallback: halt and wait. Linux is untouched. */
#if defined(__APPLE__)
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <csp/csp_debug.h>
#elif defined(__CYGWIN__)
#include <sys/sysinfo.h>
#include <csp/csp_debug.h>
#else
#include <sys/sysinfo.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
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
#if defined(__CYGWIN__) || defined(__APPLE__)
    csp_print("HALTED - Please reboot\n");
    while (true)
        sleep(1);
#else
	sync();
	reboot(LINUX_REBOOT_CMD_RESTART);
#endif
}

void csp_shutdown_hook(void) {
#if defined(__CYGWIN__) || defined(__APPLE__)
    csp_print("HALTED - Please power off\n");
    while (true)
        sleep(1);
#else
	sync();
	reboot(LINUX_REBOOT_CMD_HALT);
#endif
}
