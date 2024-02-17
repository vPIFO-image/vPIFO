<h1 align="center">
  <br>
  vPIFO
  <br>
</h1>
<p align="center">
  <a href="#-key-features">Key Features</a> •
  <a href="#-get-started">Get Started</a> •
  <a href="#-license">License</a> •
  <a href="#-links">Links</a>
</p>

## 🎯 Key Features

* FPGA-based hardware virtualization for packet schedulers to support accurate programmable hierarchical scheduling.
* A Scheduling Description Language (SDL) is defined and its compiler is implemented to more conveniently express scheduling requirements and deploy them to programmable schedulers.
* All of our experimental code has been open-sourced, including the software packet scheduler implemented with DPDK and the large-scale ns3 simulation experiments.

## 🚄 Get Started

### 🕶️ Overview

The vPIFO system offers a comprehensive scheduling solution for MTDCs. The figure below shows its three parts - the Scheduling Description Language (SDL) with its compiler, the PIFO Engine, and the PIFO Visor, and its two-phased workflow - initialization and runtime.

![Overview](fig/Overview.png)

During the initialization stage (represented by the dashed line in the figure), the SDL Compiler parses  PIFO instances and their scheduling relationships from the SDL program, generating Rank Computation Algorithms, Operation Generation Rules, and Data Placement Scheme. Rank Computation Algorithms are presented in the form of the SDL Intermediate Representation (IR), which can be further compiled by backend compilers on different platforms to implement the rank computation through Packet Transaction. Operation Generation Rules and Data Placement Scheme serve as configuration information for the running of the vPIFO system. The former is utilized to establish the [Operation Generation Table](fig/OperationGenerationTable.png), enabling the PIFO Visor to transform Packet In/Out Events into PIFO instance P/P Operations. The latter is used to create the [PIFO Instance Address Table](fig/AddressTable.png), allocating storage resources within the PIFO Engine for each PIFO instance.

During the runtime stage (represented by the solid line in the figure), the PIFO Visor receives packets with associated ranks, translates Packet In/Out Events into P/P Operations, and stores them in Operation Queues. At each clock cycle, the PIFO Visor dispatches P/P Operations to the PIFO Engine for processing through Operation Dispatcher.

### ⚙️ Requirements

This repository has hardware and software requirements.

**Hardware Requirements**

* Our testbed evaluation is conducted on the Xilinx Alveo U200 Data Center Accelerator Card.

**Software Requirements**

* ns-3: ns-3.26
* Python: 3.8.10 (the same is best)
* DPDK == 17.08.1 (For more information on DPDK requirements, refer to [DPDK_scheduler README](./software/experiment/DPDK_scheduler/Readme.md).)
* [Vivado Design Suite](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/archive.html) >= 2018.3 (We used the 2018.3 version in our experiments. Newer versions should be compatible with 2018.3, but we have not tested with older versions.)

> 🔔 In this document, all 'python' and 'pip' refer to the python version of 3.8.10.

### 🔧 Hardware



### 📮 SDL Compiler



### 🧪 Experiments

DPDK Scheduler

Algorithm Simulation

ns-3 Simulation

Tag Priority


### 📝 SDL Compiler Use Cases

TBD

## 📖 License

The project is released under the MIT License.

## 🔗 Links

Below are some links that may also be helpful to you:

- [BMWTree](https://github.com/BMWTree/BMWTree)
- [DPDK](https://www.dpdk.org/)
- [ns-3](https://www.nsnam.org/)
- [Vivado Design Suite](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/archive.html)


