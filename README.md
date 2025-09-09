# Challenge-Response Protocol Implementation

A secure communication protocol implementation using cryptographic primitives including SHA-256 hashing, HMAC-SHA256 authentication, and XOR encryption.

## 🔐 Overview

This project implements a **Challenge-Response Protocol** where Alice and Bob communicate securely using shared secret keys, counters, and nonces to prevent replay attacks and ensure message integrity.

## ✨ Features

- **Secure Communication**: XOR encryption with SHA-256 derived keys
- **Message Authentication**: HMAC-SHA256 for integrity verification
- **Replay Attack Prevention**: Counter-based state management
- **Message Freshness**: Nonce-based freshness verification
- **Error Handling**: Graceful handling of missing files and invalid inputs
- **Cross-Platform**: Works on macOS, Linux, and Windows

## 🚀 Quick Start

### Prerequisites

- GCC compiler
- OpenSSL development libraries
- macOS/Linux/Windows environment

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/sfwani/Challenge-Response-Protocol.git
   cd Challenge-Response-Protocol
   ```

2. **Install OpenSSL** (if not already installed)
   ```bash
   # macOS
   brew install openssl
   
   # Ubuntu/Debian
   sudo apt-get install libssl-dev
   
   # CentOS/RHEL
   sudo yum install openssl-devel
   ```

3. **Compile the programs**
   ```bash
   # macOS
   gcc -I/opt/homebrew/include -L/opt/homebrew/lib alice.c -lssl -lcrypto -o alice
   gcc -I/opt/homebrew/include -L/opt/homebrew/lib bob.c -lssl -lcrypto -o bob
   
   # Linux
   gcc alice.c -lssl -lcrypto -o alice
   gcc bob.c -lssl -lcrypto -o bob
   ```

## 📋 Usage

### Basic Protocol Flow

The protocol follows a three-step process:

1. **Alice creates challenge** → Bob processes challenge → Alice verifies response

### Step-by-Step Execution

1. **Create test files** (see [Testing Guide](TESTING_GUIDE.md))
   ```bash
   echo -n "Your32ByteMessageHere123456789" > Message.txt
   echo -n "YourSecretKey" > SharedKey.txt
   echo "1" > A_ctr.txt && echo "1" > B_ctr.txt
   echo "100" > A_nonce.txt && echo "100" > B_nonce.txt
   ```

2. **Run Alice** (creates challenge)
   ```bash
   ./alice Message.txt SharedKey.txt A_ctr.txt A_nonce.txt
   ```

3. **Run Bob** (processes challenge)
   ```bash
   ./bob Ciphertext.txt Signature.txt SharedKey.txt B_ctr.txt B_nonce.txt
   ```

4. **Run Alice again** (verifies response)
   ```bash
   ./alice Message.txt SharedKey.txt A_ctr.txt A_nonce.txt
   ```

### Expected Output

After successful execution, you'll see:
- `Key.txt` - Shared key in hex format
- `Ciphertext.txt` - Encrypted message
- `Signature.txt` - HMAC signature
- `Response.txt` - Bob's response
- `Acknowledgment.txt` - "Acknowledgment Successful"

## 🔧 Protocol Details

### Alice's Process
1. Reads message, shared key, counter, and nonce
2. Writes key in hex format to `Key.txt`
3. Encrypts message: `c = m ⊕ H(k || ctr)`
4. Signs ciphertext: `sig = HMAC_k(c || nonce)`
5. Waits for Bob's response
6. Verifies response and writes acknowledgment
7. Updates counter and nonce

### Bob's Process
1. Reads ciphertext, signature, shared key, counter, and nonce
2. Verifies Alice's signature
3. Decrypts ciphertext: `m = c ⊕ H(k || ctr)`
4. Computes response: `response = H(m || (ctr+1) || (nonce+1))`
5. Updates counter and nonce

## 📁 Project Structure

```
Challenge-Response-Protocol/
├── alice.c                    # Alice's implementation
├── bob.c                      # Bob's implementation
├── RequiredFunctionsHW1.c     # Utility functions template
├── test_cases/                # Test data directory
│   ├── Message1.txt           # Sample message
│   ├── SharedKey1.txt         # Sample shared key
│   ├── CorrectA_ctr1.txt      # Alice's counter
│   ├── CorrectA_nonce1.txt    # Alice's nonce
│   ├── CorrectB_ctr1.txt      # Bob's counter
│   ├── CorrectB_nonce1.txt    # Bob's nonce
│   └── VerifyingCRP.sh        # Verification script
├── PROJECT_DOCUMENTATION.md   # Detailed documentation
├── TESTING_GUIDE.md          # Testing instructions
└── README.md                 # This file
```

## 🧪 Testing

### Using Provided Test Cases
```bash
# Copy test files to current directory
cp test_cases/Message1.txt test_cases/SharedKey1.txt .
cp test_cases/CorrectA_ctr1.txt A_ctr.txt
cp test_cases/CorrectA_nonce1.txt A_nonce.txt
cp test_cases/CorrectB_ctr1.txt B_ctr.txt
cp test_cases/CorrectB_nonce1.txt B_nonce.txt

# Run the protocol
./alice Message1.txt SharedKey1.txt A_ctr.txt A_nonce.txt
./bob Ciphertext.txt Signature.txt SharedKey1.txt B_ctr.txt B_nonce.txt
./alice Message1.txt SharedKey1.txt A_ctr.txt A_nonce.txt
```

### Using Verification Script
```bash
bash test_cases/VerifyingCRP.sh
```

## 🔒 Security Features

- **Confidentiality**: XOR encryption with SHA-256 derived keys
- **Integrity**: HMAC-SHA256 message authentication
- **Authenticity**: Shared secret key verification
- **Replay Protection**: Counter-based state management
- **Freshness**: Nonce-based message freshness

## 📚 Documentation

- **[Project Documentation](PROJECT_DOCUMENTATION.md)** - Comprehensive technical details
- **[Testing Guide](TESTING_GUIDE.md)** - How to create and test with custom files
- **[Requirements](project_requirements.md)** - Original project specifications

## 🛠️ Technical Specifications

- **Message Size**: 32 bytes (256 bits)
- **Hash Function**: SHA-256
- **HMAC**: HMAC-SHA256
- **Encryption**: XOR with derived keys
- **File Format**: Hexadecimal for all outputs
- **State Management**: Synchronized counters and nonces

## 🐛 Troubleshooting

### Common Issues

1. **OpenSSL not found**
   ```bash
   # macOS
   export LDFLAGS="-L/opt/homebrew/lib"
   export CPPFLAGS="-I/opt/homebrew/include"
   ```

2. **Message not 32 bytes**
   ```bash
   wc -c Message.txt  # Should show 32
   ```

3. **Permission denied**
   ```bash
   chmod +x alice bob
   ```

4. **Files not found**
   - Ensure all input files exist
   - Check file names match exactly

## 📄 License

This project is part of a programming assignment for educational purposes.

## 👨‍💻 Author

**Sanaan** - [GitHub](https://github.com/sfwani)

## 🤝 Contributing

This is an educational project. For questions or issues, please open a GitHub issue.

---

**Status**: ✅ Complete and Functional  
**Last Updated**: September 9, 2025  
