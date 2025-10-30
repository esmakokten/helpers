# Resources

## Documentation
Intel manual: https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
Linux kernel - KVM source code: https://github.com/torvalds/linux/tree/master/virt/kvm
KVM documentation: https://www.kernel.org/doc/html/latest/virt/kvm/index.html
Qemu source code: https://github.com/qemu/qemu
Qemu documentation: https://www.qemu.org/docs/master/index.html

# Further Learning & Mentions
Here is a list of resources for further learning and exploration:

## Other Popular Hypervisors 
- Firecracker (Qemu alternative uses kvm):https://github.com/firecracker-microvm/firecracker
- Acrn(Does not use kvm. It has its own module&baremetal OS): https://github.com/projectacrn/acrn-hypervisor/  doc: https://projectacrn.github.io/latest/
- Xen (Popular open-source hypervisor): https://www.xenproject.org/

## Mentioned Papers
- Byways Paper (VMM design of former phd student implemented on top of composite microkernel) https://dl.acm.org/doi/pdf/10.1145/3698038.3698547
- Interference-free Operating System: A 6 Years’ Experience in Mitigating Cross-Core Interference in Linux -
https://doi.org/10.48550/arXiv.2412.18104


## Simple explanatory kvm host examples
- A Minimal hypervisor uses KVM, good to understand the basics: [wiser](https://github.com/flouthoc/wiser)
- Hello-world: https://github.com/dpw/kvm-hello-world/tree/master
- A little more complex one: https://github.com/sysprog21/kvm-host