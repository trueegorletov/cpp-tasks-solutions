#!/bin/bash

status=true
tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT

dd bs=513 count=1 if=/dev/urandom of="$tmpdir/random.bin" 2>/dev/null
python3 -c 'import sys; import base64; sys.stdout.buffer.write(base64.b85encode(sys.stdin.buffer.read()))' <"$tmpdir/random.bin" >"$tmpdir/random.b85"

# ---------------------------------------------------

./base85 -e <"$tmpdir/random.bin" >"$tmpdir/random.b85.test"
./base85 -d <"$tmpdir/random.b85" >"$tmpdir/random.bin.test"

if cmp -s "$tmpdir/random.b85" "$tmpdir/random.b85.test"; then
  echo Encoder ok!
else
  echo Encoder failed! >&2
  status=false
fi

if cmp -s "$tmpdir/random.bin" "$tmpdir/random.bin.test"; then
  echo Decoder ok!
else
  echo Decoder failed! >&2
  status=false
fi

# ---------------------------------------------------

$status
