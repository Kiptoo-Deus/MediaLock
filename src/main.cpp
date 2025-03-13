#include <iostream>
#include <fstream>
#include <string>

void createSampleMedia(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "This is a sample video file content.\n";
        file.close();
        std::cout << "Sample media created: " << filename << "\n";
    } else {
        std::cerr << "Error creating sample media.\n";
    }
}

std::string readMedia(const std::string& filename) {
    std::ifstream file(filename);
    std::string content;
    if (file.is_open()) {
        std::getline(file, content, '\0'); // Read entire file
        file.close();
    }
    return content;
}

int main() {
    std::cout << "MediaLock DRM Simulator v1.0\n";
    createSampleMedia("sample_video.txt");
    std::string content = readMedia("sample_video.txt");
    std::cout << "Media content: " << content << "\n";
    return 0;
}