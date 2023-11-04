FROM ubuntu:23.04
RUN apt-get update
RUN apt-get install -y build-essential g++ cmake ninja-build make 
RUN apt-get install -y g++-13
RUN apt-get install -y gcc-13
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 12
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 13
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 12
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 13
