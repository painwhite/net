
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

void sendFile(int sock, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        send(sock, line.c_str(), line.size(), 0);
    }
    file.close();
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    std::string username, password;
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;

    std::string credentials = username + ":" + password;
    send(sock, credentials.c_str(), credentials.size(), 0);

    std::string choice;
    std::cout << "Send file or text (file/text): ";
    std::cin >> choice;

    if (choice == "file") {
        std::string filename;
        std::cout << "Enter filename: ";
        std::cin >> filename;
        sendFile(sock, filename.c_str());
    } else {
        std::string text;
        std::cout << "Enter text: ";
        std::cin.ignore();
        std::getline(std::cin, text);
        send(sock, text.c_str(), text.size(), 0);
    }

    close(sock);
    return 0;
}
