"""Local integration test: TCP segmentation/coalescing does not split telemetry records."""
from pathlib import Path
import socket
import subprocess
import time

ROOT = Path(__file__).resolve().parents[1]
LOG = ROOT / "logs/server.log"
old_log = LOG.read_bytes() if LOG.exists() else None
with socket.socket() as probe:
    probe.bind(("127.0.0.1", 0))
    port = probe.getsockname()[1]
server = subprocess.Popen([str(ROOT / "server"), str(port)], cwd=ROOT,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
try:
    with socket.socket() as client:
        for attempt in range(40):
            try:
                client.connect(("127.0.0.1", port))
                break
            except ConnectionRefusedError:
                time.sleep(0.05)
        else:
            raise AssertionError("Server did not begin listening on loopback")
        message1 = b"device_id=BAT_001;timestamp=1700000001;temp=24.90;humidity=59.60;battery=99;status=OK\n"
        message2 = b"device_id=BAT_002;timestamp=1700000002;temp=25.10;humidity=60.00;battery=98;status=OK\n"
        # A single message is split across three writes.
        client.sendall(message1[:7]); client.sendall(message1[7:36]); client.sendall(message1[36:])
        # A second message follows without any extra spacing or socket reconnect.
        client.sendall(message2)
    server.wait(timeout=5)
    assert server.returncode == 0, server.stdout.read().decode(errors="replace")
    contents = LOG.read_text()
    assert "1700000001,BAT_001,24.90,59.60,99,OK" in contents
    assert "1700000002,BAT_002,25.10,60.00,98,OK" in contents
    print("PASS: TCP framing handles split and consecutive messages")
finally:
    if server.poll() is None:
        server.kill()
        server.wait(timeout=3)
    if old_log is None:
        if LOG.exists(): LOG.unlink()
    else:
        LOG.write_bytes(old_log)
