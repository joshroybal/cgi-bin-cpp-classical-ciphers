#ifndef CIPHER_HPP__
#define CIPHER_HPP__

#include <string>

const int MIN_CHAR = 65;   // 65 classical 32 ascii
const int MAX_CHAR = 90;   // 90 classical 126 ascii
const int RANGE = 26;      // 26 classical 95 ascii

class Cipher {
public:
   friend class FrequencyAnalyzer;
   void readBuf(const std::string&);
   void processBuf(const std::string&, const std::string&, std::string&, bool, bool);
   void writeBuf() const;
private:
   // private data
   std::string inbuf;
   std::string outbuf;
   // private auxiliary methods
   char caesar(char, int) const;
   char affine(char, int, int) const;
   char atbash(char) const;
   char sub(char, const std::string&, const std::string&) const;
   void strtokey(int [], const std::string&, int) const;
   void reset_auto_key(int&, char, char, const std::string&) const;
   void reset_prog_key(int&, const std::string&) const;
   std::string get_alphabet(const std::string&, const std::string&, int) const;
   // private cipher methods
   void colTran(int, const std::string&);
   void keyTran(const std::string&, const std::string&);
   void keySub(const std::string&, const std::string&);
   void Caesar(int, const std::string&);
   void Affine(int, int, const std::string&);
   void Vigenere(const std::string&, const std::string&, bool, bool);
   void Gronsfeld(const std::string&, const std::string&, bool, bool);
   void Porta(const std::string&, const std::string&);
   void Beaufort(const std::string&, const std::string&, bool, bool);
   void Variant(const std::string&, const std::string&, bool, bool);
   void Slide(const std::string&, const std::string&, const std::string&,
               const std::string&, bool, bool);
};

#endif
