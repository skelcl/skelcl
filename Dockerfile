FROM ubuntu:14.10

RUN apt-get update && apt-get install -y --force-yes \
     clang \
     cmake \
     g++ \
     libclang-dev \
     libedit-dev \
     libssl-dev \
     llvm \
     ocl-icd-opencl-dev \
     unzip \
     wget \
     zlib1g-dev

ADD . skelcl_src/

RUN cd skelcl_src && \
     wget http://googletest.googlecode.com/files/gtest-1.7.0.zip && \
     unzip -q gtest-1.7.0.zip && \
     mv gtest-1.7.0 libraries/gtest && \
     rm gtest-1.7.0.zip

RUN cd skelcl_src && mkdir build

RUN cd skelcl_src/build && cmake ..

RUN cd skelcl_src/build && make
