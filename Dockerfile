FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    libasan8 \
    make \
    g++

WORKDIR /app

# RUN useradd -m -d /home/serv -s /bin/bash serv

# Copy all source and www directories
COPY . .
# COPY www ./www
# COPY other_files_if_any .

# Now set permissions after files are in place
# RUN chown -R root:root ./srcs && chmod -R 700 ./srcs
# RUN chown -R serv:serv ./www && chmod -R 750 ./www
# RUN chown root:root . && chmod 750 .

# USER serv

CMD ["bash"]
