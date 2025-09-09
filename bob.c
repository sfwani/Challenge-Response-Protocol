/**************************
 *      Bob Implementation        *
 **************************
 *
 * This program implements Bob's part of the Challenge-Response Protocol
 * Usage: ./bob Ciphertext.txt Signature.txt SharedKey.txt B_ctr.txt B_nonce.txt
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
     if (argc != 6) {
         printf("Usage: %s <ciphertext_file> <signature_file> <shared_key_file> <counter_file> <nonce_file>\n", argv[0]);
         return 1;
     }
     
     int cipher_len, sig_len, key_len;
     char hex_output[200];
     
     // Step 1: Read ciphertext, signature, shared key, counter, and nonce
     unsigned char* ciphertext_hex = Read_File(argv[1], &cipher_len);
     unsigned char* signature_hex = Read_File(argv[2], &sig_len);
     unsigned char* shared_key = Read_File(argv[3], &key_len);
     int counter = read_counter_or_nonce(argv[4]);
     int nonce = read_counter_or_nonce(argv[5]);
     
     // Remove newlines if present
     if (ciphertext_hex[cipher_len-1] == '\n') {
         ciphertext_hex[cipher_len-1] = '\0';
         cipher_len--;
     }
     if (signature_hex[sig_len-1] == '\n') {
         signature_hex[sig_len-1] = '\0';
         sig_len--;
     }
     if (shared_key[key_len-1] == '\n') {
         shared_key[key_len-1] = '\0';
         key_len--;
     }
     
     printf("Bob: Cipher length: %d, Sig length: %d, Key length: %d\n", cipher_len, sig_len, key_len);
     printf("Bob: Counter: %d, Nonce: %d\n", counter, nonce);
     
     // Convert hex inputs to binary
     unsigned char ciphertext[MESSAGE_SIZE];
     unsigned char alice_signature[HASH_SIZE];
     Convert_To_Uchar((char*)ciphertext_hex, ciphertext, MESSAGE_SIZE);
     Convert_To_Uchar((char*)signature_hex, alice_signature, HASH_SIZE);
     
     // Step 2: Compute expected signature: sig' = HMAC_k(c||nonce)
     char nonce_str[20];
     sprintf(nonce_str, "%d", nonce);
     unsigned char* cipher_nonce = malloc(MESSAGE_SIZE + strlen(nonce_str));
     memcpy(cipher_nonce, ciphertext, MESSAGE_SIZE);
     memcpy(cipher_nonce + MESSAGE_SIZE, nonce_str, strlen(nonce_str));
     
     unsigned char expected_signature[HASH_SIZE];
     unsigned int expected_sig_len;
     HMAC(EVP_sha256(), shared_key, key_len, cipher_nonce, MESSAGE_SIZE + strlen(nonce_str), expected_signature, &expected_sig_len);
     
     // Step 3: Verify signature
     if (memcmp(alice_signature, expected_signature, HASH_SIZE) != 0) {
         printf("Bob: Signature verification failed! Exiting.\n");
         free(ciphertext_hex);
         free(signature_hex);
         free(shared_key);
         free(cipher_nonce);
         exit(1);
     }
     
     printf("Bob: Signature verification successful!\n");
     
     // Step 4: Decrypt ciphertext: m = c ⊕ H(k||ctr)
     // Create k||ctr concatenation
     char counter_str[20];
     sprintf(counter_str, "%d", counter);
     unsigned char* key_counter = malloc(key_len + strlen(counter_str));
     memcpy(key_counter, shared_key, key_len);
     memcpy(key_counter + key_len, counter_str, strlen(counter_str));
     
     // Hash k||ctr
     unsigned char hash_key_counter[HASH_SIZE];
     SHA256(key_counter, key_len + strlen(counter_str), hash_key_counter);
     
     // XOR ciphertext with hash to get message
     unsigned char decrypted_message[MESSAGE_SIZE];
     xor_arrays(ciphertext, hash_key_counter, decrypted_message, MESSAGE_SIZE);
     
     printf("Bob: Message decrypted successfully!\n");
     Show_in_Hex("Bob: Decrypted message", decrypted_message, MESSAGE_SIZE);
     
     // Step 5: Compute response: response = H(m||(ctr+1)||(nonce+1))
     char response_counter_str[20];
     char response_nonce_str[20];
     sprintf(response_counter_str, "%d", counter + 1);
     sprintf(response_nonce_str, "%d", nonce + 1);
     
     unsigned char* msg_ctr_nonce = malloc(MESSAGE_SIZE + strlen(response_counter_str) + strlen(response_nonce_str));
     memcpy(msg_ctr_nonce, decrypted_message, MESSAGE_SIZE);
     memcpy(msg_ctr_nonce + MESSAGE_SIZE, response_counter_str, strlen(response_counter_str));
     memcpy(msg_ctr_nonce + MESSAGE_SIZE + strlen(response_counter_str), response_nonce_str, strlen(response_nonce_str));
     
     unsigned char response[HASH_SIZE];
     SHA256(msg_ctr_nonce, MESSAGE_SIZE + strlen(response_counter_str) + strlen(response_nonce_str), response);
     
     // Step 6: Write response in hex format to Response.txt
     Convert_to_Hex(hex_output, response, HASH_SIZE);
     Write_File("Response.txt", hex_output);
     
     printf("Bob: Response computed and written to Response.txt\n");
     Show_in_Hex("Bob: Response", response, HASH_SIZE);
     
     // Step 7: Update counter and nonce
     write_counter_or_nonce(argv[4], counter + 1);
     write_counter_or_nonce(argv[5], nonce + 1);
     
     printf("Bob: Counter and nonce updated\n");
     
     // Cleanup
     free(ciphertext_hex);
     free(signature_hex);
     free(shared_key);
     free(cipher_nonce);
     free(key_counter);
     free(msg_ctr_nonce);
     
     printf("Bob: Protocol completed successfully!\n");
     return 0;
 }