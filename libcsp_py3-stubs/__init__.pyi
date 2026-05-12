"""Type stubs for the libcsp_py3 C extension.

Mirrors the method table + module constants in src/bindings/python/pycsp.c.
The C extension returns opaque Python capsules for sockets, connections
and packet buffers; modelled here as distinct ``NewType`` aliases so
type checkers catch argument mixups without pretending the capsule has
inspectable attributes.
"""
from __future__ import annotations

from typing import NewType

# ---------------------------------------------------------------------------
# Opaque capsule handles returned by the binding.
# ---------------------------------------------------------------------------
CspSocket = NewType("CspSocket", object)
CspConn = NewType("CspConn", object)
CspPacket = NewType("CspPacket", object)

# ---------------------------------------------------------------------------
# Module-level exception.
# ---------------------------------------------------------------------------
class Error(Exception): ...

# ---------------------------------------------------------------------------
# Connection option bits (csp/csp_types.h — CSP_SOCKET_OPTIONS).
# ---------------------------------------------------------------------------
CSP_SO_NONE: int
CSP_SO_RDPREQ: int
CSP_SO_RDPPROHIB: int
CSP_SO_HMACREQ: int
CSP_SO_HMACPROHIB: int
CSP_SO_CRC32REQ: int
CSP_SO_CRC32PROHIB: int
CSP_SO_CONN_LESS: int

CSP_O_NONE: int
CSP_O_RDP: int
CSP_O_NORDP: int
CSP_O_HMAC: int
CSP_O_NOHMAC: int
CSP_O_CRC32: int
CSP_O_NOCRC32: int

# Header flags.
CSP_FFRAG: int
CSP_FHMAC: int
CSP_FRDP: int
CSP_FCRC32: int

# Misc.
CSP_ANY: int
CSP_MAX_TIMEOUT: int
CSP_NO_VIA_ADDRESS: int

# Priorities (csp_prio_t).
CSP_PRIO_CRITICAL: int
CSP_PRIO_HIGH: int
CSP_PRIO_NORM: int
CSP_PRIO_LOW: int

# Reserved service ports (csp_service_port_t).
CSP_CMP: int
CSP_PING: int
CSP_PS: int
CSP_MEMFREE: int
CSP_REBOOT: int
CSP_BUF_FREE: int
CSP_UPTIME: int

# Error codes.
CSP_ERR_NONE: int
CSP_ERR_NOMEM: int
CSP_ERR_INVAL: int
CSP_ERR_TIMEDOUT: int
CSP_ERR_USED: int
CSP_ERR_NOTSUP: int
CSP_ERR_BUSY: int
CSP_ERR_ALREADY: int
CSP_ERR_RESET: int
CSP_ERR_NOBUFS: int
CSP_ERR_TX: int
CSP_ERR_DRIVER: int
CSP_ERR_AGAIN: int
CSP_ERR_NOSYS: int
CSP_ERR_HMAC: int
CSP_ERR_CRC32: int
CSP_ERR_SFP: int

# ---------------------------------------------------------------------------
# Lifecycle.
# ---------------------------------------------------------------------------
def init(
    hostname: str,
    model: str,
    revision: str,
    version: int = ...,
    conn_dfl_so: int = ...,
    dedup: int = ...,
) -> None: ...
def get_hostname() -> str: ...
def get_model() -> str: ...
def get_revision() -> str: ...

# ---------------------------------------------------------------------------
# Sockets / connections.
# ---------------------------------------------------------------------------
def socket(opts: int = ...) -> CspSocket: ...
def bind(socket: CspSocket, port: int) -> None: ...
def listen(socket: CspSocket, backlog: int) -> None: ...
def accept(socket: CspSocket, timeout: int = ...) -> CspConn | None: ...
def connect(
    prio: int, dest: int, dport: int, timeout: int, opts: int
) -> CspConn: ...
def close(conn: CspConn) -> None: ...
def conn_dport(conn: CspConn) -> int: ...
def conn_sport(conn: CspConn) -> int: ...
def conn_dst(conn: CspConn) -> int: ...
def conn_src(conn: CspConn) -> int: ...

