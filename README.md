# fling

Fast and simple file transfer over network.

## Overview

`fling` is a lightweight command-line tool for transferring files between machines over TCP. It focuses on simplicity and speed, with a clean progress bar showing transfer statistics.

## Features

- **Simple CLI**: Just two commands - `serve` and `send`
- **Fast transfers**: Optimized for speed with efficient buffer handling
- **Progress tracking**: Real-time progress bar with transfer speed
- **Security**: Path traversal protection and file size validation
- **Cross-platform**: Works on Linux and macOS
- **Lightweight**: Single binary, no dependencies

## Build and run

```bash
git clone https://github.com/ddoroshev/fling.git
cd fling
make
bin/fling
```

### Requirements

- C compiler (gcc or clang)
- make
- POSIX-compliant system (Linux, macOS, \*BSD)

## Usage

### Receiving files (server mode)

```bash
# Start server on default port (54321)
fling serve

# Start server on custom port
fling serve 8080
```

### Sending files

```bash
# Send file to host on default port
fling send myfile.txt 192.168.1.100

# Send file to host on custom port
fling send myfile.txt 192.168.1.100 8080
```

#### Examples

On the receiving machine:
```
$ fling serve
Listening on 0:54321...
```

On the sending machine:
```
$ fling send document.pdf 192.168.1.100
[########################################] 100% (15.34 MB / 15.34 MB) 245.67 MB/s
File sent successfully! Completed in 0.06 seconds (245.67 MB/s avg)
```

## Building and Testing

```bash
# Build the project
make

# Build and run tests
make test testrun

# Run tests including slow/large file tests
make testrun-slow

# Clean build artifacts
make clean
```

## Performance

`fling` is optimized for speed and can achieve near-line-speed transfers on local networks. In testing, it achieves:
- ~900 MB/s on localhost transfers
- ~110 MB/s on gigabit LAN (near theoretical maximum)

## Security

- **Path traversal protection**: Prevents directory traversal attacks in filenames
- **Size validation**: Verifies file sizes before and after transfer
- **Input validation**: Sanitizes all user inputs

## Limitations

- Single file transfer only (no directory support yet)
- No encryption
- No resume capability for interrupted transfers

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
