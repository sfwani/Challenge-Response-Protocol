# Testing Guide: Creating Your Own Test Files

## Understanding the Test File Format

### 1. **Message File** (`Message.txt`)
- Must be exactly **32 bytes** (256 bits)
- Can contain any data (text, binary, etc.)
- Example: "ThisIsStringOf256bitsor32bytesYZ"

### 2. **Shared Key File** (`SharedKey.txt`)
- Can be any length (typically 16-32 bytes)
- Contains the secret key both Alice and Bob share
- Example: "TheWorldEndsWithYou" (19 bytes)

### 3. **Counter Files** (`A_ctr.txt`, `B_ctr.txt`)
- Contains a single integer as text
- Both Alice and Bob start with the same value
- Example: "2"

### 4. **Nonce Files** (`A_nonce.txt`, `B_nonce.txt`)
- Contains a single integer as text
- Both Alice and Bob start with the same value
- Example: "56"

## Creating Test Files

### Method 1: Using Command Line Tools

```bash
# Create a 32-byte message
echo -n "ThisIsStringOf256bitsor32bytesYZ" > Message.txt

# Create a shared key
echo -n "MySecretKey123456789" > SharedKey.txt

# Create counter files (both start with same value)
echo "1" > A_ctr.txt
echo "1" > B_ctr.txt

# Create nonce files (both start with same value)
echo "100" > A_nonce.txt
echo "100" > B_nonce.txt
```

### Method 2: Using a Text Editor
1. Create files with the exact names above
2. Put the content exactly as shown
3. Make sure there are no extra spaces or newlines

### Method 3: Using Python Script

```python
# create_test_files.py
with open("Message.txt", "wb") as f:
    f.write(b"ThisIsStringOf256bitsor32bytesYZ")

with open("SharedKey.txt", "wb") as f:
    f.write(b"MySecretKey123456789")

with open("A_ctr.txt", "w") as f:
    f.write("1")

with open("B_ctr.txt", "w") as f:
    f.write("1")

with open("A_nonce.txt", "w") as f:
    f.write("100")

with open("B_nonce.txt", "w") as f:
    f.write("100")

print("Test files created successfully!")
```

## Testing Your Files

### Step 1: Clean Previous Test
```bash
# Remove previous test outputs
rm -f Key.txt Ciphertext.txt Signature.txt Response.txt Acknowledgment.txt
```

### Step 2: Run Alice
```bash
./alice Message.txt SharedKey.txt A_ctr.txt A_nonce.txt
```

### Step 3: Run Bob
```bash
./bob Ciphertext.txt Signature.txt SharedKey.txt B_ctr.txt B_nonce.txt
```

### Step 4: Run Alice Again
```bash
./alice Message.txt SharedKey.txt A_ctr.txt A_nonce.txt
```

### Step 5: Check Results
```bash
# Check all output files
ls -la *.txt

# Check acknowledgment
cat Acknowledgment.txt

# Check updated counters
echo "A counter: $(cat A_ctr.txt)"
echo "A nonce: $(cat A_nonce.txt)"
echo "B counter: $(cat B_ctr.txt)"
echo "B nonce: $(cat B_nonce.txt)"
```

## Expected Output Files

After successful execution, you should see:
- `Key.txt` - Shared key in hex format
- `Ciphertext.txt` - Encrypted message in hex
- `Signature.txt` - HMAC signature in hex
- `Response.txt` - Bob's response in hex
- `Acknowledgment.txt` - "Acknowledgment Successful"
- Updated counter and nonce files

## Troubleshooting

### Common Issues:
1. **Message not 32 bytes**: Use `wc -c Message.txt` to check
2. **Files not found**: Make sure all input files exist
3. **Permission denied**: Use `chmod +x alice bob` to make executables
4. **OpenSSL errors**: Make sure OpenSSL is installed and paths are correct

### Debug Tips:
- Check file sizes: `ls -la *.txt`
- View file contents: `cat filename.txt`
- Check hex content: `hexdump -C filename.txt`
