# STM32 project conventions

These instructions apply to STM32 projects in this workspace.

## Default project approach

- Use STM32CubeMX, CMake, and Visual Studio Code unless the user explicitly requests another workflow.
- Start by generating a genuine STM32CubeMX base project and retain the `.ioc` file as the hardware-configuration source of truth.
- Use the latest stable, mutually compatible versions available in the existing installation. Verify version-sensitive facts against current primary documentation.
- Document the exact versions used, including the STM32 firmware package, compiler, build tools, and third-party source revision.
- Keep source code, identifiers, technical documentation, and web-interface text in English unless the user explicitly requests another language. Conversation with the user may be in Swedish.

## Continuous project learning

- Maintain this `AGENTS.md` file continuously throughout the project. The user must not need to ask separately for it to be updated after each troubleshooting session.
- After a fault has been diagnosed and its fix has been implemented or confirmed on the target, add a concise reusable rule to the relevant section of this file during the same task.
- Record the underlying cause, the required prevention or verification step, and any important version-, board-, or tool-specific detail. Do not merely record symptoms or copy transient debug output.
- Update existing rules instead of creating duplicates. Keep project instructions general enough to prevent the same class of fault in a future clean project.
- Do not describe an unverified hypothesis as a confirmed solution. If a workaround has only been build-tested, label hardware or browser validation as still pending.

## New-project discovery

At the beginning of a new STM32 network project, inspect the prompt, board, repository, and available local tooling first. Before implementation, ask one concise consolidated question covering only choices that are not already specified. The question must include an optional Mongoose-services checklist so the user can choose the intended scope.

Offer these choices when supported by the selected Mongoose version and target hardware:

- HTTP web interface and REST/JSON API (default for a web-control request)
- MQTT client
- SNTP time synchronization
- DNS and optional mDNS/local hostname discovery
- WebSocket or Server-Sent Events for live updates
- TLS for HTTPS and/or secure MQTT
- Authentication and access control
- Firmware update/OTA support
- Static IPv4 in addition to, or instead of, DHCP
- Additional protocols or custom TCP/UDP services

Do not enable every service automatically. If the user does not answer, continue with the smallest scope explicitly required by the prompt, using DHCP, plain HTTP, and polling for a basic local web-control project. Clearly state that MQTT, SNTP, TLS, authentication, OTA, mDNS, and push updates were not enabled.

Ask follow-up details only for selected features:

- For MQTT: broker hostname/IP, port, client ID, publish and subscribe topics, QoS, retain policy, credentials, TLS requirement, reconnect behavior, and the GPIO/state payload schema. Never commit real credentials; use documented placeholders or a local ignored configuration mechanism.
- For SNTP: server choice or DHCP/default servers, synchronization interval, required accuracy, startup behavior before time is valid, and how UTC/local time should be displayed. Keep the device clock internally in UTC and treat timezone/DST as presentation policy unless explicitly required otherwise.
- For TLS: certificate trust model, hostname verification, device credentials, provisioning/rotation strategy, and whether the MCU memory budget is sufficient. Do not claim HTTPS or secure MQTT until it has been tested on the target.
- For authentication: intended users, session or token model, credential provisioning, logout/expiry, and local-network threat assumptions. Do not present unauthenticated GPIO control as secure merely because it is on a LAN.
- For OTA: image source, signing and verification, rollback/recovery strategy, flash layout, bootloader ownership, and power-loss behavior. Do not add OTA without an explicit safe update design.
- For static addressing or mDNS: address, netmask, gateway, DNS servers, hostname, and required fallback behavior.
- For live browser updates: prefer the simplest mechanism that meets the requirement; define polling interval or WebSocket/SSE reconnect and stale-state behavior.

Summarize the agreed feature set before implementation in a short project configuration block, including board/MCU, network stack, address mode, enabled services, security mode, diagnostic UART, and acceptance tests. Preserve that summary in the project README or equivalent technical notes.

## First-success implementation sequence

