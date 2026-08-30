# VitalNode — Renode CI (Stage 2 gate)

Every PR builds the firmware and runs it in **Renode**, then a Robot test asserts the
expected USART3 output. If the asserts fail the job exits non-zero, so this can be a
**required status check** — the objective pass/fail milestone gate that replaces Arm AVH.

**This was validated end-to-end** (firmware built with `arm-none-eabi-gcc`, run on the
Renode STM32F767 model, USART3 asserts passing) before being handed over.

## Layout

```
firmware/          minimal freestanding blink (builds with gcc alone — no CubeMX/HAL)
  blink.c          toggles PB0, prints "LED ON"/"LED OFF" on USART3
  linker.ld        flash @0x08000000, RAM @0x20000000
  Makefile         arm-none-eabi-gcc → blink.elf
platforms/
  nucleo_f767_ci.repl   minimal, version-proof STM32F767 model (cpu, usart3, gpiob, LED)
tests/
  vitanode-smoke.robot  boots the ELF, asserts USART3 telemetry
.github/workflows/
  renode-ci.yml    install toolchain → build → install Renode → run test
```

## Run it locally (same as CI)

```bash
make -C firmware
wget -q https://github.com/renode/renode/releases/download/v1.16.1/renode-1.16.1.linux-portable-dotnet.tar.gz
mkdir -p renode && tar xzf renode-*.tar.gz -C renode --strip-components=1
pip install -r renode/tests/requirements.txt
./renode/renode-test tests/vitanode-smoke.robot
```

Expected tail: `Tests finished successfully :)`

## Put it on GitHub

```bash
git switch -c renode-ci
git add firmware platforms tests .github
git commit -m "Add Renode CI smoke test (Stage 2 gate)"
git push -u origin renode-ci
```

Open a PR. The **Renode CI (Stage 2 gate)** check runs automatically. To enforce it:
Settings → Branches → add a branch protection rule for `main` → *Require status checks to
pass before merging* → select **renode-smoke**. Now no PR merges unless Renode is green.

## Design notes

- **Stable Renode 1.16.1** is enough here because the platform file is vendored and
  minimal — no nightly needed for CI. (For local *development* with the full board model
  — named LEDs, Ethernet, button — use a nightly and `@platforms/boards/nucleo_f767zi.repl`.)
- **The firmware is a stand-in.** It's a self-contained blink so CI has something real to
  build with no CubeMX. To gate on your *actual* subsystem, either commit its sources with
  a Makefile/CMake the `Build firmware` step invokes, or publish the `.elf` as an artifact
  and load that in the Robot test.
- **Extending to integration tests:** add Robot cases that drive the `VN_SensorReading`
  stub (Renode RESD sensor injection) and assert the alert thresholds
  (temp_c 38.5 HIGH @ call 300, spo2 88.0 CRITICAL @ call 500).
