#!/usr/bin/env python3

import socket
import sys
import time

from enum import IntEnum
from typing import NoReturn


# Message format
#
# Each message consists of:
#   - Header:
#       * 1 byte  : Message ID
#       * 4 bytes : Payload size (unsigned 32-bit integer, network byte order)
#   - Payload:
#       * N bytes : Message data (optional, length = payload size)
HEADER_SIZE = 5
MAX_PAYLOAD_SIZE = 100  # Must match Server's CHUNK_LEN


class MSG_ID(IntEnum):
    # Must match Server enum
    MSG_KEY = 1
    MSG_DATA = 2
    MSG_ERROR = 3
    MSG_END = 4
    MSG_CLOSE = 5
    MSG_TERMINATE = 6


def handle_exit(
    sock: socket.socket, exit_code: int = 0, message: str | None = None
) -> NoReturn:
    """
    Shutdown socket, close resources, and terminate the program.

    :param sock: Client socket
    :type sock: socket.socket
    :param exit_code: Process exit code
    :type exit_code: int
    :param message: Optional exit message
    :type message: str | None
    :return: This function will never return
    :rtype: NoReturn
    """
    if message:
        print(message)

    try:
        sock.shutdown(socket.SHUT_RDWR)
    except OSError:
        pass  # Fail silently
    try:
        sock.close()
    except OSError:
        pass  # Fail silently

    print("\n\n\t\tGOODBYE!!")
    sys.exit(exit_code)


def handle_key_msg(payload: bytes) -> bytes:
    """
    Parse and return the encryption key.

    :param payload: Raw KEY from Server
    :type payload: bytes
    :return: KEY in bytes
    :rtype: bytes
    """
    # TODO: Print a message that you're parsing the key.
    # TODO: Print the key (handle UTF-8 or error).
    key = payload

    # TODO: Return the key bytes to be used for decryption.
    return key


def handle_data_msg(payload: bytes, key: bytes) -> None:
    """
    Decrypt payload with key and append decrypted data the to output file.

    :param payload: Raw encrypted DATA from Server
    :type payload: bytes
    :param key: Encryption key used for XOR decryption
    :type key: bytes
    :return: None
    :rtype: None
    """
    # TODO: Validate key is not empty (if it is, raise or handle error).
    # TODO: Decrypt using simple XOR
    # TODO: Append the decrypted data to FILENAME in binary mode ("ab")


def safe_sendall(sock: socket.socket, msg: bytes) -> None:
    """
    Wrapper to safely send msg using sendall().

    If a send error occurs, the socket is closed and the program exits.

    :param sock: Client socket
    :type sock: socket.socket
    :param msg: Raw encrypted message to send to Server
    :type msg: bytes
    :return: None
    :rtype: None
    """
    try:
        sock.sendall(msg)
    except (BrokenPipeError, ConnectionResetError, socket.timeout, OSError) as e:
        handle_exit(sock, exit_code=1, message=f"[-] Send failed: {e} ...")


def send_key_request(sock: socket.socket) -> None:
    """
    Send the KEY request to the Server.

    :param sock: Client socket
    :type sock: socket.socket
    :return: None
    :rtype: None
    """
    key_req_msg = b""  # TODO: Implement KEY request message
    safe_sendall(sock, key_req_msg)


def send_file_request(sock: socket.socket, filename: str) -> None:
    """
    Send a DATA request for a given filename to the Server.

    :param sock: Client socket
    :type sock: socket.socket
    :param filename: Filename to request from Server
    :type filename: str
    """
    file_bytes = filename.encode("utf-8")

    if len(file_bytes) == 0:
        raise ValueError("[-] Filename must not be empty ...")

    if len(file_bytes) > MAX_PAYLOAD_SIZE:
        raise ValueError(
            f"[-] Filename too long: {len(file_bytes)} bytes (max {MAX_PAYLOAD_SIZE}) ..."
        )

    msg_hdr = b""  # TODO: Implement DATA request message
    file_req_msg = b""  # TODO: Implement full message
    safe_sendall(sock, file_req_msg)


def send_close(sock: socket.socket):
    """
    Send CLOSE request to the Server.

    :param sock: Client socket
    :type sock: socket.socket
    :return: None
    :rtype: None
    """
    close_msg = b""  # TODO: Implement CLOSE request message
    safe_sendall(sock, close_msg)


def main():
    # TODO: Read the list of target files
    # TODO: Verify we actually populated files

    # Reset the output file, if already present
    try:
        open(FILENAME, "wb").close()
    except OSError as e:
        print(f"[-] Failed to prepare output file {FILENAME}: {e} ...")
        sys.exit(1)

    # TODO: Create a TCP socket and connect to the remote Server
    # TODO: Set socket timeout so idle timeout logic works
    # TODO: Request the encryption KEY from the Server

    # Set idle timeout
    last_activity = time.time()

    buf = b""
    key: bytes | None = None
    file_index = 0

    # TODO: IMPLEMENT MAIN RECEIVE LOOP
    while True:
        #
        # Suggested outline:
        #
        # Try to receive data from the Server and handle errors
        #
        # Process data (i.e., message(s))
        #
        # Map msg_id to ENUM and validate
        #
        # Handle message
        #

        # Reset timeout after successfully processing a message
        last_activity = time.time()


if __name__ == "__main__":
    main()
