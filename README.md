# helpers

Research notes and measurements for the Composite VMX work.

## What is here

- **`documents/`** — measurement notes, open questions, and reading list.

## What moved

`compiling_linux/` was the original machinery for building a Linux guest image:
BusyBox initramfs scripts, benchmark programs, and kernel modules. That work now
lives in **[cos-vmimg](https://github.com/esmakokten/cos-vmimg)**, which builds
the guest image Composite's `simple_vmm` boots, and is used there as a submodule.

The programs and modules were carried over unchanged. The build machinery around
them was rewritten, because it had a few problems worth naming:

- The `/init` that made images boot lived in a directory `.gitignore` excluded,
  and `busybox_initrd.sh` deleted and rewrote it on every run — so the one file
  that made the image work was never in git.
- The BusyBox download used one version string for two URLs that spell the
  version differently (`1.37.0` vs `1_37_0`), so the fetch 404'd. The comment
  claiming busybox.net was down was wrong.
- `programs/Makefile` ran the BusyBox script and then `cd -`, which swallowed the
  exit status: a failed build reported success and packed a stale tree.
- Kernel modules were built without a populated `Module.symvers`, so `modpost`
  produced `.ko` files with every external symbol unresolved. They compiled
  cleanly and would have failed at `insmod`; the copy step hid it with
  `2>/dev/null || true`.

`compiling_linux/` is kept here for history. It is not the build path any more —
use cos-vmimg.

`tinylinux.sh` was never part of that path: it builds an unrelated vanilla 6.5.12
kernel and was a learning exercise.
