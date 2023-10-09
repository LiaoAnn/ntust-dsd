# Project2 - Quine McCluskey

#### Student ID: B11130237

#### Name: 廖廷安

## Getting Started

### Docker

```bash
# build docker image
docker build -t quine-mccluskey .

# run docker image
docker run -it --name quine-mccluskey -v $(pwd):/workspace project1-robdd /bin/bash

# Build the program
clang++ -o main src/main.cpp

# Run the program
./main <input_pla_file> <output_pla_file>
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
clang++ -o main src/main.cpp

# Run the program
./main <input_pla_file> <output_pla_file>
```

> Please make sure your outout directory is exist. If not, please create it first.
