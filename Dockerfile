FROM gcc:10
WORKDIR /app/
COPY opg.c ./
RUN gcc opg.c -o program
RUN chmod +x program