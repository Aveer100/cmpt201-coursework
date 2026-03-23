#define LISTEN_BACKLOG 32
 13
 14 #define handle_error(msg)                                                      \
 15 ┆ do {                                                                         \
 16 ┆ ┆ perror(msg);                                                               \
 17 ┆ ┆ exit(EXIT_FAILURE);                                                        \
 18 ┆ } while (0)
 19
 20 // Shared counters for: total # messages, and counter of clients (used for
 21 // assigning client IDs)
 22 int total_message_count = 0;
 23 int client_id_counter = 1;
 24
 25 // Mutexs to protect above global state.
 26 pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
 27 pthread_mutex_t client_id_mutex = PTHREAD_MUTEX_INITIALIZER;
 28
 29 struct client_info {
 30 ┆ int cfd;
 31 ┆ int client_id;
 32 };
 33
 34 void *handle_client(void *arg) {
 35 ┆ pthread_mutex_lock(&count_mutex);
 36 ┆ struct client_info *client = (struct client_info *)arg;
 37 ┆
 38 ┆ // TODO: print the message received from client
 39 ┆ // TODO: increase total_message_count per message
 40 ┆ char temp[BUF_SIZE];
 41 ┆ ssize_t val;
 42 ┆ while((val = read(client->cfd, temp, BUF_SIZE - 1)))
 43 ┆ {
 44 ┆ ┆ temp[val] = '\0';
 45 ┆ ┆ total_message_count++;
 46 ┆ ┆ int new_num = total_message_count;
 47 ┆ ┆ printf("Msg # %d; Client ID %d: %s", new_num, client->client_id, temp);
 48 ┆ }
 49 ┆ printf("client %d is done.\n", client->client_id);
 50 ┆ close(client->cfd);
 51 ┆ free(client);
 52 ┆ pthread_mutex_unlock(&count_mutex);
 53 ┆ return NULL;
 54 }

 55
 56 int main() {
 57 ┆ struct sockaddr_in addr;
 58 ┆ int sfd;
 59 ┆
 60 ┆ sfd = socket(AF_INET, SOCK_STREAM, 0);
 61 ┆ if (sfd == -1) {
 62 ┆ ┆ handle_error("socket");
 63 ┆ }
 64 ┆
 65 ┆ memset(&addr, 0, sizeof(struct sockaddr_in));
 66 ┆ addr.sin_family = AF_INET;
 67 ┆ addr.sin_port = htons(PORT);
 68 ┆ addr.sin_addr.s_addr = htonl(INADDR_ANY);
 69 ┆
 70 ┆ if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
 71 ┆ ┆ handle_error("bind");
 72 ┆ }
 73 ┆
 74 ┆ if (listen(sfd, LISTEN_BACKLOG) == -1) {
 75 ┆ ┆ handle_error("listen");
 76 ┆ }
 77 ┆
 78 ┆ for (;;) {
 79 ┆ ┆ pthread_mutex_lock(&client_id_mutex);
 80 ┆ ┆ // TODO: create a new thread when a new connection is encountered
 81 ┆ ┆ // TODO: call handle_client() when launching a new thread, and provide
 82 ┆ ┆ struct sockaddr_in location;
 83 ┆ ┆ socklen_t len = sizeof(location);
 84 ┆ ┆ int num = accept(sfd, (struct sockaddr *)&location, &len);
 85 ┆ ┆ struct client_info* stats = malloc(sizeof(struct client_info));
 86 ┆ ┆ stats->cfd = num;
 87 ┆ ┆ stats->client_id = client_id_counter++;
 88 ┆ ┆ printf("New client created! ID %d on socket FD %d\n", stats->client_id, stats->cfd);
 89 ┆ ┆ pthread_t my_thread;
 90 ┆ ┆ pthread_create(&my_thread, NULL, handle_client, stats);
 91 ┆ ┆ pthread_detach(my_thread);
 92 ┆ ┆ pthread_mutex_unlock(&client_id_mutex);
 93 ┆ }
 94 ┆
 95 ┆ if (close(sfd) == -1) {
 96 ┆ ┆ handle_error("close");
 97 ┆ }
 98 ┆
 99 ┆ return 0;
100 }


#define PORT 8000
 22 #define BUF_SIZE 64
 23 #define ADDR "127.0.0.1"
 24
 25 #define handle_error(msg)                                                      \
 26 ┆ do {                                                                         \
 27 ┆ ┆ perror(msg);                                                               \
 28 ┆ ┆ exit(EXIT_FAILURE);                                                        \
 29 ┆ } while (0)
 30
 31 int main() {
 32 ┆ struct sockaddr_in addr;
 33 ┆ int sfd;
 34 ┆ ssize_t num_read;
 35 ┆ char buf[BUF_SIZE];
 36 ┆
 37 ┆ sfd = socket(AF_INET, SOCK_STREAM, 0);
 38 ┆ if (sfd == -1) {
 39 ┆ ┆ handle_error("socket");
 40 ┆ }
 41 ┆
 42 ┆ memset(&addr, 0, sizeof(struct sockaddr_in));
 43 ┆ addr.sin_family = AF_INET;
 44 ┆ addr.sin_port = htons(PORT);
 45 ┆ if (inet_pton(AF_INET, ADDR, &addr.sin_addr) <= 0) {
 46 ┆ ┆ handle_error("inet_pton");
 47 ┆ }
 48 ┆
 49 ┆ int res = connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
 50 ┆ if (res == -1) {
 51 ┆ ┆ handle_error("connect");
 52 ┆ }
 53 ┆
 54 ┆ while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 1) {
 55 ┆ ┆ if (write(sfd, buf, num_read) != num_read) {
 56 ┆ ┆ ┆ handle_error("write");
 57 ┆ ┆ }
 58 ┆ ┆ printf("Just sent %zd bytes.\n", num_read);
 59 ┆ }
 60 ┆
 61 ┆ if (num_read == -1) {
 62 ┆ ┆ handle_error("read");
 63 ┆ }
 64 ┆
 65 ┆ close(sfd);
 66 ┆ exit(EXIT_SUCCESS);
 67 }
