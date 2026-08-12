socketcan_dep = dependency('libsocketcan', required: false)
if socketcan_dep.found()
	conf.set('CSP_HAVE_LIBSOCKETCAN', 1)
	csp_deps += socketcan_dep
	csp_sources += files(['can/can_socketcan.c'])
else
	conf.set('CSP_HAVE_LIBSOCKETCAN', 0)
endif

if host_machine.system() == 'linux'
	csp_sources += files(['eth/eth_linux.c'])
endif

if host_machine.system() == 'linux' or host_machine.system() == 'darwin'
	# The usart driver is plain POSIX termios, which macOS supports too
	csp_sources += files(['usart/usart_linux.c'])
	csp_sources += files(['usart/usart_kiss.c'])
endif
