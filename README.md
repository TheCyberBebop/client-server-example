# Programming Asssessment -- Client–Server Application (C Server + Python3 Client)

## Purpose

Evaluates ability to read, understand, and extend an existing **Client/Server application** and to test knowledge on the C and Python3 programming languages.

Complete:

- A **C-based TCP server**
- A **Python3 TCP client**

You must implement missing logic exactly where the source files indicate (`TODO` or commented sections).  
This project tests skills in:

- C memory allocation & deallocation
- Queue and linked-list data structures
- Pointer and buffer manipulation
- Binary network protocols
- Python socket programming
- Structured message parsing
- File reading and writing
- Error handling
- Simple XOR encryption

Expected to produce well-structured, correct, safe code.

## Included Files

| File | Description |
|------|-------------|
| `target_files.txt` | Test file containing a filename on each line |
| `server.c` | Server C code to complete |
| `client.py`| Client Python3 code to complete |
| `Makefile` | C make targets |
| `README.md` | This file |

## Message Protocol Specification

Each message has this binary structure:

```shell
    1 byte -> message ID
    4 bytes -> payload size (uint32, big-endian)
    N bytes -> payload data (optional)
```

### Message IDs  
(Must match both the C `MSG` enum and Python `MSG_ID` enum.)

| ID | Meaning |
|----|---------|
| `MSG_KEY` (1) | Server sends XOR key to client |
| `MSG_DATA` (2) | Client -> Server: requested filename (UTF-8). Server -> Client: encrypted file bytes (up to 100 bytes per payload) |
| `MSG_ERROR` (3) | Server could not open/read file |
| `MSG_END` (4) | End of KEY or data transmission |
| `MSG_CLOSE` (5) | Client is done and requests connection close |
| `MSG_TERMINATE` (6) | Fatal server error — both sides must exit immediately |

**NOTE:** Invalid IDs must be treated as fatal errors.

## Requirements

### Python Client Requirements

Must implement correct incremental buffered parsing:
- Partial header reads
- Partial payload reads

Enforce:
- Payload size ≤ 100 (This is for the Server payload size max, Client recv buffer can be larger!)
- Valid message IDs

Must correctly implement:
- `main()`
- `handle_key_msg()`
- `handle_data_msg()`
- `send_key_request()`
- `send_file_request()`
- `send_close()`
- Decryption using XOR
- Timeout-based receive loop (`sock.settimeout()`)
- Must write decrypted output to: `{HOST}_{PORT}_results.txt`

### C Server Requirements

Must correctly implement:
- `genKey()`
- `createQueue()`
- `newNode()`
- `enQueue()`
- `deQueue()`
- `emptyQueue()`
- `sendKey()`
- `openFile()` with length-safe filename handling  
- `encryptData()`
- `readData()`
- `sendAll()`
- `sendData()`
- `processMsg()`
- `acceptConn()`
- `setupServerSock()`
- Safe queue allocations and cleanup

Must validate:
- Message IDs
- Payload sizes
- Filename lengths
- Filenames in `target_files.txt` are expected to be local filenames; invalid or unsafe names may be rejected by the Server

Must never leak memory:
- Free nodes
- Free payloads
- Free queues
- Free global key in destructor

Must only exit when:
- `MSG_TERMINATE` is triggered
- Debug mode returns from main

### You **must NOT**:
- Change the network protocol or message structure
- Hard-code behavior or bypass expected logic
- Skip required validation or memory cleanup
- Remove any provided message IDs

### You **may**:
- Add helper functions
- Add debugging output (server uses `-DDEBUG` when built in debug mode)
- Improve safety, add error checking, or restructure logic internally
- Extend internal behavior **as long as the protocol stays identical**

### Final Notes

- The **protocol must remain exactly as defined**
- All messages must be sent/parsed according to the binary header rules
- Memory correctness and safety in C **will be evaluated**
- The server must remain running (release mode) except on MSG_TERMINATE
- The client must correctly handle:
  - partial reads
  - invalid IDs
  - timeouts
  - encryption / decryption

