# Measurements

The list of measurements that we'd like to get:
1. Guest user → **(executes F)** causes VMEXIT → back to guest user (KVM fast path only)
2. Same as (1) but **(executes S)** so that the exit reason forces userspace exit (QEMU round-trip)
3. Guest user → executes a privileged instruction in Guest Kernel (causes VM-Exit) → back to guest user (KVM fast path)
4. Same as (3) but via userspace exit (QEMU round-trip)
5. Guest kernel →  VMEXIT → back to guest kernel (fast path)
6. Same as (5) but via userspace exit

**F**: Instruction that needs to be handled in fastpath KVM and back. Some options:
- `CPUID` (Would be the easiest option)
- `RDMSR` 
- `VMCALL` instruction could also be used as alternative but we need to look in to args that expected by KVM.

**S**: Instruction for slowpath to handle KVM->Qemu->KVM->VM.
- `OUTB` to a port that handled by Qemu. You can give the port num to Qemu via [debugcon](https://qemu-project.gitlab.io/qemu/system/invocation.html#hxtool-9) option. Also we may need to do some syscalls to linux to allow the user program to execute IN/OUT instructions.


## Setup

To setup the environment for these measurements, we need:
1. A minimal Linux kernel with KVM support enabled.
2. A minimal initramfs to boot the kernel.
3. A user program that runs inside the guest VM to perform the measurements.
4. QEMU configured to use KVM for virtualization.

## Measurement Execution

The user program inside the guest VM will execute the instructions as per the measurement list above. We can use `rdtsc` instruction to measure the time taken for each operation. The operation will cause a VMEXIT, and we will measure the time taken for the VMEXIT and return to the guest.

- Use RDTSCP or RDTSC+fences inside the guest for cycle-accurate timing.
- `RDTSC exiting` needs to be disabled and `Enable RDTSCP` needs to be enabled in the KVM configuration to use `RDTSCP` for accurate timing.
- We need to ensure that the path taken is as expected (fastpath vs slowpath). We can verify this by using tracing mechanisms.

## Measurement Isolation Settings

To ensure accurate measurements, we should isolate the measurement environment as much as possible. This includes (We need to document these in detail):

- Use a single vCPU while benchmarking; add CPU isolation on the host.
	Pin guest vCPU to an isolated host core; disable turbo/freq scaling; use `nohz_full`, `irqbalance` off for the test cores.
- Lock memory; warm caches; run for many iterations; **report median & percentiles.**


Interference-free Operating System: A 6 Years’ Experience in Mitigating Cross-Core Interference in Linux -
https://doi.org/10.48550/arXiv.2412.18104

Here is some example that uses this study's approach to isolate CPU and memory for benchmarking:
https://github.com/derekgwu/virt-project?tab=readme-ov-file

## Measurement Reporting

We`ll take measurements in our servers as well and report the following for each measurement:
- Host machine specs (CPU model, kernel version, KVM version)
- Guest kernel version
- QEMU version
- Measurement type (fastpath/slowpath)
- Instruction executed
- Time taken (cycles) with median and percentiles
