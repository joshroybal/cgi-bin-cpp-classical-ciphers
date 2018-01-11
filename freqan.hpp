#ifndef FREQAN_HPP
#define FREQAN_HPP

#include <string>
#include "cipher.hpp"

static const int C = 26;

static const float english[C] = { .0781, .0128, .0293, .0411, .1305, .0288,
   .0139, .0565, .0677, .0023, .0042, .036, .0262, .0728, .0821, .0215, .0014,
   .0664, .0646, .0902, .0277, .01, .0149, .003, .0151, .0009 };

class FrequencyAnalyzer {
public:
   void readBuffer(const std::string&);
   float indexOfCoincidence() const;
   float englishCorrelation() const;
   void showStats() const;
   void showCounts() const;
   void cipherReport(const Cipher&);
private:
   int n;   // no of alphabetic chars in text_buffer
   std::string text_buffer;
   int count[C];
   float frequency[C];
   void compute_frequencies();
   float index_of_coincidence() const;
   float english_correlation() const;
};

#endif
