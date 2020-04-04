# erss-hwk4-zl246-gc171


## lib

sudo apt-get install libboost-all-dev

### [threadpool](https://github.com/vit-vit/CTPL)

## [cores](https://www.howtoforge.com/linux-taskset-command/)

find the PID 

```
ps -ef | grep ./server
```

through mask

```
taskset -p PID
```

change core through mask
```
taskset -p 0x (mask) PID
``` 

specify 
```
taskset -p PID  --cpu-list 0-2,6
```
is processors #0, #1, #2, and #6.

## testing 

* random vs certain bucket
* small(1 - 3) vs large(1 - 20) delay time
* different bucket size (i.e. 32/128/512/2048)
* different request at same time

(a) Two threading strategies: (1) create per request and (2) pre-create.

(b) The throughput of your server code when running with 1, 2, and 4 cores available (this is the performance scalability of the code).

(c) Small vs. large variations in delay count (with 4 cores active). For small delay count variability, you might try delays of 1-3 seconds. For large delay count variability, you might try delays of 1-20 seconds. 

(d) Different bucket sizes (32, 128, 512, 2048) buckets (with 4 cores active).

 

You'll do the combinations of (a), (b).  So that would be 6 data points.  You could pick any delay range (e.g. the 1-3 second small delay count) and bucket size (e.g. 512) that you like.

Then with 4 cores active only you'll test (a) and (c).  That would be 4 data points

Then with 4 cores and small delay count, you can test (a) and (d).  That would be 8 data points.