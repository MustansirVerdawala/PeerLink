{\rtf1\ansi\ansicpg1252\cocoartf2870
\cocoatextscaling0\cocoaplatform0{\fonttbl\f0\fswiss\fcharset0 Helvetica;}
{\colortbl;\red255\green255\blue255;}
{\*\expandedcolortbl;;}
\margl1440\margr1440\vieww11520\viewh8400\viewkind0
\pard\tx720\tx1440\tx2160\tx2880\tx3600\tx4320\tx5040\tx5760\tx6480\tx7200\tx7920\tx8640\pardirnatural\partightenfactor0

\f0\fs24 \cf0 # \uc0\u55356 \u57104  PeerShare: A Socket-Based P2P File Sharing System\
\
A peer-to-peer (P2P) file-sharing application built in **C using BSD sockets**, featuring a **central index server** for content discovery and **direct peer-to-peer TCP file transfers**.\
\
The system allows peers to register hosted content, search for files available on the network, download content directly from other peers, and dynamically join or leave the network. The project demonstrates core networking concepts including **UDP communication**, **TCP file transfer**, **socket programming**, and **distributed content management**.\
\
---\
\
## \uc0\u55357 \u56524  Project Overview\
\
The application consists of two major components:\
\
### \uc0\u55357 \u56741 \u65039  Index Server\
\
The central server maintains a registry of:\
\
* Connected peers\
* Registered content\
* Peer IP addresses\
* Peer TCP ports\
\
The server does **not store files**. Instead, it acts as a directory service that helps peers locate content hosted elsewhere on the network.\
\
### \uc0\u55357 \u56421  Peers\
\
Each peer acts as both:\
\
* A **client** that communicates with the index server\
* A **file server** that provides content to other peers\
\
Once a file is located through the index server, the download occurs directly between peers using TCP.\
\
---\
\
## \uc0\u55356 \u57303 \u65039  System Architecture\
\
```text\
          +------------------+\
          |   Index Server   |\
          |      (UDP)       |\
          +--------+---------+\
                   |\
      --------------------------------\
      |              |              |\
      |              |              |\
   Peer A         Peer B         Peer C\
  UDP/TCP        UDP/TCP        UDP/TCP\
      |              ^\
      |______________|\
        TCP Download\
```\
\
### Communication Protocols\
\
| Purpose                   | Protocol |\
| ------------------------- | -------- |\
| Peer \uc0\u8596  Index Server       | UDP      |\
| Peer \uc0\u8596  Peer File Transfer | TCP      |\
\
UDP was selected for lightweight content management operations, while TCP ensures reliable file delivery between peers.\
\
---\
\
## \uc0\u55357 \u56550  Features\
\
### \uc0\u9989  Content Registration\
\
Peers can register files with the index server.\
\
The server stores:\
\
* Peer name\
* Content name\
* IP address\
* TCP port\
\
---\
\
### \uc0\u55357 \u56589  Content Search\
\
Peers can search for content by filename.\
\
If the content exists, the server returns:\
\
* Hosting peer IP address\
* Hosting peer TCP port\
\
Otherwise, an error response is returned.\
\
---\
\
### \uc0\u55357 \u56549  Direct File Download\
\
After locating content:\
\
1. The requesting peer connects directly to the hosting peer.\
2. A TCP connection is established.\
3. The file is transferred reliably.\
4. The receiving peer stores the file locally.\
\
---\
\
### \uc0\u55357 \u56523  Content Listing\
\
Peers can request a complete list of all registered content currently available on the network.\
\
---\
\
### \uc0\u55357 \u56785 \u65039  Content De-registration\
\
Peers can remove individual files from the registry without disconnecting from the network.\
\
---\
\
### \uc0\u55357 \u57002  Peer Exit\
\
When a peer leaves:\
\
* All registered content associated with that peer is removed.\
* The registry is updated automatically.\
* Other peers can no longer discover those files.\
\
---\
\
## \uc0\u55357 \u56545  Protocol Data Units (PDUs)\
\
The application uses custom PDUs for communication.\
\
### Server Operations\
\
| PDU | Function            |\
| --- | ------------------- |\
| R   | Register Content    |\
| S   | Search Content      |\
| O   | List Content        |\
| T   | De-register Content |\
| Q   | Quit                |\
| F   | End of List         |\
| E   | Error               |\
\
---\
\
### Client Menu\
\
| Option | Function            |\
| ------ | ------------------- |\
| 1      | List Content        |\
| 2      | Search & Download   |\
| 3      | Register Content    |\
| 4      | De-register Content |\
| 0      | Exit Network        |\
\
---\
\
## \uc0\u9878 \u65039  Load Balancing\
\
The index server maintains a usage counter for each registered content entry.\
\
When multiple peers host the same content:\
\
* The server selects the peer with the lowest usage count.\
* The usage counter is incremented after each successful search.\
\
This prevents a single peer from serving all download requests and distributes traffic across the network.\
\
---\
\
## \uc0\u55358 \u56810  Testing\
\
The system was tested using multiple peers connected simultaneously to a single index server.\
\
### Successful Test Cases\
\
* Peer registration\
* Duplicate registration detection\
* Content listing\
* Content search\
* TCP file transfer\
* De-registration\
* Peer disconnection\
* Multiple simultaneous peers\
* Load-balanced content selection\
\
---\
\
## \uc0\u55357 \u56522  Observations\
\
* UDP communication proved efficient for lightweight registry operations.\
* TCP provided reliable file transfer without corruption.\
* The index server successfully maintained peer and content information throughout testing.\
* Multiple peers could register and share content concurrently.\
* The load-balancing mechanism distributed downloads across available content providers.\
\
---\
\
## \uc0\u55357 \u57056 \u65039  Technologies Used\
\
* C\
* BSD Sockets\
* TCP/IP Networking\
* UDP Communication\
* Linux/Unix System Calls\
* Process Management (`fork()`)\
* Client-Server Architecture\
\
---\
\
## \uc0\u55356 \u57263  Learning Outcomes\
\
This project provided practical experience with:\
\
* Socket programming\
* TCP and UDP networking\
* Client-server communication\
* Peer-to-peer architectures\
* Custom protocol design\
* File transfer systems\
* Network process management\
* Distributed system fundamentals\
\
---\
\
## \uc0\u55357 \u56526  Authors\
\
**Mustansir Verdawala**\
Toronto Metropolitan University\
\
**Harsh Solanki**\
Toronto Metropolitan University\
\
---\
\
## \uc0\u55357 \u56538  Course Information\
\
**COE768 \'96 Computer Networks**\
Department of Electrical, Computer, and Biomedical Engineering\
Toronto Metropolitan University\
\
**Instructor:** Dr. Cungang Yang}