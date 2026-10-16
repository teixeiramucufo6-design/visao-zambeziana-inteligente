FROM ubuntu:24.04 AS build
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git ca-certificates libssl-dev pkg-config \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY CMakeLists.txt ./
COPY src ./src
COPY public ./public
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --parallel 2

FROM ubuntu:24.04 AS runtime
RUN apt-get update && apt-get install -y --no-install-recommends ca-certificates libssl3 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /app/build/vzi_server /app/vzi_server
COPY --from=build /app/public /app/public
ENV PORT=8080
EXPOSE 8080
CMD ["/app/vzi_server"]
