# Project 1 - ROBDD

##### Student ID: B11130237

##### Name: 廖廷安

## Getting Start

### Docker

```bash
# Build the Image
docker build -t project1-robdd -f Dockerfile .

# Run the Container
docker run -it --name project1-robdd-container -v $(pwd):/workspace project1-robdd /bin/bash

# Build the program
clang++ -o robdd src/main.cpp

# Run the program
./robdd <pla_file_path> <dot_file_path>
```

### WSL (or other linux environment)

```bash
# Update and install packages
RUN apt-get update && apt-get install -y \
    clang \
    cmake \
    gdb \
    git \
    vim \
    && apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Build the program
clang++ -o robdd src/main.cpp

# Run the program
./robdd <pla_file_path> <dot_file_path>
```