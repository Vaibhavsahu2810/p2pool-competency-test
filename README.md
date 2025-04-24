#  P2Poolv2 NS-3 Competency Test


## What Is This Test?

This is the **competency test** for the P2Poolv2 Sharechain simulation (with uncles) under **Summer of Bitcoin 2025**. It models decentralized mining share propagation using **NS-3.35**, with logs, PCAP traces, and animated flow via NetAnim.


## Objectives

-  **Mesh Topology:** Simulate 10+ nodes connected via point-to-point links.
- **Share Broadcast:** Each node sends `node_X_share` to its peers every 2 seconds.
- **Logging & Tracing:**
  - `NS_LOG_INFO` prints for sends/receives
  - `.pcap` files for packet traces
  - `.xml` file for NetAnim animation
-  **Validation:**
  - Real-time terminal logs
  - Inspectable `.pcap` files in Wireshark
  - Visual packet flow in NetAnim GUI


##  Tech Stack

| Tool        | Purpose                             |
|-------------|-------------------------------------|
| **NS-3.35** | Discrete-event network simulation   |
| **NetAnim** | Packet animation viewer             |
| **Wireshark** | Analyze `.pcap` trace files       |
| **Docker**  | Portable development environment    |


##  Repository Structure

```
p2pool-competency-test/
├── scratch/
│   └── p2pool-comp.cc          # Main NS-3 simulation
├── mesh-network-*.pcap         # PCAP traces per interface
├── mesh-sim.xml                # NetAnim XML trace
├── Screenshots/               
│   ├── build-success.png
│   ├── runtime-log.png
│   └── netanim-snapshot.png
└── README.md                  
```

##  Quick Start

### 1. Docker Setup

```bash
docker run -it ubuntu
apt update && apt install -y build-essential git g++ python3
git clone https://gitlab.com/nsnam/ns-3-dev.git ns3
cd ns3 && ./build.py --enable-examples --enable-tests
```

### 2. Open in VS Code

- Launch **VS Code**
- Use **Remote Containers → Attach to Running Container**
- Navigate to `/ns3/ns-3.35`

### 3. Run the Simulation

```bash
./waf --run scratch/p2pool-comp
```

- Outputs `.pcap` files (e.g., `mesh-network-0-0.pcap`)
- Generates NetAnim trace: `mesh-sim.xml`

## Sample Logs

Below is the final log from a successful simulation:

```text
Waf: Entering directory `/ns3/ns-3.35/build'
[2830/2877] Compiling scratch/p2pool-comp.cc
[2838/2877] Linking build/scratch/p2pool-comp
'build' finished successfully (1.844s)

0.101s - Node[0] sent: "node_0_share" to 10.1.1.2
0.201s - Node[1] sent: "node_1_share" to 10.0.1.2
...
1.502s - Node[5] received: "node_3_share" from 10.2.1.1
1.502s - Node[7] received: "node_6_share" from 10.3.1.1
...
```



## PCAP Traces

After running the simulation, PCAP files are saved in the root directory:

```bash
mesh-network-0-0.pcap  mesh-network-0-1.pcap  ... mesh-network-0-9.pcap
```

Open with **Wireshark** to inspect UDP packets exchanged between nodes.


##  NetAnim Visualization

To visualize share flow over time:

```bash
netanim mesh-sim.xml
```



## Screenshots Overview
Build Success 

<img width="487" alt="successfully build ns3-3 35-p2pool-simulation" src="https://github.com/user-attachments/assets/066c3693-29d9-4023-99d4-300e7c5fb870" />

netanim-mesh-simulation
<img width="1470" alt="netanim-mesh-simulation" src="https://github.com/user-attachments/assets/5e466e83-6ee1-49c6-9661-1c3295a08bcd" />

Runtime Log
<img width="933" alt="final Runtime Log" src="https://github.com/user-attachments/assets/0588073e-9fae-4d3c-b533-21ac815057f0" />




## Outcome

This simulation fulfills all technical and validation requirements for the P2Poolv2 competency test under SoB 2025—demonstrating sharechain gossip, packet-level tracing, and visual flow in a fully connected decentralized network.

