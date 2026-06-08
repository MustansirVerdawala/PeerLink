# 🌐 PeerLink: A Socket-Based P2P File Sharing System

A peer-to-peer (P2P) file-sharing application built in **C using BSD sockets**, featuring a **central index server** for content discovery and **direct peer-to-peer TCP file transfers**.

The system allows peers to register hosted content, search for files available on the network, download content directly from other peers, and dynamically join or leave the network. The project demonstrates core networking concepts including **UDP communication**, **TCP file transfer**, **socket programming**, and **distributed content management**.

---

## 📌 Project Overview

The application consists of two major components:

### 🖥️ Index Server

The central server maintains a registry of:

* Connected peers
* Registered content
* Peer IP addresses
* Peer TCP ports

The server does **not store files**. Instead, it acts as a directory service that helps peers locate content hosted elsewhere on the network.

### 👥 Peers

Each peer acts as both:

* A **client** that communicates with the index server
* A **file server** that provides content to other peers

Once a file is located through the index server, the download occurs directly between peers using TCP.

---

## 🏗️ System Architecture

```text
          +------------------+
          |   Index Server   |
          |      (UDP)       |
          +--------+---------+
                   |
      --------------------------------
      |              |              |
      |              |              |
   Peer A         Peer B         Peer C
  UDP/TCP        UDP/TCP        UDP/TCP
      |              ^
      |______________|
        TCP Download
```

### Communication Protocols

| Purpose                   | Protocol |
| ------------------------- | -------- |
| Peer ↔ Index Server       | UDP      |
| Peer ↔ Peer File Transfer | TCP      |

UDP was selected for lightweight content management operations, while TCP ensures reliable file delivery between peers.

---

## 📦 Features

### ✅ Content Registration

Peers can register files with the index server.

The server stores:

* Peer name
* Content name
* IP address
* TCP port

---

### 🔍 Content Search

Peers can search for content by filename.

If the content exists, the server returns:

* Hosting peer IP address
* Hosting peer TCP port

Otherwise, an error response is returned.

---

### 📥 Direct File Download

After locating content:

1. The requesting peer connects directly to the hosting peer.
2. A TCP connection is established.
3. The file is transferred reliably.
4. The receiving peer stores the file locally.

---

### 📋 Content Listing

Peers can request a complete list of all registered content currently available on the network.

---

### 🗑️ Content De-registration

Peers can remove individual files from the registry without disconnecting from the network.

---

### 🚪 Peer Exit

When a peer leaves:

* All registered content associated with that peer is removed.
* The registry is updated automatically.
* Other peers can no longer discover those files.

---

## 📡 Protocol Data Units (PDUs)

The application uses custom PDUs for communication.

### Server Operations

| PDU | Function            |
| --- | ------------------- |
| R   | Register Content    |
| S   | Search Content      |
| O   | List Content        |
| T   | De-register Content |
| Q   | Quit                |
| F   | End of List         |
| E   | Error               |

---

### Client Menu

| Option | Function            |
| ------ | ------------------- |
| 1      | List Content        |
| 2      | Search & Download   |
| 3      | Register Content    |
| 4      | De-register Content |
| 0      | Exit Network        |

---

## ⚖️ Load Balancing

The index server maintains a usage counter for each registered content entry.

When multiple peers host the same content:

* The server selects the peer with the lowest usage count.
* The usage counter is incremented after each successful search.

This prevents a single peer from serving all download requests and distributes traffic across the network.

---

## 🧪 Testing

The system was tested using multiple peers connected simultaneously to a single index server.

### Successful Test Cases

* Peer registration
* Duplicate registration detection
* Content listing
* Content search
* TCP file transfer
* De-registration
* Peer disconnection
* Multiple simultaneous peers
* Load-balanced content selection

---

## 📊 Observations

* UDP communication proved efficient for lightweight registry operations.
* TCP provided reliable file transfer without corruption.
* The index server successfully maintained peer and content information throughout testing.
* Multiple peers could register and share content concurrently.
* The load-balancing mechanism distributed downloads across available content providers.

### Known Limitation

A downloaded file was not automatically re-registered by the receiving peer after transfer.

As a result, newly downloaded content required manual registration before becoming available to other peers.

---

## 🛠️ Technologies Used

* C
* BSD Sockets
* TCP/IP Networking
* UDP Communication
* Linux/Unix System Calls
* Process Management (`fork()`)
* Client-Server Architecture

---

## 🎯 Learning Outcomes

This project provided practical experience with:

* Socket programming
* TCP and UDP networking
* Client-server communication
* Peer-to-peer architectures
* Custom protocol design
* File transfer systems
* Network process management
* Distributed system fundamentals

---

## 📎 Authors

**Mustansir Verdawala**
Toronto Metropolitan University

**Harsh Solanki**
Toronto Metropolitan University

---

## 📚 Course Information

**COE768 – Computer Networks**
Department of Electrical, Computer, and Biomedical Engineering
Toronto Metropolitan University

**Instructor:** Dr. Cungang Yang
