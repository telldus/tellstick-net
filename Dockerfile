FROM debian:jessie

MAINTAINER Micke Prag <micke.prag@telldus.se>

RUN apt-get update -yq && \
    apt-get install -y unzip wget cmake

RUN wget http://ww1.microchip.com/downloads/en/DeviceDoc/picc-18-pro_9.66.8870-linux.zip && \
    unzip picc-18-pro_9.66.8870-linux.zip && \
    rm picc-18-pro_9.66.8870-linux.zip && \
    chmod +x picc-18-pro_9.66.8870-linux.run && \
    ./picc-18-pro_9.66.8870-linux.run --noexec && \
    rm picc-18-pro_9.66.8870-linux.run && \
    cd picc-18-pro_9.66.8870-linux && \
    mkdir -p /usr/hitech/picc-18/pro/9.66 && \
    cp -R ./* "/usr/hitech/picc-18/pro/9.66" && \
    cd && \
    rm -r /picc-18-pro_9.66.8870-linux && \
    /usr/hitech/picc-18/pro/9.66/bin/picc18 --setup="/usr/hitech/picc-18/pro/9.66"
