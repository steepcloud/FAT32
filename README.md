# FAT32 File System Simulation

This project is a C++ simulation of a FAT32 (File Allocation Table) file system. It includes the essential structure and functionality to model a simplified version of FAT32, where data and metadata management are handled through custom code.

## Features

- **File Allocation Table (FAT)**: An array that simulates the FAT structure, mapping allocation units and defining the space for the ROOT and HDD zones.
- **Root Directory Simulation**: The code includes a root directory that can hold metadata for up to 340 files, each represented by a 17-byte file name.
- **HDD Structure**: A virtual HDD matrix representing disk space with 4096 allocation units, each with a fixed size of 32 bytes.
- **Metadata Management**: Prepares for functionality to handle file name storage, allocation tracking, and data simulation.

## Code Structure

- **Constants**:
  - `Nr_UA`: Total allocation units on the HDD.
  - `UA`: Size of each allocation unit.
  - `nrFisROOT`: Maximum files allowed in the ROOT directory.
- **FAT Array**: Holds allocation information for each unit on the simulated disk.
- **HDD Vector Matrix**: A two-dimensional vector representing data blocks in HDD.

## Planned Features

- Save and update HDD content to an external file for tracking and persistence.
- Functions to add, read, and manage files within the ROOT and FAT structures.

This simulation offers an overview of how the FAT32 file system works, specifically focusing on storage, file management, and allocation. Perfect for educational purposes and foundational understanding of file system structure.
