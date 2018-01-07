#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "cipher.hpp"

// Cipher class public methods

// read piped input from stdin into core memory
void Cipher::readBuf(const std::string& message)
{
   for (int i = 0; i < message.length(); i++) 
      if (isalpha(message[i]))
         inbuf += toupper(message[i]);
}

// process input read into buffer
void Cipher::processBuf(const std::string& cipher, const std::string& flag,
                        std::string& keystr, bool autokey, bool progkey)
{
   std::transform(keystr.begin(), keystr.end(), keystr.begin(), ::toupper);
   if (cipher == "coltran") {
      int k;
      std::istringstream ss(keystr);
      ss >> k;
      colTran(k, flag);
   }
   else if (cipher == "keytran")
      keyTran(keystr, flag);
   else if (cipher == "keysub")
      keySub(keystr, flag);
   else if (cipher == "Caesar") {
      int k;
      std::istringstream ss(keystr);
      ss >> k;
      Caesar(k, flag);
   }
   else if (cipher == "Affine") {
      int a, b;
      std::istringstream ss(keystr);
      ss >> a >> b;
      Affine(a, b, flag);
   }
   else if (cipher == "Vigenere")
      Vigenere(keystr, flag, autokey, progkey);
   else if (cipher == "Gronsfeld")
      Gronsfeld(keystr, flag, autokey, progkey);
   else if (cipher == "Porta")
      Porta(keystr, flag);
   else if (cipher == "Beaufort")
      Beaufort(keystr, flag, autokey, progkey);
   else if (cipher == "Variant")
      Variant(keystr, flag, autokey, progkey);
   else if (cipher == "SlideI" || cipher == "SlideII"
            || cipher == "SlideIII") {
      // clean up the cgi stuff from the keystring
      int n = keystr.length();
      for (int i = 0; i < n; i++)
         if (!isalpha(keystr[i]))
            keystr[i] = ' ';
      std::istringstream ss(keystr);
      std::string primary, secondary;
      ss >> primary >> secondary;      
      if (secondary == "") secondary = primary;
      Slide(primary, secondary, flag, cipher, autokey, progkey);
   }

   if (flag == "decrypt")
      std::transform(outbuf.begin(), outbuf.end(), outbuf.begin(), ::tolower);
}

// write output buffer to stdout
void Cipher::writeBuf() const
{
   // append newlines in core
   int n = outbuf.length();
   std::string fmtbuf;
   for (int i = 0; i < n; i++) {
      fmtbuf += outbuf[i];
      if ((i+1) % 78 == 0)
         fmtbuf += '\n';
   }
   // write output
   std::cout << fmtbuf << std::endl;
}

// Cipher class private methods

// Cipher class private auxiliary methods

// Caesar linear shift character in alphabet
char Cipher::caesar(char ch, int k) const
{
   int cc = ch - MIN_CHAR;
   cc = (cc + k) % RANGE;
   if (cc < 0) cc += RANGE;
   char result = cc + MIN_CHAR;
   return result;
}

// Affine mapping of character in alphabet
char Cipher::affine(char ch, int a, int b) const
{
   int cc = ch - MIN_CHAR;
   cc = (a*cc + b) % RANGE;
   if (cc < 0) cc += RANGE;
   char result = cc + MIN_CHAR;
   return result;
}

// map index to RANGE - (index + 1)
char Cipher::atbash(char ch) const
{
   char result = MIN_CHAR + RANGE - (ch - MIN_CHAR + 1);
   return result;
}

// substitution auxiliary method
char Cipher::sub(char ch, const std::string& alphabet,
                  const std::string& flag) const
{
   char result;
   int idx;
   if (flag == "encrypt") {
      idx = ch - MIN_CHAR;
      result = alphabet[idx];
   } else {
      idx = alphabet.find(ch);
      result = MIN_CHAR + idx;
   }
   return result;
}

