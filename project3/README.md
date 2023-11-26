# Project 3 - State Minimization

##### Student ID: B11130237

##### Name: 廖廷安

## Getting Start

### Docker

```bash
# Build the Image
docker build -t project3 -f Dockerfile .

# Run the Container
docker run -it --name project3 -v $(pwd):/workspace project3 /bin/bash

# Build the program
clang++ -o main src/B11130237.cpp

# Run the program
./main <kiss_in> <kiss_out> <dot_file>
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
clang++ -o main src/B11130237.cpp

# Run the program
./main <kiss_in> <kiss_out> <dot_file>
```