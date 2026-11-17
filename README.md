# IoT Telemetry Simulator | C / TCP / Linux + Windows

A small C systems-programming project with a simulated IoT device and a TCP server. It creates environmental telemetry, encodes a readable wire format, sends complete newline-delimited messages, validates received telemetry and writes a structured CSV-like log.

**Implementation:** C11, TCP sockets, WinSock2 on Windows and POSIX sockets on Linux. This is a networked *software simulation*, not physical IoT firmware or a hardware sensor driver.

## Modules

| Component | Purpose |
| --- | --- |
| `sensor.c` | Simulates temperature, humidity, battery and status |
| `config.c` | Loads and validates server address, port, interval and device ID |
| `protocol.c` | Encodes/decodes and validates telemetry values |
| `socket_utils.c` | Cross-platform TCP, full-send loop, newline-based message framing |
| `server_main.c` | Receives one client at a time, validates and logs complete messages |
| `logger.c` | Appends accepted telemetry to `logs/server.log` |

## Build

Requires GCC and GNU Make. On **Windows**, open the **MSYS2 UCRT64** terminal (uses WinSock2). On **Linux/WSL**, use the normal shell (POSIX sockets):

```bash
make
make test
```

Run from the project root in two terminals:

```bash
./server       # Linux; Windows/MSYS2: ./server.exe
./device       # Linux; Windows/MSYS2: ./device.exe
```

The device reads `config/device.conf`, connects to `127.0.0.1:8080` by default and transmits once every three seconds. Use Ctrl+C to stop. Run only on your own computer or trusted network.

## Example message

```text
device_id=BAT_001;timestamp=1773257332;temp=24.90;humidity=59.60;battery=100;status=OK
```

The wire protocol ends each message with `\n`. TCP is a byte stream, so `recv()` boundaries are not message boundaries. This revision uses full-send handling and newline framing instead of assuming each receive call contains exactly one telemetry record.

## Engineering details

- Socket handles retain the platform's full pointer-sized representation; the Windows `SOCKET` is not truncated to `int`.
- `send_message` retries short writes; `receive_message` reads complete newline-delimited messages, with a size limit.
- The parser rejects trailing garbage and out-of-range values; configuration loading checks required fields and bounds.
- Compilation outputs and runtime logs are gitignored instead of stored alongside source files.

## Limitations

The server supports one connected client at a time; the network protocol is plaintext and unauthenticated. Line-by-line receiving prioritizes correctness and clarity over throughput; a production implementation would use buffered reads and robust reconnection/error reporting. This project does not model embedded-device power constraints or actual sensors.

## Test

`make test` exercises telemetry parsing, validation and configuration. A manual two-terminal integration run checks live TCP and log output.
