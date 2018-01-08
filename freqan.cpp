#include <iostream>
#include <iomanip>
#include <vector>
#include <cctype>
#include "freqan.hpp"

// FrequencyAnalyzer class public methods

// method parses text buffer
void FrequencyAnalyzer::readBuffer(const std::string& buf)
{
   n = 0;
   text_buffer = "";
   for (int i = 0; i < buf.length(); i++)
      if (isalpha(buf[i])) {
         ++n;
         text_buffer += toupper(buf[i]);
      }
   compute_frequencies();
}

// public accessor method retrieves index of coincidence computation
float FrequencyAnalyzer::indexOfCoincidence() const
{
   return index_of_coincidence();
}

// public accessor method retrieves english plaintext correlation computation
float FrequencyAnalyzer::englishCorrelation() const
{
   return english_correlation();
}

// display raw count of counted occurences of letters
void FrequencyAnalyzer::showCounts() const
{
   std::cout << std::fixed;
   std::cout.precision(3);
   std::cout << "<table class='gradienttable'>\n";
   std::cout << "<tr><th>letter</th><th>count</th><th>frequency</th><th>english</th></tr>\n";
   for (char letter = 'A'; letter <= 'Z'; letter++) {
      int idx = letter - 65;
      std::cout << "<tr><td>" << letter << "</td>";
      std::cout << "<td>" << count[idx] << "</td>";
      std::cout << "<td>" << frequency[idx] << "</td>";
      std::cout << "<td>" << english[idx] << "</td></tr>\n";
   }
   std::cout << "</table>\n";
}

void FrequencyAnalyzer::cipherReport(const Cipher& cipher_)
{
   // input buffer computations
   text_buffer = cipher_.inbuf;  // set text buffer to input buffer
   compute_frequencies();
   float inioc = indexOfCoincidence();
   float inept = englishCorrelation();
   std::vector<int> incount(count, count + C);
   std::vector<float> infreq(frequency, frequency + C);
   std::cout << std::fixed;
   // output buffer computations
   text_buffer = cipher_.outbuf; // set text buffer to output buffer
   compute_frequencies();
   float outioc = indexOfCoincidence();
   float outept = englishCorrelation();
   std::vector<int> outcount(count, count + C);
   std::vector<float> outfreq(frequency, frequency + C);
   // emit html list
   std::cout.precision(3);
   std::cout << "<ul>\n";
   std::cout << "<li>no. of characters processed = " << n << "</li>\n";
   std::cout << "<li>input index of coincidence = " << inioc << "</li>\n";
   std::cout << "<li>input english plaintext correlation = " << inept << "</li>\n";
   std::cout << "<li>output index of coincidence = " << outioc << "</li>\n";
   std::cout << "<li>output english plaintext correlation = " << outept << "</li>\n";
   std::cout << "</ul>\n";
   // emit html table
   std::cout << "<table class='gradienttable'>\n";
   std::cout << "<tr><th>letter</th><th>input count</th><th>input frequency</th><th>english</th>";
   std::cout << "<th>output frequency</th><th>output count</th></tr>\n";
   for (char letter = 'A'; letter <= 'Z'; letter++) {
      int idx = letter - 65;
      std::cout << "<tr><td>" << letter << "</td>";
      std::cout << "<td>" << incount[idx] << "</td>";
      std::cout << "<td>" << infreq[idx] << "</td>";
      std::cout << "<td>" << english[idx] << "</td>";
      std::cout << "<td>" << outfreq[idx] << "</td>";
      std::cout << "<td>" << outcount[idx] << "</td></tr>\n";
   }
   std::cout << "</table>\n";
}

// FrequencyAnalyzer class private methods

// method counts no. of occurrences of each letter of the alphabet
void FrequencyAnalyzer::compute_frequencies()
{
   n = 0;   // set char count to zero
   // set counting table to all zeros
   for (int i = 0; i < C; i++)
      count[i] = 0;
   // count occurrences of letters of the alphabet
   for (int i = 0; i < text_buffer.length(); i++) {
      if (isalpha(text_buffer[i])) {
         ++n;
         int idx = toupper(text_buffer[i]) - 65;
         ++(count[idx]);
      }
   }
   // compute frequencies
   for (int i = 0; i < C; i++)
      frequency[i] = (float)count[i] / (float)n;
}

// method computes index of coincidence of text buffer
float FrequencyAnalyzer::index_of_coincidence() const
{
   float s = 0;
   for (int i = 0; i < C; i++)
      s += (float)count[i] * (float)(count[i] - 1);
   float ioc = s / ((float)n * (float)(n - 1) / (float)C);
   return ioc;
}

// method computes english plaintext correlation of text buffer
float FrequencyAnalyzer::english_correlation() const
{
   float corr = 0;
   for (int i = 0; i < C; i++)
      corr += english[i] * frequency[i];
   return corr;
}
