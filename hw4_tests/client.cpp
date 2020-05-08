#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

#define PORT 12345

const int num_buckets = 32;
const char *server_name;
struct sockaddr_in serv_addr;
struct hostent *server;

void send_req(int sockfd, const char *buff, int len) {
  int count = 0;
  do {
    count += send(sockfd, (buff+count), (len-count), 0);
  } while (count != len);
}

int recv_resp(int sockfd, char *buff, int len) {
  int resp, count = 0;
  do {
    count += read(sockfd, (buff+count), (len-count));
  } while (buff[count-1] != '\n');

  buff[count-1] = 0;
  return (atoi(buff));
}

int new_conn() {
  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "\n Socket creation error \n");
    return -1;
  }
  
  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "\nConnection Failed\n");
    return -1;
  }
  return sock;
}

void check_test(int test, int *expected, int *observed)
{
  int i, passed = 1;
  for (i=0; i < num_buckets; i++) {
    if (observed[i] != expected[i]) {
      passed = 0;
    }
  }
  if (passed) {
    printf("Test %d passed.\n", test);
  } else {
    printf("Test 0 failed.\n  Expected: ");
    for (i=0; i < num_buckets; i++) {
      printf("%d, ", expected[i]);
    }
    printf("\n Observed: ");
    for (i=0; i < num_buckets; i++) {
      printf("%d, ", observed[i]);
    }
    printf("\n");
  }
}

void run_test0()
{
  int expected[num_buckets] = {0};
  int observed[num_buckets] = {0};

  int clientfd = new_conn();
  if (clientfd == -1) return;

  printf("Running test 0...\n");
  
  char req[] = "3,5\n";
  send_req(clientfd, req, strlen(req));

  char resp[32]={'\0'};
  int value = recv_resp(clientfd, resp, 32);
  expected[5] = 3;
  observed[5] = value;
  close(clientfd);
  
  check_test(0, expected, observed);
}

void run_test1()
{
  int i;
  int num_reqs = 1000;
  int bucket_to_use = 12;
  int expected[num_buckets] = {0};
  int observed[num_buckets] = {0};

  
  srand(0);
  printf("Running test 1...\n");

  for (i=0; i < num_reqs; i++) {
    int clientfd = new_conn();
    if (clientfd == -1) return;

    int delay = (i < 5) ? i : 0;
    expected[bucket_to_use] += delay;

    std::stringstream ss;
    ss << delay << "," << bucket_to_use << "\n";
    send_req(clientfd, ss.str().c_str(), ss.str().length());
    
    char resp[32]={'\0'};
    int value = recv_resp(clientfd, resp, 32);
    observed[bucket_to_use] = value;
    close(clientfd);
  }

  check_test(1, expected, observed);
}

void run_test2()
{
  int i;
  int num_reqs = 1000;
  int expected[num_buckets] = {0};
  int observed[num_buckets] = {0};
  
  srand(0);
  printf("Running test 2...\n");

  for (i=0; i < num_reqs; i++) {
    int clientfd = new_conn();
    if (clientfd == -1) return;
    
    int r = rand() % num_buckets;
    int d = (i < 10) ? 3 : 0;
    expected[r] += d;

    std::stringstream ss;
    ss << d << "," << r << "\n";
    send_req(clientfd, ss.str().c_str(), ss.str().length());
    
    char resp[32]={'\0'};
    int value = recv_resp(clientfd, resp, 32);
    observed[r] = value;
    close(clientfd);
  }

  check_test(2, expected, observed);
}

void run_test3()
{
  int i;
  int num_reqs = 50;
  int expected[num_buckets] = {0};
  int observed[num_buckets] = {0};
  
  srand(0);
  printf("Running test 3...\n");

  for (i=0; i < num_reqs; i++) {
    int clientfd = new_conn();
    if (clientfd == -1) return;

    int r = rand() % num_buckets;
    int d = rand() % 3;
    expected[r] += d;

    std::stringstream ss;
    ss << d << "," << r << "\n";
    send_req(clientfd, ss.str().c_str(), ss.str().length());
    
    char resp[32]={'\0'};
    int value = recv_resp(clientfd, resp, 32);
    observed[r] = value;
    close(clientfd);
  }

  check_test(3, expected, observed);
}

struct tdata_t {
  int clientfd;
  int bucket;
  int delay;
};

void *processReq(void *args) {
  struct tdata_t *tdata = (tdata_t*)args;
  int clientfd = tdata->clientfd;
  int bucket = tdata->bucket;
  int delay = tdata->delay;
  
  std::stringstream ss;
  ss << delay << "," << bucket << "\n";
  send_req(clientfd, ss.str().c_str(), ss.str().length());
  
  char resp[32]={'\0'};
  int value = recv_resp(clientfd, resp, 32);

  close(clientfd);
  free(tdata);
}

void run_test4()
{
  int i;
  int num_reqs = 1000;
  int expected[num_buckets] = {0};
  int observed[num_buckets] = {0};
  int bucket[num_reqs];
  int delay[num_reqs];
  
  srand(0);
  printf("Running test 4...\n");

  for (i=0; i < num_reqs; i++) {
    bucket[i] = rand() % num_buckets;
    delay[i] = 1;
    expected[bucket[i]]++;    
  }
  
  pthread_t *threads = (pthread_t*)malloc(num_reqs * sizeof(pthread_t));
  for (i=0; i < num_reqs; i++) {
    struct tdata_t *args = (struct tdata_t*)malloc(sizeof(tdata_t));
    int clientfd = new_conn();
    if (clientfd == -1) return;

    args->clientfd = clientfd;
    args->bucket = bucket[i];
    args->delay = delay[i];

    pthread_create(&threads[i], NULL, processReq, (void*)args);
  }

  for (i=0; i < num_reqs; i++) {
    pthread_join(threads[i], NULL);
  }

  //One last req to each bucket to get final values
  for (i=0; i < num_buckets; i++) {
    int clientfd = new_conn();
    if (clientfd == -1) return;

    int r = i;
    int d = 0;

    std::stringstream ss;
    ss << d << "," << r << "\n";
    send_req(clientfd, ss.str().c_str(), ss.str().length());
    
    char resp[32]={'\0'};
    int value = recv_resp(clientfd, resp, 32);
    observed[r] = value;
    //expected[r]++;
    close(clientfd);
  }

  check_test(4, expected, observed);
  free(threads);
}

int main(int argc, char const *argv[])
{
  int sock = 0, valread;
  int test_id;
  std::string hello = "10,5\n";

  if (argc < 3) {
    fprintf(stderr, "Usage: ./client <test_id> <server>\n");
    return -1;
  }
  test_id = atoi(argv[1]);
  server_name = argv[2];
  
  printf("Connecting to server at %s\n", server_name);
  server = gethostbyname(server_name);
  if (server == NULL) {
    fprintf(stderr, "\nNo such host.\n");
    return -1;
  }

  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(PORT);

  switch(test_id) {
  case 0:
    run_test0();
    break;
  case 1:
    run_test1();
    break;
  case 2:
    run_test2();
    break;
  case 3:
    run_test3();
    break;
  case 4:
    run_test4();
    break;
  default:
    fprintf(stderr, "Unknown test_id (%d) specified\n", test_id);
    return -1;
  }
    
  return 0;
} 