- Bring the project up in observable stages: startup and UART, GPIO, Ethernet link, DHCP/static address, minimal machine-readable HTTP endpoint, packed root page and assets, then optional services such as MQTT, SNTP, TLS, WebSocket, authentication, or OTA.
- Keep each stage buildable and avoid introducing optional protocols before the basic network and diagnostic path is verified.
- For a new board, verify pin mappings, PHY address/interface/clocking, VCP UART mapping, LED polarity, and button polarity against the current official board documentation or BSP before generating application code.
- Define success before implementation. For a basic HTTP GPIO project, success means a clean build; correct ELF/map/linker checks; observable UART startup, link, DHCP and listener logs; direct `GET /api/state` and `GET /` responses; successful loading of all browser assets over explicit `http://`; actual independent GPIO control; and correct physical button-state reporting.
- Add acceptance tests for every selected optional service. At minimum, verify MQTT connect/reconnect and publish/subscribe behavior, SNTP valid-time transition and resynchronization, TLS certificate/hostname validation and failure handling, and live-update reconnect behavior when those features are enabled.
- Do not declare the project complete solely from compilation or static inspection. Report separately what was build-verified, host-tested, and confirmed on physical hardware.

## Mongoose defaults

When a project requests Mongoose on an Ethernet-capable STM32 board, unless the user explicitly asks otherwise:

- Build a bare-metal application with no RTOS.
- Use Mongoose's built-in TCP/IP stack and the appropriate built-in STM32 Ethernet driver.
- Do not enable or add lwIP, FreeRTOS, ThreadX, NetXDuo, or another external TCP/IP stack or operating system.
- Follow the current official Mongoose integration guide and, when available, the Mongoose Wizard recipe for the exact evaluation board.
- Keep Mongoose sources and configuration isolated from custom application and platform-integration files.
- Embed web assets in the firmware using Mongoose's supported packed filesystem workflow; do not require a runtime filesystem.
- Use DHCP by default unless the user requests static addressing.
- If the selected MCU provides a hardware random-number generator, enable it in STM32CubeMX and initialize it before Mongoose. Use it as Mongoose's random source through the supported Cube/HAL integration.
- Do not silently replace an available hardware RNG with a software pseudo-random generator. Use a software fallback only when the MCU has no suitable hardware RNG, and document that limitation and its security implications.
- Verify that the hardware RNG implementation is linked and that RNG initialization survives the final CubeMX regeneration.
- Do not enable the Ethernet interrupt in STM32CubeMX when using Mongoose's built-in STM32 Ethernet driver. Mongoose must own the Ethernet IRQ handler; verify that no Cube HAL Ethernet IRQ handler is compiled or linked.
- After CubeMX regeneration, inspect `main.c` rather than trusting the `ProjectManager.functionlistsort` call flag: CubeMX 6.17 may retain `MX_ETH_Init()` even when that flag is false. If the ETH peripheral must remain in the `.ioc` for RMII pin generation, redirect the generated call inside preserved `USER CODE` blocks to a Mongoose-specific clock/GPIO initializer and confirm the final ELF retains only Mongoose's Ethernet IRQ handler.

## Mongoose memory and linker requirements

For STM32 projects using Mongoose's built-in TCP/IP stack:

- Configure at least `0x10000` (64 KiB) heap and `0x2000` (8 KiB) stack in STM32CubeMX unless current official board-specific guidance requires more.
- Do not trust the `.ioc` values alone. After the final CubeMX regeneration, inspect the linker script actually used by CMake and verify that `_Min_Heap_Size` and `_Min_Stack_Size` contain the required values. CubeMX may regenerate a different IDE linker script while CMake continues using an older file.
- Place Mongoose sections matching `.eth_ram` and `.eth_ram*` explicitly in RAM that is accessible by the MCU's Ethernet DMA controller. Preserve the alignment required by the current Mongoose STM32 driver, normally at least 32 bytes on STM32H-class devices.
- Inspect the final map file and confirm that Mongoose RX/TX descriptors and packet buffers are present in `.eth_ram`, located in DMA-accessible RAM, and do not overlap the heap, stack, or other reserved regions.
- If newlib allocation is used, provide a bounded `_sbrk()` implementation. It must reject allocations that would cross the configured stack boundary and return `(void *) -1` with `errno = ENOMEM`; an unchecked bump allocator is not acceptable.
- When linking with newlib-nano, provide `_exit()` as well as the bounded allocation and I/O stubs; `abort()` references `_exit()` and the final link otherwise fails even when application code never exits.
- Add a linker assertion, or an equivalent build-time check, proving that the static sections and minimum heap/stack reservations fit in RAM without overlap.
- Account for Mongoose's default dynamically allocated Ethernet buffers, including the transmit frame buffer and receive queue. A successful link with only CubeMX's small default heap reservation is not sufficient verification.
- After linking, inspect `_end`, the heap limit or stack boundary, `_sstack`, `_estack`, `.data`, `.bss`, and `.eth_ram`. Record the relevant sizes in the verification notes.
- Treat a fault program counter in SRAM as likely memory corruption, stack damage, an invalid function pointer, or execution from data memory. Resolve both the stacked PC and LR against the exact flashed ELF with `arm-none-eabi-addr2line`; never diagnose addresses using an ELF from a different build.

