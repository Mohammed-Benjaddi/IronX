FROM ubuntu:latest

# Install required tools and libraries
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    libasan8 \
    make \
    g++ \
    && apt-get clean

WORKDIR /app

COPY . .

RUN mkdir -p obj

CMD ["bash"]
