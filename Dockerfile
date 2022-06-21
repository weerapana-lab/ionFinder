from python:3.9

MAINTAINER "Aaron Maurais -- Weerapana Lab"

# initialize
RUN apt-get update && \
    apt-get -y install cmake r-base man less && \
    R -e "install.packages(c('Rcpp', 'ggplot2'))" && \
    mkdir -p /code/envoMatch /code/ionFinder/ionFinder /data

# install envoMatch
RUN git clone https://github.com/ajmaurais/envoMatch /code/envoMatch 
RUN cd /code/envoMatch && \
    python setup.py build && \
    pip install .

# install ionFinder
COPY ./ /code/ionFinder/ionFinder
RUN cd /code/ionFinder && \
    mkdir build && \
    cd build && \
    cmake ../ionFinder && \
    make && \
    cp -v bin/* /usr/local/bin

# build ms2 plotting stuff
RUN cd /code/ionFinder/ionFinder && mkdir lib && \
    Rscript rpackages/install_packages.R

WORKDIR /data

