# erss-hwk4-zl246-gc171

## testing data

Testing Data.xlsx

## testing instruction

### client

build client

``` shell
cd testing && chmod +x build.sh && ./build.sh
```

run client, we provide 2 test, short delay: test, long delay: test_long_delay

```shell
./test thread_number
```

### server(in docker)

go to yml file, find command

```shell
taskset --cpu-list 0
```

0 means core you want program run on

if you need run on 4 cores, then use
```shell
taskset --cpu-list 0-3
```

the server takes two parameters, 
the first indicates the thread strategy:
0: per thread per request
1: pre-created thread, i.e., thread pool

the second parameter indicates the size of bucket.

e.g., run the server without thread pool and a bucket of size 512
```shell
./server 0 512
```

then run

```shell
sudo docker-compose up --build
```

## [threadpool](https://github.com/vit-vit/CTPL)
