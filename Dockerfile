from ubuntu:19.04
RUN apt-get update -y
RUN apt-get install build-essential -y 
RUN apt-get install gcc-9 -y 
RUN apt-get install python -y 
RUN apt-get install xxd -y 
RUN mkdir ./SHELF
COPY ./ ./SHELF
WORKDIR ./SHELF
RUN make 
RUN ./loader
RUN sleep 1000

