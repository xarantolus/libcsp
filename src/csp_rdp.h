#pragma once

#include <csp/csp_types.h>

/* Bounds for the connection parameters proposed by a peer. They limit how long
   a connection the peer never completes can occupy a slot, and how fast it may
   retransmit while it does. */
#ifndef CSP_RDP_MIN_CONN_TIMEOUT
#define CSP_RDP_MIN_CONN_TIMEOUT 1000
#endif
#ifndef CSP_RDP_MAX_CONN_TIMEOUT
#define CSP_RDP_MAX_CONN_TIMEOUT 60000
#endif
#ifndef CSP_RDP_MIN_PACKET_TIMEOUT
#define CSP_RDP_MIN_PACKET_TIMEOUT 100
#endif
#ifndef CSP_RDP_MAX_PACKET_TIMEOUT
#define CSP_RDP_MAX_PACKET_TIMEOUT 60000
#endif
#ifndef CSP_RDP_MIN_ACK_TIMEOUT
#define CSP_RDP_MIN_ACK_TIMEOUT 10
#endif

bool csp_rdp_new_packet(csp_conn_t * conn, csp_packet_t * packet);

void csp_rdp_init(csp_conn_t * conn);
void csp_rdp_check_timeouts(csp_conn_t * conn);
int csp_rdp_connect(csp_conn_t * conn);
int csp_rdp_close(csp_conn_t * conn, uint8_t closed_by);
int csp_rdp_send(csp_conn_t * conn, csp_packet_t * packet);
int csp_rdp_check_ack(csp_conn_t * conn);
bool csp_rdp_conn_is_active(csp_conn_t *conn);
