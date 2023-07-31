#include "loginSystem.h"



// Function to generate a salt value
std::string loginSystem::generate_salt() {
    unsigned char salt[16];
    RAND_bytes(salt, sizeof(salt));

    std::stringstream ss;
    for (int i = 0; i < sizeof(salt); i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(salt[i]);
    }

    return ss.str();
}

// Function to perform PBKDF2 hashing
std::string loginSystem::pbkdf2(const std::string& password, const std::string& salt) {
    //variable declaration
    const EVP_MD* md = EVP_sha256();
    const unsigned int iterations = 10000;
    const unsigned int key_length = 32;
    unsigned char key[key_length];

    // Calculate the PBKDF2 hash
    PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), reinterpret_cast<const unsigned char*>(salt.c_str()), salt.length(), iterations, md, key_length, key);

    // Convert the key to a hexadecimal string
    std::stringstream ss;
    for (int i = 0; i < key_length; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(key[i]);
    }

    return ss.str();
}