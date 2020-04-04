# erss-hwk4-zl246-gc171

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

```shell
./server 0(per create)/1(pre create i.e. thread pool) bucket_num
```

then run

```shell
sudo docker-compose up --build
```

## [threadpool](https://github.com/vit-vit/CTPL)