## Serial logging and on-target diagnostics

For STM32 projects with Mongoose or another network stack:

- Provide working serial diagnostics by default. Use the evaluation board's actual ST-LINK Virtual COM Port mapping from the current official board documentation or BSP; do not assume that an arbitrary CubeMX-enabled UART is connected to the host computer.
- Configure the VCP for `115200 8N1` unless the user or board-specific guidance requires another setting, and document the selected UART instance, TX/RX pins, baud rate, and terminal settings.
- Initialize the diagnostic UART before `mg_mgr_init()` so that PHY detection, driver initialization, link state, DHCP, listener failures, and assigned IP-address messages are visible.
- Route Mongoose logging through `mg_log_set_fn()` to the diagnostic UART and set an appropriate development log level, normally `MG_LL_INFO`. Emit an explicit startup banner before initializing the network stack.
- Do not provide a `_write()` implementation that silently discards output. Route `_write()` to the same diagnostic UART, or clearly document and intentionally disable stdio if retargeting is unsuitable.
- Use bounded UART writes with explicit failure behavior. Logging must not allocate memory recursively or depend on the network stack it is diagnosing.
- At minimum, make startup, HTTP-listener success or failure, PHY identification, Ethernet link state, DHCP progress, and the final assigned IP address observable during development.
- After the final clean build, confirm that the UART initialization, UART transmit function, `mg_log_set_fn`, and the selected logging callback are linked into the ELF.
- Clearly separate build verification from serial and network hardware verification. Do not claim that VCP output, DHCP, or Ethernet works until it has been observed on the physical board.

## Mongoose packed web filesystem

When web assets are embedded with Mongoose's packed or in-memory filesystem:

- Generate the packed filesystem source as a reproducible build artifact from the maintained HTML, CSS, JavaScript, image, and other web source files. Document the regeneration command.
- Do not assume that compiling the generated packed-files source automatically activates it. For Mongoose versions using `mg_mem_files`, explicitly assign `mg_mem_files = mg_packed_files` before the HTTP server can serve files, unless the selected official integration uses a different documented activation mechanism.
- Activate the packed filesystem before `mg_mgr_init()` or before accepting HTTP requests, following the API contract of the exact Mongoose version in the project.
- When serving through `mg_fs_packed`, verify that the HTTP serving options reference that filesystem and use a root path compatible with the packed file names.
- With Mongoose 7.21, do not pass the root request `/` through `mg_http_serve_dir()` with both `root_dir = "/"` and packed files named with a leading slash. Its index-path construction can request `//index.html`, which does not match `/index.html` in `mg_fs_packed`. Handle `/` explicitly with `mg_http_serve_file(c, hm, "/index.html", ...)`, or use another tested root/path combination that produces exactly `/index.html`.
- Validate packed-file path joining against the exact Mongoose version in use. Log the requested URI and, when diagnosing an asset failure, confirm the final lookup path does not contain an unintended double slash.
- Emit a startup log confirming that the packed filesystem was activated and print at least one known packed path. Treat a null table or missing `/index.html` as an initialization error.
- Ensure the packed file table is referenced strongly enough that linker garbage collection cannot discard the table or asset arrays. A generated `packed_fs.c` file on disk is not proof that the assets are present in the firmware.
- After the final clean build, inspect the ELF or map file and confirm that `mg_packed_files`, `mg_mem_files`, `mg_fs_packed`, the HTTP listener, `/index.html`, and the expected asset payloads are retained. Compare firmware size with and without assets if retention is uncertain.
- Add development logging for incoming HTTP method and URI so that network reachability can be distinguished from routing, API, and filesystem failures.
- Verify the root page and a simple machine-readable endpoint separately. At minimum, test or provide on-target checks for `GET /` and `GET /api/state`; the latter helps prove HTTP transport even if the browser UI fails.
- Document that the device URL uses plain `http://` unless TLS is explicitly implemented. Do not allow instructions or browser behavior to imply that `https://` is available when it is not.
- Treat DHCP success as evidence only for link and IP configuration. It does not prove that the HTTP listener is running, that port 80 is reachable, or that packed web assets are active.
- If the UART logs no incoming HTTP request, do not change URI routing or packed-file serving code: the handler has not run yet. From the same host as the browser, first request the exact DHCP address with a direct client such as `curl -v http://<ip>/api/state` and `curl -v http://<ip>/`. A successful `200 OK` proves that the board, port 80, listener, and relevant response path are reachable and moves diagnosis to the browser, proxy, VPN, HTTPS upgrade, cache, or address-entry behavior.
- After changing root-page serving, verify `/`, `/api/state`, and every page dependency such as CSS, JavaScript, and images independently. Do not infer browser failure from a successful root HTML response alone.
- When the direct HTTP tests succeed but entering the address in a browser produces no UART request log, use an explicit `http://<ip>/` URL. Chrome may default or upgrade a manually entered IP address to `https://`; disable its automatic HTTPS/HTTPS-first mode for that address, or test a private window or different browser, before modifying firmware.

