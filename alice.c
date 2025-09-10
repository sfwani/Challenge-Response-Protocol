/**************************
 *      Alice Implementation        *
 **************************
 *
 * This program implements Alice's part of the Challenge-Response Protocol
 * Usage: ./alice Message.txt SharedKey.txt A_ctr.txt A_nonce.txt
 *
 */

 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <openssl/sha.h>
 #include <openssl/evp.h>
 #include <openssl/hmac.h>
 
 #define HASH_SIZE 32
 #define MESSAGE_SIZE 32  // Message is guaranteed to be 32 bytes
 
 // Function prototypes
 unsigned char* Read_File(char fileName[], int *fileLen);
 void Write_File(char fileName[], char input[]);
 void Convert_to_Hex(char output[], unsigned char input[], int inputlength);
 void Convert_To_Uchar(char* input_hex, unsigned char output[], int output_len);
 void Show_in_Hex(char name[], unsigned char input[], int inputlen);
 int read_counter_or_nonce(char* filename);
 void write_counter_or_nonce(char* filename, int value);
 void xor_arrays(unsigned char* a, unsigned char* b, unsigned char* result, int len);
 
 /*============================
         Read from File
 ==============================*/
 unsigned char* Read_File(char fileName[], int *fileLen)
 {
     FILE *pFile;
     pFile = fopen(fileName, "r");
     if (pFile == NULL)
     {
         printf("Error opening file: %s\n", fileName);
         exit(1);
     }
     fseek(pFile, 0L, SEEK_END);
     int temp_size = ftell(pFile) + 1;
     fseek(pFile, 0L, SEEK_SET);
     unsigned char *output = (unsigned char*) malloc(temp_size);
     fgets((char*)output, temp_size, pFile);
     fclose(pFile);
 
     *fileLen = temp_size - 1;
     return output;
 }
 
 /*============================
         Write to File
 ==============================*/
 void Write_File(char fileName[], char input[])
 {
     FILE *pFile;
     pFile = fopen(fileName, "w");
     if (pFile == NULL) {
         printf("Error opening file for writing: %s\n", fileName);
         exit(1);
     }
     fputs(input, pFile);
     fclose(pFile);
 }
 
 /*============================
         Convert to Hex 
 ==============================*/
 void Convert_to_Hex(char output[], unsigned char input[], int inputlength)
 {
     for (int i = 0; i < inputlength; i++) {
         sprintf(&output[2*i], "%02x", input[i]);
     }
     output[2*inputlength] = '\0';  // Null terminate
 }
 
 /*===================================
         Convert from Hex to unsigned char 
 =====================================*/
 void Convert_To_Uchar(char* input_hex, unsigned char output[], int output_len)
 {   
     for(int i = 0; i < output_len; i++){
         char tmp[3];
         tmp[0] = input_hex[2*i];
         tmp[1] = input_hex[2*i+1];
         tmp[2] = '\0';
         output[i] = (unsigned char)strtol(tmp, NULL, 16);
     }
 }
 
 /*============================
         Showing in Hex 
 ==============================*/
 void Show_in_Hex(char name[], unsigned char input[], int inputlen)
 {
     printf("%s %d: ", name, inputlen);
     for (int i = 0; i < inputlen; i++)
         printf("%02x", input[i]);
     printf("\n");
 }
 
 /*============================
         Read Counter/Nonce from file
 ==============================*/
 int read_counter_or_nonce(char* filename)
 {
     FILE* file = fopen(filename, "r");
     if (file == NULL) {
         printf("Error opening file: %s\n", filename);
         exit(1);
     }
     
     int value;
     if (fscanf(file, "%d", &value) != 1) {
         printf("Error reading value from file: %s\n", filename);
         fclose(file);
         exit(1);
     }
     
     fclose(file);
     return value;
 }
 
 /*============================
         Write Counter/Nonce to file
 ==============================*/
 void write_counter_or_nonce(char* filename, int value)
 {
     FILE* file = fopen(filename, "w");
     if (file == NULL) {
         printf("Error opening file for writing: %s\n", filename);
         exit(1);
     }
     
     fprintf(file, "%d", value);
     fclose(file);
 }
 
 /*============================
         XOR two arrays
 ==============================*/
 void xor_arrays(unsigned char* a, unsigned char* b, unsigned char* result, int len)
 {
     for (int i = 0; i < len; i++) {
         result[i] = a[i] ^ b[i];
     }
 }
 
 int main(int argc, char *argv[])
 {
     if (argc != 5) {
         printf("Usage: %s <message_file> <shared_key_file> <counter_file> <nonce_file>\n", argv[0]);
         return 1;
     }
     
     int msg_len, key_len;
     char hex_output[512];
     
     // Step 1: Read message, shared key, counter, and nonce
     unsigned char* message = Read_File(argv[1], &msg_len);
     unsigned char* shared_key = Read_File(argv[2], &key_len);
     int counter = read_counter_or_nonce(argv[3]);
     int nonce = read_counter_or_nonce(argv[4]);
     
     // Remove newlines if present
     if (message[msg_len-1] == '\n') {
         message[msg_len-1] = '\0';
         msg_len--;
     }
     if (shared_key[key_len-1] == '\n') {
         shared_key[key_len-1] = '\0';
         key_len--;
     }
     
     printf("Alice: Message length: %d, Key length: %d\n", msg_len, key_len);
     printf("Alice: Counter: %d, Nonce: %d\n", counter, nonce);
     
     // Step 2: Write key in hex format to Key.txt
     Convert_to_Hex(hex_output, shared_key, key_len);
     Write_File("Key.txt", hex_output);
     
     // Step 3: Encrypt message with XOR: c = m ⊕ H(k||ctr)
     // First, create k||ctr concatenation
     char counter_str[20];
     sprintf(counter_str, "%d", counter);
     unsigned char* key_counter = malloc(key_len + strlen(counter_str));
     memcpy(key_counter, shared_key, key_len);
     memcpy(key_counter + key_len, counter_str, strlen(counter_str));
     
     // Hash k||ctr
     unsigned char hash_key_counter[HASH_SIZE];
     SHA256(key_counter, key_len + strlen(counter_str), hash_key_counter);
     
     // XOR message with hash to get ciphertext
     unsigned char ciphertext[MESSAGE_SIZE];
     xor_arrays(message, hash_key_counter, ciphertext, MESSAGE_SIZE);
     
     // Step 4: Write ciphertext in hex format to Ciphertext.txt
     Convert_to_Hex(hex_output, ciphertext, MESSAGE_SIZE);
     Write_File("Ciphertext.txt", hex_output);
     
     // Step 5: Compute signature using HMAC: sig = HMAC_k(c||nonce)
     char nonce_str[20];
     sprintf(nonce_str, "%d", nonce);
     unsigned char* cipher_nonce = malloc(MESSAGE_SIZE + strlen(nonce_str));
     memcpy(cipher_nonce, ciphertext, MESSAGE_SIZE);
     memcpy(cipher_nonce + MESSAGE_SIZE, nonce_str, strlen(nonce_str));
     
     unsigned char signature[HASH_SIZE];
     unsigned int sig_len;
     HMAC(EVP_sha256(), shared_key, key_len, cipher_nonce, MESSAGE_SIZE + strlen(nonce_str), signature, &sig_len);
     
     // Step 6: Write signature in hex format to Signature.txt
     Convert_to_Hex(hex_output, signature, sig_len);
     Write_File("Signature.txt", hex_output);
     
     // Step 7: Read Bob's response from Response.txt (graceful exit if doesn't exist)
     FILE* response_file = fopen("Response.txt", "r");
     if (response_file == NULL) {
         printf("Alice: Response.txt not found. Bob hasn't responded yet. Exiting gracefully.\n");
         free(message);
         free(shared_key);
         free(key_counter);
         free(cipher_nonce);
         return 0;
     }
     fclose(response_file);
     
     int response_len;
     unsigned char* bob_response_hex = Read_File("Response.txt", &response_len);
     if (bob_response_hex[response_len-1] == '\n') {
         bob_response_hex[response_len-1] = '\0';
         response_len--;
     }
     
     // Convert Bob's response from hex to binary
     unsigned char bob_response[HASH_SIZE];
     Convert_To_Uchar((char*)bob_response_hex, bob_response, HASH_SIZE);
     
     // Step 8: Compute expected response: response' = H(m||(ctr+1)||(nonce+1))
     char expected_counter_str[20];
     char expected_nonce_str[20];
     sprintf(expected_counter_str, "%d", counter + 1);
     sprintf(expected_nonce_str, "%d", nonce + 1);
     
     unsigned char* msg_ctr_nonce = malloc(MESSAGE_SIZE + strlen(expected_counter_str) + strlen(expected_nonce_str));
     memcpy(msg_ctr_nonce, message, MESSAGE_SIZE);
     memcpy(msg_ctr_nonce + MESSAGE_SIZE, expected_counter_str, strlen(expected_counter_str));
     memcpy(msg_ctr_nonce + MESSAGE_SIZE + strlen(expected_counter_str), expected_nonce_str, strlen(expected_nonce_str));
     
     unsigned char expected_response[HASH_SIZE];
     SHA256(msg_ctr_nonce, MESSAGE_SIZE + strlen(expected_counter_str) + strlen(expected_nonce_str), expected_response);
     
     // Step 9: Compare responses and write result
     if (memcmp(bob_response, expected_response, HASH_SIZE) == 0) {
         Write_File("Acknowledgment.txt", "Acknowledgment Successful");
         printf("Alice: Acknowledgment Successful!\n");
     } else {
         Write_File("Acknowledgment.txt", "Acknowledgment Failed");
         printf("Alice: Acknowledgment Failed!\n");
     }
     
     // Step 10: Update counter and nonce
     write_counter_or_nonce(argv[3], counter + 1);
     write_counter_or_nonce(argv[4], nonce + 1);
     
     // Cleanup
     free(message);
     free(shared_key);
     free(key_counter);
     free(cipher_nonce);
     free(bob_response_hex);
     free(msg_ctr_nonce);
     
     printf("Alice: Protocol completed successfully!\n");
     return 0;
 }