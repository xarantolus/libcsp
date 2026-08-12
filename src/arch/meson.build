if host_machine.system() == 'linux' or host_machine.system() == 'darwin'
	subdir('posix')
	conf.set('CSP_POSIX', 1)
elif host_machine.system() == 'freertos'
	subdir('freertos')
	conf.set('CSP_FREERTOS', 1)
else
	error('invalid system @0@'.format(host_machine.system()))
endif
