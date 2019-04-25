FROM centos:6.10

#Install libraries 

RUN yum -y update

RUN yum install -y \
gcc \
gcc-c++ \
centos-release-scl-rh \
devtoolset-6-gcc-c++ \
epel-release \
wget \
cmake 

#Install Boost

RUN wget https://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.gz 
RUN tar -xvzf boost_1_63_0.tar.gz
RUN cd boost_1_63_0 && ./bootstrap.sh
RUN cd boost_1_63_0 && ./b2 || true 
RUN cd boost_1_63_0 && ./bjam install || true

#Install Qt

RUN yum whatprovides "*/qt5"; yum -y install qt5-qtbase; yum -y install qt5-qtbase-devel;

#Install glibc

RUN wget http://ftp.gnu.org/gnu/glibc/glibc-2.14.tar.gz
RUN tar -xvzf glibc-2.14.tar.gz
RUN cd glibc-2.14 && \
mkdir build && \
cd build && \
../configure --prefix=/opt/glibc-2.14

RUN yum install -y \
libpng-devel \
glibc-static \
libstdc++-static \
libpng-static \
graphviz \
scl-utils \
devtoolset-7 

# Install gosu
ENV GOSU_VERSION=1.11
RUN gpg --keyserver ha.pool.sks-keyservers.net --recv-keys B42F6819007F00F88E364FD4036A9C25BF357DD4 \
            && curl -o /usr/local/bin/gosu -SL "https://github.com/tianon/gosu/releases/download/${GOSU_VERSION}/gosu-amd64" \
            && curl -o /usr/local/bin/gosu.asc -SL "https://github.com/tianon/gosu/releases/download/${GOSU_VERSION}/gosu-amd64.asc" \
            && gpg --verify /usr/local/bin/gosu.asc \
            && rm /usr/local/bin/gosu.asc \
            && rm -r /root/.gnupg/ \
            && chmod +x /usr/local/bin/gosu \
            && gosu nobody true

RUN yum -y --disablerepo="*" --enablerepo="epel" install python34

COPY entrypoint.sh /usr/local/bin/entrypoint.sh

RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