// generate transposition key from string
void Cipher::strtokey(int key[], const std::string& keystr, int n) const
{
   int j = 0;
   int tmp[n];
   // forward letter occurrence numbering
   for (char letter = 'A'; letter <= 'Z' && j < n; letter++)
      for (int i = 0; i < n && j < n; i++)
         if (keystr[i] == letter) {
            tmp[i] = j++;
            if (j == n)
               break;
         }
   // build 'take-off/write-in' index array
   for (int i = 0; i < n; i++)
      for (int j = 0; j < n; j++)
         if (tmp[j] == i)
            key[i] = j;
}

// auxiliary reset autokey method
void Cipher::reset_auto_key(int& idx, char plaintext, char ciphertext,
                              const std::string& flag) const
{
   if (flag == "encrypt")
      idx = plaintext - MIN_CHAR;
   else if (flag == "decrypt")
      idx = -(ciphertext - MIN_CHAR);
}

// auxiliary reset progkey method
void Cipher::reset_prog_key(int& idx, const std::string& flag) const
{
   if (flag == "encrypt")
      ++idx % RANGE;
   else if (flag == "decrypt")
      --idx % RANGE;
}

// returns specific instances of slide generated ciphertext alphabets
std::string Cipher::get_alphabet(const std::string& plaintext,
                           const std::string& ciphertext, int idx) const
{
   std::string alphabet;
   std::string strip = ciphertext.substr(idx,RANGE-idx)+ciphertext.substr(0,idx);
   for (char letter = 'A'; letter <= 'Z'; letter++) {
      idx = plaintext.find(letter);
      alphabet += strip[idx];
   }
   return alphabet;
}

// Cipher class cipher methods, buffer encryption/decryption

// Columnar transposition cipher method
void Cipher::colTran(int k, const std::string& flag)
{
   int n = inbuf.length();
   int idx = 0;
   if (flag == "encrypt")
      for (int i = 0; i < k; i++)
         for (int j = i; j < n; j += k)
            outbuf += inbuf[j];
   else if (flag == "decrypt") {
      outbuf.resize(n);
      int idx = 0;
      for (int i = 0; i < k; i++)
         for (int j = i; j < n; j += k)
            outbuf[j] = inbuf[idx++];
   }
}

// Keyword prepared transposition cipher method
void Cipher::keyTran(const std::string& keystr, const std::string& flag)
{
   // prepare columnar transposition array from keyword
   int keylen = keystr.length();
   int key[keylen];
   strtokey(key, keystr, keylen);
   // columnar transposition
   int n = inbuf.length();
   if (flag == "encrypt")
      for (int i = 0; i < keylen; i++)
         for (int j = key[i]; j < n; j += keylen)
            outbuf += inbuf[j];
   else if (flag == "decrypt") {
      outbuf.resize(n);
      int idx = 0;
      for (int i = 0; i < keylen; i++)
         for (int j = key[i]; j < n; j += keylen)
            outbuf[j] = inbuf[idx++];
   }
}

// Keyword prepared simple substitution cipher method
void Cipher::keySub(const std::string& keystr, const std::string& flag)
{
   // prepare ciphertext alphabet from keyword
   int keylen = keystr.length();
   int key[keylen];
   strtokey(key, keystr, keylen);
   std::string plaintext = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   std::string ciphertext;
   int idx = 0;
   for (int i = 0; i < keylen; i++)
      for (int j = key[i]; j < plaintext.length(); j += keylen)
         ciphertext += plaintext[j];
   // encrypt/decrypt processing
   for (int i = 0; i < inbuf.length(); i++)
      outbuf += sub(inbuf[i], ciphertext, flag);
}

// Caesar shift cipher method
void Cipher::Caesar(int k, const std::string& flag)
{
   if (flag == "decrypt")  // negative of key for decryption
      k = -k;
   // encrypt/decrypt processing
   for (int i = 0; i < inbuf.length(); i++)
      outbuf += caesar(inbuf[i], k);
}

