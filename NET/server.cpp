
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8080

bool validateUser(const std::string& credentials) {
    // 简单的用户名和口令验证
    return credentials == "user:password";
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    if (listen(server_sock, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    std::cout << "Server is listening on port " << SERVER_PORT << std::endl;

    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len)) >= 0) {
        char buffer[1024] = {0};
        read(client_sock, buffer, 1024);

        std::string credentials(buffer);
        if (validateUser(credentials)) {
            std::cout << "User " << credentials << " logged in" << std::endl;
            send(client_sock, "Login successful", strlen("Login successful"), 0);

            while (true) {
                memset(buffer, 0, sizeof(buffer));
                int bytes_read = read(client_sock, buffer, 1024);
                if (bytes_read <= 0) break;
                std::cout << "Received: " << buffer << std::endl;
                send(client_sock, buffer, bytes_read, 0);
            }
        } else {
            std::cout << "Invalid user: " << credentials << std::endl;
            send(client_sock, "Login failed", strlen("Login failed"), 0);
        }

        close(client_sock);
    }

    close(server_sock);
    return 0;
}
