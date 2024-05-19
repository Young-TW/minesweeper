#include "network.h"

#include <unistd.h>
#include "gl.h"
#include <vector>
#include <cstring>

int game_interaction(int socket_fd, game_data* data, Board* board) {
    // 接收: recv(int sockfd, void *buf, size_t len, int flags)
    // -> recv(socket_fd, RECV_MESSENGE, sizeof(RECV_MESSENGE), 0);
    // 傳送: send(int sockfd, const void *buf, size_t len, int flags)
    // -> send(socket_fd, MESSENGE, sizeof(MESSENGE), 0);
    // flags填入0即可
    while (data -> game_status) {
        char buffer[sizeof(game_data)];
        
        if (recv(socket_fd, buffer, sizeof(buffer), 0) <= 0) {
            close(socket_fd);
            return -1;
        }
        
        std::memcpy(data, buffer, sizeof(game_data));
        
        // 遊戲互動環節
        if (data -> action == REVEAL) {
            board -> reveal(board -> blocks[data-> block_index]);
        } else if (data -> action == FLAG) {
            board -> flagged(board -> blocks[data -> block_index]);
        } else if (data -> action == REMOVE_FLAG) {
            board -> remove_flagged(board -> blocks[data -> block_index]);
        }
    }
    close(socket_fd);
    return 0;
}

int host_game(u_int16_t port, int max_member) {
    max_member -= 1;  // exclude host

    // AF_INET: use IPv4
    // SOCK_DGRAM: UDP, SOCK_STREAM: TCP
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_address;

    if (socket_fd < 0) {
        return SOCKET_CREATE_FAILED;
    }

    // server connection information
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; // vaild address accept
    server_address.sin_port = htons(port); // use specify (include) port

    // bind port on host
    if (bind(socket_fd, (const struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        close(socket_fd);
        return BIND_FAILED;
    }

    // listen init
    if (listen(socket_fd, max_member) == SOCKET_CREATE_FAILED) {
        close(socket_fd);
        return LISTEN_ERROR;
    }

    if (close(socket_fd) < 0) {
        return SOCKET_CLOSE_ERROR;
    }

    // client connection information
    int reply_sockfd;
    sockaddr_in client_address;
    int client_index = 0;
    unsigned int client_len = sizeof(client_address);

   GL gl;
    bool game_started = false;

    while (!game_started) {
        reply_sockfd = accept(socket_fd, (struct sockaddr *)&client_address, &client_len);
    }

    // init board
    Board game(8, 8, 10);
    gl.init_board(game);
    // send board information to client
    send(socket_fd, &game, sizeof(game), 0);
    // need consider client accept later disconnect

    // 遊戲互動環節
    game_data data;
    std::thread(game_interaction, socket_fd, &data, &game).detach();

    if (close(socket_fd) < 0) {
        return SOCKET_CLOSE_ERROR;
    }

    return SUCESS;
}

int join_game(uint32_t host_address, uint16_t host_port) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        return SOCKET_CREATE_FAILED;
    }

    // server information
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = host_address;
    server_address.sin_port = htons(host_port);
    int server_len = sizeof(server_address);

    // try connect to server
    if (connect(socket_fd, (struct sockaddr *)&server_address, server_len) == SOCKET_CREATE_FAILED) {
        close(socket_fd);
        return CONNECT_FAILED;
    }

    Board board(8, 8, 10);
    char board_buffer[sizeof(Board)];
    ssize_t bytes_received = recv(socket_fd, board_buffer, sizeof(board_buffer), 0);
    if (bytes_received != sizeof(board_buffer)) {
        close(socket_fd);
        return MESSENGE_RECV_ERROR;
    }
    std::memcpy(&board, board_buffer, sizeof(Board));

    game_data data;
    std::thread(game_interaction, socket_fd, &data, &board).detach();

    if (close(socket_fd) < 0) {
        return SOCKET_CLOSE_ERROR;
    }

    return SUCESS;
}