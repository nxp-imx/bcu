# BCU Power Measurement Probes Documentation

This document describes the power measurement probe placement for bench boards. The probes are installed by removing inductors (L), cutting jumpers (SH), or replacing resistors (R) from the board and connecting the measurement probe in their place.

## General Installation Procedure

1. **For Inductors (Ln)**: Remove the inductor from the board, solder it to the measurement probe, then solder the probe back to the board in place of the inductor.

2. **For Jumpers (SHn)**: Cut the jumper trace, solder a 0Ω resistor to the measurement probe, then connect the probe to the cut jumper pads.

3. **For Resistors (Rn)**: Remove the resistor from the board, solder it to the measurement probe, then solder the probe back to the board in place of the resistor.

---

## bench_imx95frdm15

| Rail Name | Probe Location | Notes |
|-----------|---------------|-------|
| vdd_soc | L6 | Remove inductor L6 |
| vdd_arm | L7 | Remove inductor L7 |
| vdd_3v3 | L1 | Remove inductor L1 |
| vdd_1v8 | L3 | Remove inductor L3 |
| vdd_0v8 | L2 | Remove inductor L2 |
| vcc_dram_1v1 | L5 | Remove inductor L5 |
| vcc_dram_0v6 | L4 | Remove inductor L4 |
| vdd_M2 | R506 & R505 | Remove resistors R506 and R505 |
| VCC12A_PCIE_TXRX_ARA2 | L5 | Remove inductor L5 |
| VDD_CORE_U12_ARA2 | L4 | Remove inductor L4 |
| VDD_CORE_U11_ARA2 | L3 | Remove inductor L3 |
| VDDQ_ARA2 | L13 | Remove inductor L13 |
| VDDQ_MEM_1V1_ARA2 | L1 | Remove inductor L1 |
| VDD_1V8_ARA2 | L2 | Remove inductor L2 |

---

## bench_imx8qm_revB

| Rail Name | Probe Location | Notes |
|-----------|---------------|-------|
| NVCC_DRAM | SH703 | Cut jumper SH703, use 0Ω resistor on probe |
| VDD_DRAM | SH704 | Cut jumper SH704, use 0Ω resistor on probe |
| VPU | SH702 | Cut jumper SH702, use 0Ω resistor on probe |
| GPU | SH701 | Cut jumper SH701, use 0Ω resistor on probe |
| VDD_ARM_CORE | SH709 | Cut jumper SH709, use 0Ω resistor on probe |
| VDD_ARM_SoC | SH708 | Cut jumper SH708, use 0Ω resistor on probe |
| VDD_PLATFORM | R928 & R929 | Remove resistors R928 and R929 |

---

## bench_imx8qxp_revB

| Rail Name | Probe Location | Notes |
|-----------|---------------|-------|
| VCC_CPU | L7 | Remove inductor L7 |
| VCC_GPU | L4 | Remove inductor L4 |
| VCC_MAIN | L1+L2 | Remove inductors L1 and L2 |
| VCC_DDRIO | L6 | Remove inductor L6 |
| VCC_1V8 | L5 | Remove inductor L5 |
| VCC_3V3 | L3 | Remove inductor L3 |

---

## bench_imx8mq

| Rail Name | Probe Location | Notes |
|-----------|---------------|-------|
| NVCC_DRAM | SH703 | Cut jumper SH703, use 0Ω resistor on probe |
| VDD_DRAM | SH704 | Cut jumper SH704, use 0Ω resistor on probe |
| VPU | SH702 | Cut jumper SH702, use 0Ω resistor on probe |
| GPU | SH701 | Cut jumper SH701, use 0Ω resistor on probe |
| VDD_ARM_CORE | SH709 | Cut jumper SH709, use 0Ω resistor on probe |
| VDD_ARM_SoC | SH708 | Cut jumper SH708, use 0Ω resistor on probe |

---

## bench_imx6ull_revB

| Rail Name | Probe Location | Notes |
|-----------|---------------|-------|
| VDD_SNVS_IN | - | No specific location comment available |
| NVCC_NAND | - | No specific location comment available |
| VDD_ARM_SOC_IN | - | No specific location comment available |
| VDDA_ADC_3P3 | - | No specific location comment available |
| DRAM_1V35 | - | No specific location comment available |
| VDD_HIGH_IN | - | No specific location comment available |
| DCDC_3V3 | - | No specific location comment available |

---

## bench_mcu

| Rail Name | Probe Location | Notes |
|-----------|---------------|-------|
| VDD_DCDC | - | No specific location comment available |
| VDD_CORE | - | No specific location comment available |
| VDD_LDO_SYS | - | No specific location comment available |
| VDD_MCU | - | No specific location comment available |
| VDD_P2 | - | No specific location comment available |
| VDD_P3 | - | No specific location comment available |
| VDD_P4 | - | No specific location comment available |
| VDD_USB | - | No specific location comment available |
| VDDA | - | No specific location comment available |
| VDD_BAT | - | No specific location comment available |

---

## Notes

- All measurements use PAC1934 current/power monitors
- Rsense values are specified in the board configuration (in mΩ)
- SR_ prefixed signals are power rail status/enable signals
- Ensure proper probe calibration before measurements
- Always verify board schematic revision matches the probe placement