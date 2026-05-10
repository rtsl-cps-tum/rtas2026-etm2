## ETM^2: Empowering Traditional Memory Bandwidth Regulation using ETM
ETM^2 regulator is Memory Bandwidth Regulator that uses the CoreSight Debug and Trace infrastructure on the Arm Cortex-A based SoCs.
For more information have a look at: https://arxiv.org/abs/2603.16490

This repo contains an example implementation of ETM based Memory Bandwidth regulation for the ZynqMP SoCs such as the ZCU102 platform.
For an implementation on a linux based platform we require two main components:
- `kernel module` to register an IRQ with the CTI. (Example Device Tree entries for the CTI can be found in docs/dtbs for ZCU102)
- `regulator` user space code to configure the ETM, PMU, CTM and DBG interfaces to perform memory bandwidth regulation

### Compilation
For kernel module:
```
## Assuming cross compilation on an x86_64 platform
$ cd ./kernel_module/
$ make KDIR=/path/to/kernel/source
```

For Regulator
```
## Assuming cross compilation on an x86_64 platform
$ cd ./regulator/
$ make 
# etm_regulator is the application
# requires root access for some operations

```

### Usage
```
## Insert the kernel module to register the IRQ handlers
$ insmod cti_irq_driver.ko

## Enable ETM^2 memory bandwidth regulation
## For example with ETM^2 PR method with nearly 100, 200, 400, 800 MB/s 
## regualtion target for Cores 0 to 3 respectively on the ZCU102 platform
$ ./etm_regulator --budget 31 62 125 250 --cycle 24000 --mode 1

## General usage
$ ./etm_regulator --budget <x x x x> --cycle <x> --mode <x>
    --budget(-b):  sets the combined budget in ETM counter for PMU
    --cycle(-c):   sets the regulation period
    --mode(-m)
    \t 0 for No Regulation 
    \t 1 for ETM PR algorithm
    \t 2 for ETM TB 1:3 algorithm
    \t 3 for ETM TB 2:2 algorithm
    \t 4 for ETM TB 3:1 algorithm
```
For more information on intended usage and regulation have a look at: https://arxiv.org/abs/2603.16490

### Citing the Code
Do consider to cite the paper
```
@article{zuepke2026etm2,
  title        = {ETM2: Empowering Traditional Memory Bandwidth Regulation using ETM},
  author       = {Zuepke, Alexander and Pradhan, Ashutosh and Ottaviano, Daniele and Bastoni, Andrea and Caccamo, Marco},
  year         = {2026},
  journal      = {arXiv preprint arXiv:2603.16490},
  eprint       = {2603.16490},
  archivePrefix= {arXiv},
  primaryClass = {cs.PF},
  url          = {https://arxiv.org/abs/2603.16490}
}
```

For more examples on using the CoreSight infrastructure on ZynqMP SoCs follow: https://github.com/wchen258/TPAw0v
