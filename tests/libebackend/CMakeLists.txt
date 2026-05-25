set(extra_deps
	ebackend
)

set(extra_defines)

set(extra_cflags)

set(extra_incdirs)

set(extra_ldflags)

set(TESTS
	e-backend-plugin-manager-test
)

foreach(_test ${TESTS})
	set(SOURCES ${_test}.c)

	add_installable_test(${_test}
		SOURCES
		extra_deps
		extra_defines
		extra_cflags
		extra_incdirs
		extra_ldflags
		"session-exclusive"
		"TEST_INSTALLED_SERVICES=1"
	)
endforeach(_test)