## Tools and installations

- Reuse the STM32 and Visual Studio Code tooling already installed on this computer.
- Do not install or recommend additional extensions, packages, toolchains, or applications unless the user explicitly approves it first.
- For VS Code debugging, use the installed STMicroelectronics STM32 extension and its `stlinkgdbtarget` debug type.
- Do not use or require Cortex-Debug when the existing STMicroelectronics debugger can handle the project.
- Prefer the ARM GCC, GDB, ST-LINK GDB server, and STM32CubeProgrammer bundled with the installed STM32CubeIDE.

## CubeMX project structure

- Keep projects based on the STM32CubeMX-generated structure and retain the `.ioc` file.
- Modify CubeMX-generated source or header files only inside designated `USER CODE BEGIN` / `USER CODE END` sections.
- Keep application logic, third-party libraries, platform integration, and web assets in separate non-generated files and directories.
- Before delivery, regenerate the project with STM32CubeMX and verify that all custom integrations survive.
- On macOS, STM32CubeMX `-q` takes a command-script path rather than an `.ioc` path. Use a script containing `config load`, `generate code`, and `exit`, with absolute project paths because the launcher may change its working directory.
- After regeneration, confirm which linker script and startup file the CMake target actually uses. Remove unused generated IDE output if it introduces duplicate startup files or misleading linker scripts.

## CMake and build verification

- Use CMake presets when available.
- Perform a clean build after the final CubeMX regeneration.
- Verify that exactly one startup assembly file is compiled.
- Check for duplicate startup files, obsolete ELF files, CubeMX `_save` files, and stale build products before delivery.
- Keep the generated `.elf`, `.bin`, `.hex`, and map file names deterministic.
- Verify that the ELF is an ARM executable, inspect its size, and confirm that the expected entry point and application/network symbols are linked.
- Inspect the map file for the effective heap and stack reservations and, for Ethernet projects, the DMA-buffer placement. The values in the `.ioc` file are not proof that the linked firmware uses them.
- For Mongoose STM32 Ethernet builds, confirm that `mg_tcpip_driver_stm32h` or the appropriate family driver, `mg_mgr_init`, the HTTP listener, and exactly one Ethernet IRQ handler are linked.
- Treat compilation as verified only after a successful clean build; clearly separate build verification from on-target hardware verification.

## VS Code debugging

- Use `type: "stlinkgdbtarget"` in `.vscode/launch.json`.
- Point `imagesAndSymbols[].imageFileName` explicitly to the current project's ELF file; do not rely on discovery when the path is known.
- Run the project's clean CMake build task before debugging, normally through `preLaunchTask`.
- Set `runEntry` to `main` unless the project requires a different entry point.
- Confirm that the configured ELF exists after the final build.

Example configuration:

```json
{
  "version": "0.2.0",
  "configurations": [{
    "type": "stlinkgdbtarget",
    "request": "launch",
    "name": "STM32Cube: ST-LINK",
    "origin": "snippet",
    "cwd": "${workspaceFolder}",
    "preLaunchTask": "CMake: clean build",
    "runEntry": "main",
    "imagesAndSymbols": [{
      "imageFileName": "${workspaceFolder}/build/debug/PROJECT.elf"
    }]
  }]
}
```

Replace `PROJECT.elf` with the actual current target name and verify the path rather than copying it unchanged.
