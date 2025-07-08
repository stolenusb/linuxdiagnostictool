FROM debian:bookworm-slim

RUN apt update && \
    apt install -y g++ make ncurses-dev procps && \
    apt clean

WORKDIR /app

COPY . .

RUN make

CMD ["./bin/linuxdiag"]