// Affine cipher method
void Cipher::Affine(int a, int b, const std::string& flag)
{
   if (flag == "decrypt") { // convert a, b to a^-1 and a^-1 x -b
      int ainv;
      int n = 1;
      while (n < RANGE) {
         if (a*n % RANGE == 1) {
            ainv = n;
            break;
         }
         n += 2;
         if (n == RANGE / 2) n += 2;
      }
      a = ainv;
      b = (ainv*-b) % RANGE;
   }
   for (int i = 0; i < inbuf.length(); i++)
      outbuf += affine(inbuf[i], a, b);
}

// Vigenere cipher methodi
void Cipher::Vigenere(const std::string& keystr, const std::string& flag,
                        bool autokey, bool progkey)
{
   int n = inbuf.length();
   int keylen = keystr.length();
   int key[keylen];
   // determine key array for encryption/decryption
   if (flag == "encrypt")
      for (int i = 0; i < keylen; i++)
         key[i] = keystr[i] - MIN_CHAR;
   else if (flag == "decrypt")
      for (int i = 0; i < keylen; i++)
         key[i] = -(keystr[i] - MIN_CHAR);
   // encrypt/decrypt
   if (autokey) {
      for (int i = 0; i < n; i++) {
         outbuf += caesar(inbuf[i], key[i % keylen]);
         reset_auto_key(key[i % keylen], inbuf[i], outbuf[i], flag);
      }
   }
   else if (progkey) {
      for (int i = 0; i < n; i++) {
         outbuf += caesar(inbuf[i], key[i % keylen]);
         reset_prog_key(key[i % keylen], flag);
      }
   }
   else
      for (int i = 0; i < n; i++)
         outbuf += caesar(inbuf[i], key[i % keylen]);
}

// Gronsfeld cipher method
void Cipher::Gronsfeld(const std::string& keystr, const std::string& flag,
                        bool autokey, bool progkey)
{
   int n = inbuf.length();
   int keylen = keystr.length();
   int key[keylen];
   // determine key array for encryption/decryption
   if (flag == "encrypt")
      for (int i = 0; i < keylen; i++)
         key[i] = keystr[i] - 48;
   else if (flag == "decrypt")
      for (int i = 0; i < keylen; i++)
         key[i] = -(keystr[i] - 48);
   // encrypt/decrypt
   if (autokey) {
      for (int i = 0; i < n; i++) {
         outbuf += caesar(inbuf[i], key[i % keylen]);
         if (flag == "encrypt")
            key[i % keylen] = inbuf[i] - 48;
         else if (flag == "decrypt")
            key[i % keylen] = -(outbuf[i] - 48);
      }
   }
   else if (progkey) {
      for (int i = 0; i < n; i++) {
         outbuf += caesar(inbuf[i], key[i % keylen]);
         if (flag == "encrypt")
            ++(key[i % keylen]) % 10;
         else if (flag == "decrypt")
            --(key[i % keylen]) % 10;
      }
   }
   else
      for (int i = 0; i < n; i++)
         outbuf += caesar(inbuf[i], key[i % keylen]);
}

// Porta cipher method
void Cipher::Porta(const std::string& keystr, const std::string& flag)
{
   int n = inbuf.length();
   int keylen = keystr.length();
   int key[keylen];
   const int HALF = RANGE / 2;
   // convert keyword string to array of ints
   for (int i = 0; i < keylen; i++)
      key[i] = (keystr[i] - MIN_CHAR) / 2;
   // encrypt/decrypt processing
   for (int i = 0; i < n; i++) {
      int keyidx = key[i % keylen];
      int chidx = inbuf[i] - MIN_CHAR;
      if (chidx < HALF) {
         if (chidx + keyidx + HALF < RANGE)
            keyidx += HALF;
         }
      else {
         if (chidx - (HALF + keyidx) < 0)
            keyidx = -keyidx;
         else
            keyidx = -(HALF + keyidx);
      }
      outbuf += caesar(inbuf[i], keyidx);
   }
}

