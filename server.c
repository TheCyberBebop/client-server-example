#include <time.h>
#include <sys/random.h>
#include <linux/limits.h>
#include <limits.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...)                                 \
    do                                                        \
    {                                                         \
        fprintf(stderr, "[DEBUG:%s] %s:%d: " fmt,             \
                __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
        fprintf(stderr, "\n");                                \
    } while (0)

#define DEBUG_ERROR(fmt, ...)                                 \
    do                                                        \
    {                                                         \
        fprintf(stderr, "[ERROR:%s] %s:%d: " fmt,             \
                __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
        fprintf(stderr, "\n");                                \
    } while (0)
#else
#define DEBUG_PRINT(fmt, ...) \
    do                        \
    {                         \
    } while (0)
#define DEBUG_ERROR(fmt, ...) \
    do                        \
    {                         \
    } while (0)
#endif

// Server IPv4 and port to bind/listen on
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 4444

// Max payload data chunk length
#define CHUNK_LEN 100

// Size of encryption MSG_KEY
#define KEY_LEN 32

// Maximum Client connections allowed
#define MAX_CONN 10

// Encryption key
char *gKey = NULL;

// Message ID ENUM
typedef enum
{
    MSG_KEY = 1,
    MSG_DATA,
    MSG_ERROR,
    MSG_END,
    MSG_CLOSE,
    MSG_TERMINATE
} MSG;

// Payload message (TCP data sent to Client)
typedef struct payload
{
    MSG id;
    int32_t size;
    char data[];
} payload;

// On-wire header (portable)
typedef struct __attribute__((packed)) payload_hdr
{
    uint8_t id;    // MSG as a single byte
    uint32_t size; // payload size in bytes, in network byte order
} payload_hdr;

// Payload queue Node
typedef struct pktNode
{
    payload *payload;
    struct pktNode *next;
} pktNode;

// Payload queue
typedef struct payloadQueue
{
    pktNode *front;
    pktNode *rear;
} payloadQueue;

// Function prototypes
void initServer(void) __attribute__((constructor));
void deinitServer(void) __attribute__((destructor));
void genKey(void) __attribute__((visibility("hidden")));
void cleanupPayloadPkt(payload **);
void deQueue(payloadQueue *);
void emptyQueue(payloadQueue *);
void encryptData(char *, int32_t);
void sendData(payload *, int32_t);
int32_t setupServerSock(void);
int32_t acceptConn(int32_t);
int32_t handleComms(int32_t);
int32_t enQueue(payloadQueue *);
int32_t processMsg(payload *, payloadQueue *);
int32_t sendKey(payloadQueue *);
int32_t readData(FILE *, payloadQueue *);
FILE *openFile(const char *, size_t);
payloadQueue *createQueue(void);

/********************************************************************************
 * @brief Generates a random Server encryption key.
 *
 * This function takes no parameters and generates a random encryption key of
 * length defined by KEY_LEN. The key is constructed by selecting random
 * characters from a predefined character set consisting of lowercase/uppercase
 * letters and digits. The generated key is assigned to the global variable gKey.
 ********************************************************************************/
void genKey(void)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "0123456789";
    
    // TODO: IMPLEMENT THIS

    // Generate random key
    // Randomly select an index from the character set
    // NULL terminate the generated key string

    DEBUG_PRINT("Generated KEY: %.*s", KEY_LEN, gKey);
}

/********************************************************************************
 * @brief Create a new payload queue for holding pktNode nodes.
 *
 * This function takes no parameters and allocates memory for a new payloadQueue
 * structure, initializes it to zero, and returns a pointer to the newly created
 * queue.
 *
 * @return Pointer on success, or NULL on failure.
 ********************************************************************************/
payloadQueue *createQueue(void)
{
    payloadQueue *q = NULL;

    // TODO: IMPLEMENT THIS

    // Allocate memory for the queue

    return q; // Return the pointer to the newly created queue
}

/********************************************************************************
 * @brief Create a new payload queue node.
 *
 * This function takes no parameters and allocates memory for a new pktNode,
 * initializes it, and returns the pointer to the newly created node.
 *
 * @return Pointer on success, or NULL on failure.
 ********************************************************************************/
pktNode *newNode(void)
{
    pktNode *node = NULL;

    // TODO: IMPLEMENT THIS

    return node; // Return the pointer to the newly created node
}

/********************************************************************************
 * @brief Add a new node to the rear of the payload queue.
 *
 * Allocates a new queue node, initializes it, and adds it to the end of the
 * payloadQueue. If the queue is empty, the new node becomes both the front
 * and rear of the queue.
 *
 * @param[in,out] q Pointer to the payloadQueue structure to which the node will
 *                  be added.
 *
 * @return 0 on success or -1 on failure.
 ********************************************************************************/
int32_t enQueue(payloadQueue *q)
{
    // TODO: IMPLEMENT THIS

    // Create and initialize a new node

    return 0;
}

/********************************************************************************
 * @brief Remove the front node from the payload queue and free its memory.
 *
 * This function removes the first node (front) from the queue, ensures that any
 * dynamically allocated memory for the payload is properly freed, and resets
 * the queue's front and rear pointers as needed. If the queue becomes empty
 * after the operation, the rear pointer is also set to NULL.
 *
 * @param[in,out] q Pointer to the payloadQueue structure to be modified.
 ********************************************************************************/
void deQueue(payloadQueue *q)
{
    // TODO: IMPLEMENT THIS

    DEBUG_PRINT("Front node successfully removed and memory freed");
}

/********************************************************************************
 * @brief Remove and free all nodes from the payload queue.
 *
 * This function iteratively removes all nodes from the payload queue, ensuring
 * that any memory dynamically allocated for payloads is properly freed. Once
 * the queue is emptied, the front and rear pointers are reset to NULL to
 * indicate the queue is empty.
 *
 * @param[in,out] q Pointer to the payloadQueue structure to be emptied.
 ********************************************************************************/
void emptyQueue(payloadQueue *q)
{
    // TODO: IMPLEMENT THIS

    // Dequeue all nodes and free memory

    DEBUG_PRINT("Payload queue successfully emptied and reset");
}

/********************************************************************************
 * @brief Handle sending an encryption key and termination signal to the client.
 *
 * This function enqueues two payloads into the queue: the first containing the
 * encryption key and the second marking the end of the transmission. It ensures
 * proper memory allocation and handles errors gracefully.
 *
 * @param[in,out] q Pointer to the payload queue.
 *
 * @return MSG_END on success, or MSG_TERMINATE on failure.
 ********************************************************************************/
int32_t sendKey(payloadQueue *q)
{
    // TODO: IMPLEMENT THIS

    // Add a new payload Node to the rear of the Queue
    // Allocate memory for the first payload (MSG_KEY)
    // Setup for MSG_KEY send
    // Add a new payload Node to the rear of the queue
    // Allocate memory payloadQueue node's payload
    // Setup for MSG_END send

    return MSG_END;
}

/********************************************************************************
 * @brief Open a file to read.
 *
 * This function validates and handles opening of a file name (full path) in
 * read mode.
 *
 * @param[in] file Name of file to open.
 * @param[in] len Size of file string.
 *
 * @return Valid file pointer or NULL on error.
 ********************************************************************************/
FILE *openFile(const char *name, size_t len)
{
    FILE *fp = NULL;

    // TODO: IMPLEMENT THIS

    // Basic sanity checks: no slashes, no "..", no embedded NULL
    // Open file in reading mode

    return fp;
}

/********************************************************************************
 * @brief Encrypt a chunk of memory using a simple encryption algorithm.
 *
 * This function applies "encryption" to the given data buffer. The encryption
 * algorithm is defined within this function and modifies the input buffer
 * directly. It assumes that the input data is non-null and the size parameter
 * is valid.
 *
 * @param[in,out] data Pointer to the data buffer to encrypt.
 * @param[in]     size The size of the data buffer in bytes.
 ********************************************************************************/
void encryptData(char *data, int32_t size)
{
    // TODO: IMPLEMENT THIS
}

/********************************************************************************
 * @brief Read the contents of a file and fill payload Node's data fields up to
 *        CHUNK_LEN.
 *
 * This function reads chunks of data from a file, creates payload nodes with
 * the data, encrypts the data, and appends the nodes to the provided payload
 * queue. If the end of the file is reached, an MSG_END payload is enqueued.
 *
 * @param[in] fp File pointer to read from.
 * @param[in,out] q Pointer to the payload queue where the payloads will be.
 *
 * @return MSG_END on success or MSG_ERROR on failure.
 ********************************************************************************/
int32_t readData(FILE *fp, payloadQueue *q)
{
    // TODO: IMPLEMENT THIS

    // Process the file contents in chunks
    // Add a new payload Node to the rear of the queue
    // Allocate memory for the payload
    // Initialize the payload ID and attempt to read data
    // Check for end-of-file or read error
    // Adjust payload allocation if less than CHUNK_LEN bytes were read
    // Set the payload size and encrypt the data

    return MSG_END;
}

/********************************************************************************
 * @brief Send all bytes from a buffer on a connected socket.
 *
 * This function attempts to send exactly len bytes from buf to the socket fd.
 * It repeatedly calls send() until all data is transmitted or an unrecoverable
 * error occurs.
 *
 * @param[in] fd Connected socket file descriptor.
 * @param[in] buf Pointer to the data buffer to be sent.
 * @param[in] len Number of bytes to send from buf.
 *
 * @return 0 on success, or -1 on failure.
 ********************************************************************************/
static int sendAll(int fd, const void *buf, size_t len)
{
    // TODO: IMPLEMENT THIS

    return 0; // success (or clean close)
}

/********************************************************************************
 * @brief Send a data chunk to the connected client over the provided socket.
 *
 * This function handles transmitting a payload structure and its associated data
 * to the client. It logs the payload details and continues execution even if
 * the send operation fails.
 *
 * @param[in] chunk  Pointer to the payload structure containing the data to send.
 * @param[in] connfd Connection file descriptor for communication with the client.
 ********************************************************************************/
void sendData(payload *chunk, int32_t connfd)
{
    // TODO: IMPLEMENT THIS

    // Build on-wire header
    // First send the header
    // Then send the payload data (if any)
}

/********************************************************************************
 * @brief Process a received message based on it's ID.
 *
 * This function processes the message from the client, handles different message
 * types, and manages the payload queue. It performs the required actions based
 * on the message type, including sending a key, reading data, or closing the
 * connection.
 *
 * Depending on the message id, the function will:
 * - Send the encryption key if the id is MSG_KEY.
 * - Open a file and process data if the id is MSG_DATA.
 * - Close the connection and clean up if the id is MSG_CLOSE.
 *
 * @param[in] payloadPkt The payload containing the message to be processed.
 * @param[in] payloadQueue The queue that holds payload data chunks.
 *
 * @return 0/ret/MSG_CLOSE on success, NSG_ERROR on failure.
 ********************************************************************************/
int32_t processMsg(payload *payloadPkt, payloadQueue *payloadQueue)
{
    // TODO: IMPLEMENT THIS

    // Process message based on its id

    return 0;
}

/********************************************************************************
 * @brief Clean up the memory used by a payload packet.
 *
 * This function frees the memory allocated for a payload packet and sets the
 * pointer to `NULL` to prevent dangling references.
 *
 * @param[in] ptr Double pointer of the payload structure to be cleaned up.
 ********************************************************************************/
void cleanupPayloadPkt(payload **ptr)
{
    DEBUG_PRINT("Cleaning up payload packet memory");
    if (NULL != *ptr)
    {
        free(*ptr);
        *ptr = NULL;
        DEBUG_PRINT("Payload packet memory successfully cleaned up");
    }
    else
    {
        DEBUG_PRINT("No payload packet to clean up (NULL pointer)");
    }
}

/********************************************************************************
 * @brief Handle communication with a client by processing incoming messages
 *        and sending responses using a payload queue.
 *
 * This function manages the reception and processing of client messages, handles
 * errors gracefully, and ensures all resources are cleaned up before exiting.
 *
 * @param[in] connfd Connection file descriptor for communication with the client.
 *
 * @return Message ID indicating the result.
 ********************************************************************************/
int32_t handleComms(int32_t connfd)
{
    int32_t ret = 0;
    int32_t received = 0;
    int32_t payloadHdrLen = (int32_t)sizeof(payload_hdr);
    ssize_t nread = 0;
    payload_hdr hdr;
    payload *payloadPkt __attribute__((cleanup(cleanupPayloadPkt))) = NULL;
    payloadQueue *payloadQueue = NULL;

    // Create payloadQueue
    payloadQueue = createQueue();
    if (NULL == payloadQueue)
    {
        DEBUG_PRINT("createQueue() failed");
        return MSG_TERMINATE;
    }

    // Allocate and initialize memory for receiving data
    payloadPkt = (payload *)calloc(1, sizeof(payload) + CHUNK_LEN);
    if (NULL == payloadPkt)
    {
        DEBUG_ERROR("calloc() failed");
        emptyQueue(payloadQueue);
        free(payloadQueue);
        return MSG_TERMINATE; // Bail and exit
    }

    // Process client messages until MSG_END or MSG_ERROR
    while (MSG_END != ret && MSG_ERROR != ret)
    {
        // Reset variables
        received = 0;
        memset(&hdr, 0, sizeof(hdr));

        // Read the payload header
        while (received < payloadHdrLen)
        {
            // Receive data from client
            nread = recv(connfd,
                         ((char *)&hdr) + received,
                         payloadHdrLen - received,
                         0);
            if (-1 == nread)
            {
                if (errno == EINTR)
                {
                    continue; // Interrupted, retry
                }

                DEBUG_ERROR("recv() failed while reading the payload header");
                emptyQueue(payloadQueue);
                free(payloadQueue);
                return MSG_TERMINATE; // Bail and exit
            }
            else if (0 == nread)
            {
                DEBUG_PRINT("connection closed by client during data recv()");
                emptyQueue(payloadQueue);
                free(payloadQueue);
                return MSG_CLOSE;
            }
            received += (int32_t)nread;
        }

        // Verify MSG_ID is valid
        if (MSG_KEY > hdr.id || MSG_TERMINATE < hdr.id)
        {
            DEBUG_ERROR("Invalid message id");
            emptyQueue(payloadQueue);
            free(payloadQueue);
            return MSG_TERMINATE; // Bail and exit
        }

        // Convert payload header to host order and store in payloadPkt
        payloadPkt->id = (MSG)hdr.id;
        payloadPkt->size = (int32_t)ntohl(hdr.size);

        // Validate size AFTER copying from hdr
        if (0 > payloadPkt->size || CHUNK_LEN < payloadPkt->size)
        {
            DEBUG_ERROR("Invalid payload size");
            emptyQueue(payloadQueue);
            free(payloadQueue);
            return MSG_TERMINATE; // Bail and exit
        }

        // Read payload data, if any
        if (0 < payloadPkt->size && MSG_CLOSE != payloadPkt->id)
        {
            received = 0; // Reset

            while (received < payloadPkt->size)
            {
                // Receive data from Client
                nread = recv(connfd,
                             ((char *)payloadPkt->data) + received,
                             payloadPkt->size - received,
                             0);
                if (-1 == nread)
                {
                    if (errno == EINTR)
                    {
                        continue; // Interrupted, retry
                    }

                    DEBUG_ERROR("recv() failed while reading the payload data");
                    emptyQueue(payloadQueue);
                    free(payloadQueue);
                    return MSG_TERMINATE; // Bail and exit
                }
                else if (0 == nread)
                {
                    DEBUG_PRINT("connection closed by client during data recv()");
                    emptyQueue(payloadQueue);
                    free(payloadQueue);
                    return MSG_CLOSE;
                }
                received += (int32_t)nread;
            }
        }
        else // No payload data
        {
            memset(payloadPkt->data, 0, CHUNK_LEN);
        }

        DEBUG_PRINT("Payload received --> msg id : %d", payloadPkt->id);

        // Process the received message
        ret = processMsg(payloadPkt, payloadQueue);
        if (MSG_CLOSE == ret || MSG_TERMINATE == ret)
        {
            // Handle the MSG_CLOSE message, empty the queue and clean up
            emptyQueue(payloadQueue);
            break;
        }
    }

    // Setup MSG_ERROR payload and send to client
    if (MSG_ERROR == ret || MSG_TERMINATE == ret)
    {
        DEBUG_PRINT("Preparing MSG_ERROR payload");

        void *temp = realloc(payloadPkt, sizeof(payload));
        if (NULL == temp)
        {
            DEBUG_ERROR("realloc() failed");
            emptyQueue(payloadQueue);
            free(payloadQueue);
            return MSG_TERMINATE;
        }

        payloadPkt = (payload *)temp;
        payloadPkt->id = ret;
        payloadPkt->size = 0;

        sendData(payloadPkt, connfd);
        emptyQueue(payloadQueue);
    }

    // Send queued payload data to the client
    while (NULL != payloadQueue->front)
    {
        sendData(payloadQueue->front->payload, connfd);
        deQueue(payloadQueue);
    }

    // Free the payload queue
    if (NULL != payloadQueue)
    {
        free(payloadQueue);
    }

    return ret;
}

/********************************************************************************
 * @brief Accepts a client connection.
 *
 * Waits for a client connection on the specified server socket (sockfd) and
 * accepts a single client connection.
 *
 * @param[in] sockfd The server socket file descriptor.
 *
 * @return A valid socket file descriptor on success, or -1 on failure.
 ********************************************************************************/
int32_t acceptConn(int32_t sockfd)
{
    int32_t connfd = -1;
    struct sockaddr_in connAddr;
    socklen_t addrLen = 0;

    // TODO: IMPLEMENT THIS

    // Initialize connAddr structure for the accept() call
    // Accept a client connection

    return connfd;
}

/********************************************************************************
 * @brief Set up the server socket using SERVER_IP and SERVER_PORT.
 *
 * This function takes no parameters and performs the necessary steps to create,
 * configure, and bind a IPv4 TCP socket for use as a server socket, except for
 * the "accept()" call.
 *
 * @return A valid socket file descriptor on success, or -1 on failure.
 ********************************************************************************/
int32_t setupServerSock(void)
{
    int32_t sockfd = -1;
    const int32_t enable = 1;
    struct sockaddr_in servaddr;

    // TODO: IMPLEMENT THIS

    // Create a TCP socket file descriptor
    // Set socket option SO_REUSEADDR
    // Setup IPv4, IP, and PORT
    // Bind the socket
    // Listen on the socket

    return sockfd;
}

/********************************************************************************
 * @brief Top-level logic and flow for the server.
 *
 * This function takes no parameters and calls various helper functions for setup
 * and loops on handleComms() until a MSG_CLOSE payload is received from the
 * client.
 *
 * @return EXIT_SUCCESS on success or EXIT_FAILURE on failure.
 ********************************************************************************/
int32_t main(void)
{
    int32_t connfd = -1;
    int32_t sockfd = -1;
    int32_t ret = 0;

    // Should never hit -- safe guard
    if (NULL == gKey)
    {
        DEBUG_PRINT("initServer() failed");
        return EXIT_FAILURE;
    }

    // Initialize and configure the server socket
    sockfd = setupServerSock();
    if (-1 == sockfd)
    {
        DEBUG_PRINT("setupServerSock() failed");
        return EXIT_FAILURE;
    }

    // Accept an incoming client connection
    connfd = acceptConn(sockfd);
    if (-1 == connfd)
    {
        DEBUG_PRINT("acceptConn() failed");
        close(sockfd);
        return EXIT_FAILURE;
    }

    // Handle server communications
    do
    {
        ret = handleComms(connfd);
        if (MSG_CLOSE == ret)
        {
            close(connfd);
            connfd = -1;
#ifdef DEBUG
            break; // Server will exit in debug mode
#endif
            connfd = acceptConn(sockfd);
            if (-1 == connfd)
            {
                DEBUG_PRINT("acceptConn() failed");
                break;
            }
        }
        else if (MSG_TERMINATE == ret)
        {
            DEBUG_PRINT("handleComms() failed");
            break;
        }
    } while (1);

    DEBUG_PRINT("\n\n\t\tGOODBYE!!\n");

    // Clean up
    if (-1 != connfd)
    {
        close(connfd);
    }
    close(sockfd);

    return EXIT_SUCCESS;
}

/********************************************************************************
 * @brief Generates the Server's encryption key.
 *
 * This function takes no parameters and is responsible for generating a random
 * encryption key prior to the program calling main(). If key generation fails,
 * the program ends with EXIT_FAILURE.
 ********************************************************************************/
void initServer(void)
{
    DEBUG_PRINT("Generating random encryption key");

    // Seed at startup
    unsigned int seed = 0;

    // Linux only -- kernel entropy
    if ((ssize_t)sizeof(seed) != getrandom(&seed, sizeof(seed), 0))
    {
        DEBUG_ERROR("getrandom() failed!");
        exit(EXIT_FAILURE);
    }

    srand(seed);

    genKey();
    if (NULL == gKey)
    {
        DEBUG_ERROR("genKey() failed!");
        exit(EXIT_FAILURE);
    }
}

/********************************************************************************
 * @brief Cleans up encryption key memory.
 *
 * This function takes no parameters and is responsible for deallocating the
 * memory the global encryption key `gKey` and sets the pointer to NULL.
 ********************************************************************************/
void deinitServer(void)
{
    DEBUG_PRINT("Cleaning up encryption key memory");

    if (NULL != gKey)
    {
        free(gKey);
        gKey = NULL;
    }
}
