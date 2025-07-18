FROM debian:bookworm-slim

RUN apt update && \
    apt install -y g++ make ncurses-dev procps && \
    apt clean

WORKDIR /app

COPY . .


CMD ["./dev.sh"]