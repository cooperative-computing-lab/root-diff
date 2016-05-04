# Docker image for root_diff
FROM centos
MAINTAINER Charles Zheng 

# Required software
RUN yum install -y gcc wget gcc-c++ unzip make numpy
# Required libs
RUN yum install -y libSM libX11 libXext libXpm libXft
# Get root 6.04 binary
RUN wget https://root.cern.ch/download/root_v6.04.16.Linux-centos7-x86_64-gcc4.8.tar.gz -O /root/root.tar.gz
RUN cd /root && tar -xzf root.tar.gz && rm root.tar.gz

# Set env for root 
ENV MANPATH "/root/root/man:"
ENV OLDPWD /root/root
ENV LD_LIBRARY_PATH /root/root/lib
ENV LIBPATH /root/root/lib
ENV PATH "/root/root/bin:$PATH"
ENV PWD /root/root/bin
ENV ROOTSYS /root/root
ENV DYLD_LIBRARY_PATH /root/root/lib
ENV PYTHONPATH /root/root/lib
ENV SHLIB_PATH /root/root/lib
ENV CMAKE_PREFIX_PATH /root/root

# Get root_diff
RUN wget https://github.com/charleszheng44/root_diff/archive/master.zip -O /root/root-diff.zip
RUN cd /root && unzip root-diff.zip && rm root-diff.zip
RUN cd /root/root_diff-master && make

# Set default working dir
WORKDIR /root
