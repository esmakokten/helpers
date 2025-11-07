# Open Questions

- gcc -static option uses glibc static linking. But which version, how is it compatible with kernel version that we build?
- How many VMs a single machine have for cloud providers? Which operating system are they using? How are they sharing resources (cores/memory/disk IO etc)? And hardware technologies (ie SRIOV, VT-d etc) they are using to improve performance?

# Addressed Questions

- How to trace the KVM fastpath and slowpath exits?

    One option could be the trace KVM functions. `trace-cmd` can be used to trace kernel functions and events. For KVM, we can trace the kvm_entry and kvm_exit events to see when the VM exits and enters. This will help us identify the fastpath and slowpath exits.

    For the fastpath exits, we can look for the kvm_exit events that are handled directly by the KVM module in the kernel. These exits are typically caused by instructions that can be handled by KVM without involving userspace (QEMU). For the slowpath exits, we can look for the kvm_exit events that result in a userspace exit. 

    Some useful trace points to consider. You can find more details in the Linux kernel source code under `arch/x86/kvm/trace.h`.
        kvm:kvm_entry -> indicates the VM is entering guest mode.
        kvm:kvm_exit -> VM is exiting guest mode, you can see the exit reason in the event data.
        kvm:kvm_userspace_exit -> VM is exiting to userspace (QEMU).
        kvm:kvm_pio -> Port I/O operations, useful for tracing IN/OUT instructions.
    ```bash
    sudo trace-cmd record -e kvm:kvm_exit -e kvm:kvm_entry -e kvm:kvm_userspace_exit -e kvm:kvm_pio 
    # Run your measurement workload here 
    sudo trace-cmd report > run_trace.txt
    sudo trace-cmd reset 
    ```

    Then analyze the `run_trace.txt` file to see the sequence of events and identify fastpath vs slowpath exits based on the exit reasons and whether they involve userspace.