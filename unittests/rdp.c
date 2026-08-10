#include <check.h>
#include <string.h>
#include <endian.h>

#include "../include/csp/csp.h"
#include "../include/csp/csp_id.h"
#include "../include/csp/csp_iflist.h"
#include "../include/csp/csp_interface.h"

#include "../src/csp_conn.h"
#include "../src/csp_rdp.h"

#define RDP_SYN 0x08
#define RDP_ACK 0x04

#define TEST_ADDR 10
#define PEER_ADDR 11
#define TEST_PORT 12

typedef struct __attribute__((packed)) {
	uint8_t flags;
	uint16_t seq_nr;
	uint16_t ack_nr;
} test_rdp_header_t;

static unsigned int test_tx_count;

static int test_nexthop(csp_iface_t * iface, uint16_t via, csp_packet_t * packet, int from_me) {
	(void)iface; (void)via; (void)from_me;
	test_tx_count++;
	csp_buffer_free(packet);
	return CSP_ERR_NONE;
}

static csp_iface_t test_if = {
	.addr = TEST_ADDR,
	.netmask = 14,
	.name = "test",
	.nexthop = test_nexthop,
	.is_default = 1,
};

static csp_socket_t test_sock;

static void setup_stack(void) {
	csp_init();
	csp_iflist_add(&test_if);
	memset(&test_sock, 0, sizeof(test_sock));
	csp_bind(&test_sock, TEST_PORT);
	csp_listen(&test_sock, 4);
	test_tx_count = 0;
}

static csp_packet_t * new_rdp_packet(void) {
	csp_packet_t * packet = csp_buffer_get(0);
	ck_assert_ptr_nonnull(packet);

	packet->id.pri = 2;
	packet->id.src = PEER_ADDR;
	packet->id.dst = TEST_ADDR;
	packet->id.dport = TEST_PORT;
	packet->id.sport = 40;
	packet->id.flags = CSP_FRDP;
	packet->length = 0;

	return packet;
}

static void put_header_and_route(csp_packet_t * packet, uint8_t flags, uint16_t seq, uint16_t ack) {
	test_rdp_header_t * header = (test_rdp_header_t *)&packet->data[packet->length];
	header->flags = flags;
	header->seq_nr = htobe16(seq);
	header->ack_nr = htobe16(ack);
	packet->length += sizeof(*header);

	csp_qfifo_write(packet, &test_if, NULL);
	csp_route_work();
}

/* Deliver a SYN carrying `words` option words; 6 is a complete block. */
static void send_syn_words(const uint32_t * opts, unsigned int words) {
	csp_packet_t * packet = new_rdp_packet();

	for (unsigned int i = 0; i < words; i++) {
		packet->data32[i] = htobe32(opts[i]);
	}
	packet->length = words * sizeof(uint32_t);

	put_header_and_route(packet, RDP_SYN, 1000, 0);
}

static void send_syn(const uint32_t opts[6]) {
	send_syn_words(opts, opts ? 6 : 0);
}

static const csp_conn_t * find_rdp_conn(void) {
	size_t count;
	const csp_conn_t * conns = csp_conn_get_array(&count);

	for (size_t i = 0; i < count; i++) {
		if ((conns[i].state == CONN_OPEN) && (conns[i].idin.flags & CSP_FRDP)) {
			return &conns[i];
		}
	}
	return NULL;
}

START_TEST(test_rdp_syn_without_options_is_rejected)
{
	setup_stack();

	/* No option block on the wire: nothing may be adopted from the buffer. */
	send_syn(NULL);

	ck_assert_ptr_null(find_rdp_conn());
	/* The sender is told, rather than left waiting. */
	ck_assert_uint_ge(test_tx_count, 1);
}
END_TEST

START_TEST(test_rdp_syn_with_partial_options_is_rejected)
{
	setup_stack();

	/* One word short of a complete block still walks off the end. */
	const uint32_t opts[5] = { 4, 10000, 1000, 1, 250 };
	send_syn_words(opts, 5);

	ck_assert_ptr_null(find_rdp_conn());
}
END_TEST

Suite * rdp_suite(void)
{
	Suite * s;
	TCase * tc_syn;

	s = suite_create("RDP");

	tc_syn = tcase_create("syn");
	tcase_add_test(tc_syn, test_rdp_syn_without_options_is_rejected);
	tcase_add_test(tc_syn, test_rdp_syn_with_partial_options_is_rejected);
	suite_add_tcase(s, tc_syn);

	return s;
}
