csp_posix_helper = static_library('csp_posix_helper',
								  'csp_posix_helper.c',
								  include_directories : csp_inc,
								  build_by_default : false
								 ).extract_all_objects(recursive : true)

executable('csp_server_client',
	'csp_server_client.c',
	include_directories : csp_inc,
	c_args : csp_c_args,
	dependencies : csp_dep,
	objects : csp_posix_helper,
	build_by_default : false)

executable('csp_server',
	'csp_server.c',
	include_directories : csp_inc,
	c_args : csp_c_args,
	dependencies : csp_dep,
	objects : csp_posix_helper,
	build_by_default : false)

executable('csp_client',
	'csp_client.c',
	include_directories : csp_inc,
	c_args : csp_c_args,
	dependencies : csp_dep,
	objects : csp_posix_helper,
	build_by_default : false)

if host_machine.system() == 'linux'
	# Needs the socketcan driver, which is Linux-only
	executable('csp_bridge_can2udp',
		['csp_bridge_can2udp.c'],
		include_directories : csp_inc,
		c_args : csp_c_args,
		dependencies : csp_dep,
		build_by_default : false)
endif

executable('csp_arch',
	'csp_arch.c',
	include_directories : csp_inc,
	c_args : csp_c_args,
	dependencies :  csp_dep,
	build_by_default : false)

executable('zmqproxy',
	'zmqproxy.c',
	include_directories : csp_inc,
	c_args : csp_c_args,
	dependencies : csp_dep,
	build_by_default : false)

if host_machine.system() == 'linux'
	# Uses pthread_timedjoin_np, a glibc extension macOS doesn't have
	executable('csp_sfp_server_client',
		'csp_sfp_server_client.c',
		include_directories : csp_inc,
		c_args : csp_c_args,
		dependencies : csp_dep,
		build_by_default : false)
endif