// Beaufort cipher method
void Cipher::Beaufort(const std::string& keystr, const std::string& flag,
                        bool autokey, bool progkey)
{
   int n = inbuf.length();
   int keylen = keystr.length();
   int key[keylen];
   for (int i = 0; i < keylen; i++)
      key[i] = keystr[i] - MIN_CHAR + 1;
   // encrypt/decrypt
   if (autokey) {
      for (int i = 0; i < n; i++) {
         outbuf += caesar(atbash(inbuf[i]), key[i % keylen]);
         if (flag == "encrypt")
            key[i % keylen] = inbuf[i] - MIN_CHAR;
         else if (flag == "decrypt")
            key[i % keylen] = outbuf[i] - MIN_CHAR;
      }
   }
   else if (progkey) {
      for (int i = 0; i < n; i++) {
         outbuf += caesar(atbash(inbuf[i]), key[i % keylen]);
         ++(key[i % keylen]) % RANGE;
      }
   }
   else
      for (int i = 0; i < n; i++)
         outbuf += caesar(atbash(inbuf[i]), key[i % keylen]);
}

// Beaufort variant cipher method
void Cipher::Variant(const std::string& keystr, const std::string& flag,
                     bool autokey, bool progkey)
{
   int n = inbuf.length();
   int keylen = keystr.length();
   int key[keylen];
   for (int i = 0; i < keylen; i++)
      key[i] = atbash(keystr[i]) - MIN_CHAR + 1;
   // determine key array for encryption/decryption
   if (flag == "encrypt")
      for (int i = 0; i < keylen; i++)
         key[i] = atbash(keystr[i]) - MIN_CHAR + 1;
   else if (flag == "decrypt")
      for (int i = 0; i < keylen; i++)
         key[i] = -(atbash(keystr[i]) - MIN_CHAR + 1);
   // encrypt/decrypt
   if (autokey) {
      for (int i = 0; i < n; i++) {
         outbuf += caesar(inbuf[i], key[i % keylen]);
         reset_auto_key(key[i % keylen], inbuf[i], outbuf[i], flag);
      }
   }
   else if (progkey) {
      for (int i = 0; i < n; i++) {
         outbuf += caesar(inbuf[i], key[i % keylen]);
         reset_prog_key(key[i % keylen], flag);
      }
   }
   else
      for (int i = 0; i < n; i++)
         outbuf += caesar(inbuf[i], key[i % keylen]);
}

// mixed slide cipher method
// Gaines I - plaintext slides against ciphertext
void Cipher::Slide(const std::string& primary, const std::string& secondary,
                     const std::string& flag, const std::string& cipher,
                     bool autokey, bool progkey)
{
   // prepare mixed alphabet from keyword
   std::string fixed, strip;
   int keylen = primary.length();
   int key[keylen];
   strtokey(key, primary, keylen);
   int idx = 0;
   for (int i = 0; i < keylen; i++)
      for (int j = key[i]; j < RANGE; j += keylen)
         if (cipher == "SlideI")
            fixed += MIN_CHAR + j;
         else
            strip += MIN_CHAR + j;
   // set the other or both alphabets according to slide type
   if (cipher == "SlideI")
      strip = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   else if (cipher == "SlideII")
      fixed = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   else
      fixed = strip;
   // encrypt/decrypt subroutine
   std::string keystr = secondary;
   keylen = secondary.length();
   for (int i = 0; i < inbuf.length(); i++) {
      int idx = strip.find(keystr[i % keylen]);
      std::string alphabet = get_alphabet(fixed, strip, idx);
      outbuf += sub(inbuf[i], alphabet, flag);
      if (autokey) {
         if (flag == "encrypt")
            keystr[i % keylen] = inbuf[i];
         else
            keystr[i % keylen] = outbuf[i];
      }
      if (progkey) {
         int idx = keystr[i % keylen] - MIN_CHAR;
         ++idx;
         idx %= RANGE;
         keystr[i % keylen] = MIN_CHAR + idx;
      }
   }
}