## Client/Server Order of Operations

1. The Server program will load and wait, indefinitely, for a connection from the Client program.

2. Upon connection of the Client program, the Server will exchange an encryption KEY of a fixed length of 32-bytes and then wait for a filename from the Client. Upon receiving a valid filename, the Server will open the local file, read and encrypt its contents, and send the contents back to the Client as *N* `MSG_DATA` payloads, each with a payload size up to 100 bytes.

**NOTE:** TCP is a byte stream; the Client’s `recv()` may return partial headers, partial payloads, or multiple payloads at once. The Client must buffer and parse incrementally.

3. Upon receiving `MSG_DATA` payloads, the Client will extract the encrypted file contents, decrypt it, and append the contents to a local file. This will occur for *N* number of files written in the `target_files.txt` file (each file will be on a single line).

4. Upon receiving an `MSG_ERROR` payload from the Server, the Client will skip the current file, move on to the next filename in `target_files.txt`, and continue processing. After all filenames have been processed (successfully or with errors), the Client will send a `MSG_CLOSE` payload and disconnect from the Server.

5. Upon receiving an `MSG_TERMINATE` payload from the Server, the Client must immediately stop processing, close the connection, and exit. No further filenames should be sent, and no additional communication should occur. This message indicates a fatal internal server condition.

**NOTE:** During the encryption KEY exchange phase, receiving an unexpected or invalid message may cause the Server to send `MSG_TERMINATE`. In that case, the Client must exit immediately.

## Example Communications

Encryption KEY exchange will be initiated immediately by the Client upon connection to the Server. The process includes two payloads, 1) the `MSG_KEY` payload itself (32-bytes in length) and 2) an `MSG_END` payload to let the Client know the KEY exchange is complete. If an error occurs, the Server will send an `MSG_TERMINATE` payload. Upon receiving an `MSG_TERMINATE` payload, the Client and Server will exit immediately.

```shell
    Server                        Client
            --------> MSG_KEY
            --------> MSG_END

    Server                        Client
            --------> MSG_KEY
            --------> MSG_TERMINATE
```

After successfully receiving and handling the encryption `MSG_KEY` payload from the Server, the Client will open the `target_files.txt` file and parse its contents (a filename on each line). The Client will send the first filename as a `MSG_DATA` payload to the Server and wait for an *N* number of `MSG_DATA` payloads. Upon receiving `MSG_DATA` payloads the Client will decrypt the data contents (i.e., the Server file's contents), open a local file, and write the decrypted contents to the local file. Finally, once the entire file has been processed, the Server will send an `MSG_END` payload to notify the Client the contents of Server file are complete. The Client should advance to the next filename and repeat the process until `target_files.txt` has been fully parsed.

**NOTE:** The Server should **NEVER** exit under this scenario.
**NOTE:** Communication from the Client, i.e., filename, is **NOT** encrypted.

```shell
    Server                        Client
            <-------- MSG_DATA (filename)
            --------> MSG_DATA (file content)
                . . .
                . . .
            --------> MSG_DATA
            --------> MSG_END
            <-------- MSG_DATA (filename)
            --------> MSG_DATA (file content)
                . . .
                . . .
            --------> MSG_DATA
            --------> MSG_END
```

If an error occurs on the Server, the Server will send an `MSG_ERROR` payload to the Client. The Client will **NEVER** send an `MSG_ERROR` payload to the Server. Upon receiving an `MSG_ERROR` payload, the Client will skip the current file, move on to the next filename in `target_files.txt`, and continue processing. The provided client writes decrypted bytes to a single output file as they arrive; it does not roll back previously written bytes. After all files from the `target_files.txt` file have been processed, the Client will disconnect from the Server and exit.

**NOTE:** The Server should **NEVER** exit under this scenario.

```shell
    Server                        Client
            <-------- MSG_DATA
            --------> MSG_DATA
            --------> MSG_ERROR
```
