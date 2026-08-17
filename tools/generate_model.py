#!/usr/bin/env python3
from pathlib import Path
import random
import struct

MAGIC = b"EIR1"
INPUT, HIDDEN, OUTPUT = 4, 8, 3
random.seed(42)

def vals(count, scale):
    return [random.uniform(-scale, scale) for _ in range(count)]

weights = [
    *vals(INPUT * HIDDEN, 0.35),
    *vals(HIDDEN, 0.10),
    *vals(HIDDEN * OUTPUT, 0.35),
    *vals(OUTPUT, 0.10),
]

out = Path(__file__).resolve().parents[1] / "models" / "tinynet.eir"
out.parent.mkdir(parents=True, exist_ok=True)

with out.open("wb") as f:
    f.write(MAGIC)
    f.write(struct.pack("<III", INPUT, HIDDEN, OUTPUT))
    for value in weights:
        f.write(struct.pack("<f", value))

print(f"Wrote {out}")
