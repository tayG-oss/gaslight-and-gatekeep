#!/usr/bin/env python3
"""
Gates of Babylon - bridge between the web page and the compiled C++ program.

  * serves index.html (from the same folder as this file)
  * POST /api/run  : feeds a circuit to YOUR compiled C++ program exactly like a person
                     typing at the keyboard, and returns what the program printed.

Usage (from the folder that holds index.html and server.py):
    python3 server.py                       # uses ./a.out on port 8000
    python3 server.py --binary ~/proj/a.out --port 8765

Only the Python standard library is used. It listens on 127.0.0.1 ONLY (not on the network);
reach it from your PC with a PuTTY tunnel, or from a browser on the same machine.
"""
import argparse
import json
import os
import subprocess
import sys
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

HERE = os.path.dirname(os.path.abspath(__file__))
CODES = {"NOT": 0, "AND": 1, "OR": 2, "NAND": 3, "NOR": 4, "XOR": 5}   # menu numbers in main.cc
ARITY = {"NOT": 1, "AND": 2, "OR": 2, "NAND": 2, "NOR": 2, "XOR": 2}
MAX_BODY = 100000
MAX_INPUTS = 10
MAX_GATES = 100
BINARY = None   # set in main()


def is_int(v):
    return isinstance(v, int) and not isinstance(v, bool)


def build_tokens(req):
    """Turn the web page's circuit into the exact list of numbers the C++ program reads.
    Raises ValueError for anything malformed (the C++ program itself still judges whether
    the circuit is legal and prints BAD INPUT! if not)."""
    if not isinstance(req, dict):
        raise ValueError("request must be a JSON object")
    n, gates, choice = req.get("n"), req.get("gates"), req.get("choice")
    if not is_int(n) or not 1 <= n <= MAX_INPUTS:
        raise ValueError("n must be an integer from 1 to %d" % MAX_INPUTS)
    if choice not in (1, 2) or not is_int(choice):
        raise ValueError("choice must be 1 (circuit block) or 2 (truth table)")
    if not isinstance(gates, list) or not 1 <= len(gates) <= MAX_GATES:
        raise ValueError("gates must be a list of 1 to %d gates" % MAX_GATES)
    tokens = [n]
    for g in gates:
        if not isinstance(g, dict):
            raise ValueError("each gate must be an object")
        t, ins = g.get("type"), g.get("ins")
        if not isinstance(t, str) or t not in CODES:
            raise ValueError("unknown gate type")
        if not isinstance(ins, list) or len(ins) != ARITY[t] or not all(is_int(i) and 0 <= i <= 100000 for i in ins):
            raise ValueError("bad inputs for a %s gate" % t)
        tokens.append(CODES[t])
        tokens.extend(ins)
    tokens.append(6)        # DONE
    tokens.append(choice)   # 1) print circuit block  /  2) print truth table
    return tokens


def run_program(tokens, timeout=10):
    text = "\n".join(str(t) for t in tokens) + "\n"
    try:
        p = subprocess.run([BINARY], input=text, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        return {"ok": False, "error": "The C++ program took too long and was stopped."}
    except OSError as e:
        return {"ok": False, "error": "Cannot run the C++ program (%s). Is it compiled and executable?" % e}
    out = p.stdout
    if p.returncode != 0 or "BAD INPUT!" in out:
        why = "crashed (signal %d)" % -p.returncode if p.returncode < 0 else "exited with code %d" % p.returncode
        msg = "The C++ program printed BAD INPUT!" if "BAD INPUT!" in out else "The C++ program " + why
        if p.stderr.strip():
            msg += " - " + p.stderr.strip()[:200]
        return {"ok": False, "error": msg}
    return {"ok": True, "stdout": out}


class Handler(BaseHTTPRequestHandler):
    server_version = "GatesBridge/1.0"

    def _send(self, code, body, ctype):
        data = body if isinstance(body, bytes) else body.encode("utf-8")
        self.send_response(code)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(data)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(data)

    def _json(self, code, obj):
        self._send(code, json.dumps(obj), "application/json; charset=utf-8")

    def do_GET(self):
        path = self.path.split("?", 1)[0]
        if path in ("/", "/index.html"):
            try:
                with open(os.path.join(HERE, "index.html"), "rb") as f:
                    self._send(200, f.read(), "text/html; charset=utf-8")
            except OSError:
                self._send(404, "index.html not found next to server.py", "text/plain; charset=utf-8")
        elif path == "/api/health":
            self._json(200, {"ok": True, "binary": BINARY, "binary_exists": os.path.isfile(BINARY)})
        else:
            self._send(404, "not found", "text/plain; charset=utf-8")

    def do_POST(self):
        if self.path.split("?", 1)[0] != "/api/run":
            self._send(404, "not found", "text/plain; charset=utf-8")
            return
        try:
            length = int(self.headers.get("Content-Length", ""))
        except ValueError:
            self._json(400, {"ok": False, "error": "Content-Length required"})
            return
        if length < 0 or length > MAX_BODY:
            self._json(413, {"ok": False, "error": "request too large"})
            return
        try:
            req = json.loads(self.rfile.read(length).decode("utf-8"))
            tokens = build_tokens(req)
        except (ValueError, UnicodeDecodeError) as e:
            self._json(400, {"ok": False, "error": "bad request: %s" % e})
            return
        self._json(200, run_program(tokens))


def smoke_test():
    """One-gate circuit (NOT) so we can warn early if a.out is missing or is the old, crashing build."""
    if not os.path.isfile(BINARY):
        print("WARNING: %s does not exist. Compile your program first (see instructions)." % BINARY)
        return
    r = run_program([1, 0, 0, 6, 2])
    lines = [l.strip() for l in r.get("stdout", "").split("\n") if l.strip()]
    if r["ok"] and lines[-2:] == ["1|0", "0|1"]:
        print("C++ program check: OK (%s)" % BINARY)
    else:
        print("WARNING: the C++ program did not behave as expected on a 1-gate test circuit.")
        print("         Did you recompile after replacing main.cc, gatekeep.h and circuit.h?")
        print("         Details: %s" % (r.get("error") or "unexpected output"))


def main():
    global BINARY
    ap = argparse.ArgumentParser(description="Gates of Babylon web bridge")
    ap.add_argument("--binary", default="a.out", help="path to the compiled C++ program (default ./a.out)")
    ap.add_argument("--port", type=int, default=8000, help="port to listen on (default 8000)")
    args = ap.parse_args()
    BINARY = os.path.abspath(os.path.expanduser(args.binary))
    if not 1 <= args.port <= 65535:
        sys.exit("--port must be between 1 and 65535")
    smoke_test()
    try:
        httpd = ThreadingHTTPServer(("127.0.0.1", args.port), Handler)
    except OSError as e:
        sys.exit("Could not listen on port %d (%s). Try another one, e.g.  python3 server.py --port 8765" % (args.port, e))
    print("Open  http://localhost:%d  (through your PuTTY tunnel).  Press Ctrl+C to stop." % args.port)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nStopped.")


if __name__ == "__main__":
    main()
