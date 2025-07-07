FROM debian:bullseye-slim

# Install necessary build tools and libraries
RUN apt update && \
    apt install -y g++ make ncurses-dev procps && \
    apt clean

# Set working directory
WORKDIR /app

# Copy project files
COPY . /app

# Build the project
RUN make

# Run the binary
CMD ["./bin/linuxdiag"]