# ---------------------------------------------------------------------------
# Packets.
# ---------------------------------------------------------------------------
def buffer_get(size: int = ...) -> CspPacket: ...
def buffer_free(packet: CspPacket) -> None: ...
def buffer_remaining() -> int: ...
def packet_get_data(packet: CspPacket) -> bytes: ...
def packet_get_length(packet: CspPacket) -> int: ...
def packet_get_id(packet: CspPacket) -> int:
    """On-wire 32-bit CSP1 identifier packed from `csp_packet_t::id`:
    pri[2] | src[5] | dst[5] | dport[6] | sport[6] | flags[8]. Promisc
    observers parse it back into fields in Python."""
    ...
def packet_set_data(packet: CspPacket, data: bytes) -> None: ...

# ---------------------------------------------------------------------------
# I/O.
# ---------------------------------------------------------------------------
def read(conn: CspConn, timeout: int = ...) -> CspPacket | None: ...
def send(conn: CspConn, packet: CspPacket, timeout: int = ...) -> None: ...
def sendto(
    prio: int,
    dest: int,
    dport: int,
    src_port: int,
    opts: int,
    packet: CspPacket,
) -> None: ...
def sendto_reply(
    request: CspPacket, reply: CspPacket, opts: int = ...
) -> None: ...
def recvfrom(socket: CspSocket, timeout: int = ...) -> CspPacket | None: ...
def transaction(
    prio: int,
    dest: int,
    port: int,
    timeout: int,
    outbuf: bytes,
    inbuf: bytearray,
    allow_any_incoming_length: int = ...,
) -> int: ...
def service_handler(packet: CspPacket) -> None: ...

# ---------------------------------------------------------------------------
# Router.
# ---------------------------------------------------------------------------
def route_start_task() -> None: ...

# ---------------------------------------------------------------------------
# CSP Management Protocol (CMP) helpers.
# ---------------------------------------------------------------------------
def ping(node: int, timeout: int, size: int, opts: int) -> int: ...
def reboot(node: int) -> None: ...
def shutdown(node: int) -> None: ...
def cmp_ident(node: int, timeout: int) -> tuple[str, str, str, str, str]: ...
def cmp_route_set(
    node: int, timeout: int, addr: int, via: int, ifstr: str
) -> None: ...
def cmp_peek(node: int, timeout: int, addr: int, length: int) -> bytes: ...
def cmp_poke(node: int, timeout: int, addr: int, data: bytes) -> None: ...
def cmp_clock_set(node: int, timeout: int, sec: int, nsec: int) -> None: ...
def cmp_clock_get(node: int, timeout: int) -> tuple[int, int]: ...

# ---------------------------------------------------------------------------
# RDP options.
# ---------------------------------------------------------------------------
def rdp_set_opt(
    window_size: int,
    conn_timeout_ms: int,
    packet_timeout_ms: int,
    delayed_acks: int,
    ack_timeout: int,
    ack_delay_count: int,
) -> None: ...
def rdp_get_opt() -> tuple[int, int, int, int, int, int]: ...

# ---------------------------------------------------------------------------
# Routing table (requires CSP_USE_RTABLE=ON at build time).
# ---------------------------------------------------------------------------
def rtable_set(addr: int, netmask: int, ifstr: str, via: int = ...) -> None: ...
def rtable_clear() -> None: ...
def rtable_check(addr: int) -> int: ...
def rtable_load(rtable: str) -> int: ...
def print_routes() -> None: ...

# ---------------------------------------------------------------------------
# Interfaces.
# ---------------------------------------------------------------------------
def can_socketcan_init(
    ifc: str, addr: int = ..., bitrate: int = ..., promisc: int = ...
) -> None: ...
def kiss_init(
    device: str,
    addr: int,
    baudrate: int = ...,
    mtu: int = ...,
    if_name: str = ...,
) -> None: ...

def print_connections() -> None: ...
def print_interfaces() -> None: ...
