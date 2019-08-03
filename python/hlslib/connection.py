"""Open the UDP socket"""
import enum
import logging
import socket
import struct

BUFFER_SIZE = 64 * 1024 * 1024  # 64MB Kernel send buffer size

LOGGER = logging.getLogger('hlslib.connection')


class FrameType(enum.IntEnum):
    """Frame packet types"""
    tile = 1
    swap = 2


class Packet:  # pylint: disable=R0903
    """Encapsulate the raw packet into something nice"""
    type = FrameType.tile
    frameno = 0
    position = (0, 0)
    _size = 0  # apparently unused
    payload = b''

    def __repr__(self):
        return '<Packet(type={type}, frameno={frameno}, position={pos}, payload_len={plen})>'.format(
            type=self.type, frameno=self.frameno, pos=self.position,
            plen=len(self.payload)
        )

    def pack(self):
        """Pack to binary"""
        # Make sure frameno is within byte
        if self.frameno < 0:
            self.frameno = 0
        frameno = self.frameno % 255
        LOGGER.debug('Packing {} (packet frameno={})'.format(self, frameno))
        header = struct.pack('<BBHH2s',
                             self.type,
                             frameno,
                             self.position[0],
                             self.position[1],
                             b''
                             )
        LOGGER.debug('raw header: {}'.format(repr(header)))
        return header + self.payload


class Connection:
    """Keep track of the socket"""
    _sock = None
    swap_addr = None

    def __init__(self, swap_ip, swap_port=9998):
        self.swap_addr = (swap_ip, swap_port)
        self.open()

    def close(self):
        """Close the socket"""
        if self._sock is None:
            return
        self._sock.close()
        self._sock = None

    def open(self):
        """Open the socket for use"""
        self._sock = socket.socket(socket.AF_INET,
                                   socket.SOCK_DGRAM,
                                   socket.IPPROTO_UDP)
        self._sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        try:
            self._sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, BUFFER_SIZE)
        except OSError:
            pass

    def send_tile(self, tile, frameno):
        """send a given tile to the panels UDP socket"""
        packet = Packet()
        packet.type = FrameType.tile
        packet.frameno = frameno
        packet.position = tile.panel_position
        packet.payload = tile.pixeldata.tobytes()
        try:
            packed = packet.pack()
            self._sock.sendto(packed, tile.panel.addr)
        except OSError:
            # TODO: log ?
            pass

    def send_swap(self, frameno):
        """Broadcast the swap frame command"""
        packet = Packet()
        packet.frameno = frameno
        packet.type = FrameType.swap
        try:
            self._sock.sendto(packet.pack(), self.swap_addr)
        except OSError:
            # TODO: log ?
            pass
