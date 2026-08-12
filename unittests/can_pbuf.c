#include <check.h>
#include <string.h>

#include "../include/csp/csp.h"
#include "../include/csp/csp_buffer.h"
#include "../include/csp/interfaces/csp_if_can.h"

#include "../src/csp_buffer_private.h"

/* A flaky or erroring bus can emit CFP BEGIN frames whose MORE/END frames
 * never arrive. Each such frame starts a reassembly that pins a buffer for
 * PBUF_TIMEOUT_MS (1s), so a burst of them used to drain the pool to zero
 * (csp_can_pbuf_new allocated with the reserve-bypassing
 * csp_buffer_get_always, which panics on a truly empty pool).
 *
 * This floods the interface with BEGIN frames from distinct senders and
 * checks that reassembly stops allocating at the reserve floor, reports
 * CSP_ERR_NOMEM, and leaves the reserved buffers usable. */
START_TEST(test_can_begin_flood_keeps_buffer_reserve)
{
	csp_init();

	csp_can_interface_data_t ifdata;
	memset(&ifdata, 0, sizeof(ifdata));

	csp_iface_t iface;
	memset(&iface, 0, sizeof(iface));
	iface.name = "CAN_FLOOD";
	iface.addr = 1;
	iface.interface_data = &ifdata;

	/* CSP id + length fields of a CFP 2.0 BEGIN frame (content irrelevant,
	 * the packet is never completed) */
	const uint8_t data[8] = {0};

	int accepted = 0;
	int rejected = 0;

	/* Twice the pool size guarantees the flood hits the floor. Each frame
	 * has a unique sender field, so each starts a new reassembly. */
	for (uint32_t sender = 0; sender < 2 * CSP_BUFFER_COUNT; sender++) {
		ck_assert_uint_le(sender, CFP2_SENDER_MASK);

		uint32_t id = ((sender & CFP2_SENDER_MASK) << CFP2_SENDER_OFFSET) |
					  ((uint32_t)(1 & CFP2_BEGIN_MASK) << CFP2_BEGIN_OFFSET);

		int ret = csp_can_rx(&iface, id, data, sizeof(data), NULL);
		if (ret == CSP_ERR_NONE) {
			accepted++;
		} else {
			ck_assert_int_eq(ret, CSP_ERR_NOMEM);
			rejected++;
		}

		/* The pool must never be drained below the reserve */
		ck_assert_int_ge(csp_buffer_remaining(), CSP_BUFFER_RESERVED_COUNT);
	}

	/* Reassembly took every buffer above the reserve, then started failing */
	ck_assert_int_eq(accepted, CSP_BUFFER_COUNT - CSP_BUFFER_RESERVED_COUNT);
	ck_assert_int_eq(rejected, 2 * CSP_BUFFER_COUNT - accepted);
	ck_assert_int_eq(csp_buffer_remaining(), CSP_BUFFER_RESERVED_COUNT);
	ck_assert_uint_eq(iface.rx_error, (unsigned int)rejected);

	/* The reserve is still available to critical allocations: this call
	 * would have panicked on the pre-fix drained pool */
	csp_packet_t * reserved = csp_buffer_get_always();
	ck_assert_ptr_nonnull(reserved);
	csp_buffer_free(reserved);
}
END_TEST

Suite * can_pbuf_suite(void)
{
	Suite *s;
	TCase *tc_exhaustion;

	s = suite_create("CAN Reassembly");

	tc_exhaustion = tcase_create("exhaustion");
	tcase_add_test(tc_exhaustion, test_can_begin_flood_keeps_buffer_reserve);
	suite_add_tcase(s, tc_exhaustion);

	return s;
}
