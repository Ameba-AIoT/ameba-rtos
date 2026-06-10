# Peripheral Examples — Pin Usage Guide

Pin names in the examples (e.g., `_PA_26`) are chip-level. On a module or EVB, not every chip pin is physically accessible or free to use. If an example does not run, confirm the pin in two steps.

---

## Step 1 — The physical pin is brought out and connected

Two checks, in order:

**1. The pin is brought out.** Confirm the chip pin used by the example is routed to an external pad or header.

**2. The pin reaches the target signal.** Confirm the pin actually connects to where the signal lives — a pin header, or a fixed peripheral connection point such as audio signal pad, an SD-card slot, etc.

**Where to check** — Module pin-out diagram, the EVB User Guide and the hardware schematic:

- https://aiot.realmcu.com/filelist?document_type=all

---

## Step 2 — Swap an unavailable or conflicting pin

If the pin is not brought out, or it clashes with another function, swap to another pin that supports the same function. Choose the replacement from the pinmux table.

- Pinmux spec (`ameba_pinmux_rtlxxxx.xlsx`): https://aiot.realmcu.com/filelist?document_type=7
