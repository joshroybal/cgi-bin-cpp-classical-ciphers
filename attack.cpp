#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include "cipher.hpp"
#include "cgilib.hpp"
#include "freqan.hpp"
#include "hypertext.hpp"

struct keyrow {
   int a;
   int b;
   float ioc;
   float cor;
   int n;
};

float correlation(const float [], int);
std::string formatStr(const std::string&);
std::string autoKey(const std::string&);
bool runDown(keyrow[], const std::string&, int);
bool autoKey(keyrow[], const std::string&, int, bool);
void readData();
void errMsg();

int main()
{
   char card[257];   
   std::string cipher, flag, keystr;
   keyrow table[C * C];

   readQueryString(card);
   std::cout << "Content-Type: text/html; charset=utf-8\n\n";
   std::string header = "cgi-bin C++ classical cipher attacks";
   printPageTop(header);

   if (strcmp(card, "step1") == 0) {
      std::string formData;
      getline(std::cin, formData);
      // get message
      std::string message = readFormValue(formData, "message");
      stripMessage(message);
      // check buffer
      int substrlen = C * C;
      if (message.length() < substrlen)
         substrlen = message.length();
      std::cout << "<p>ciphertext</p>\n";
      std::cout << "<pre>" << formatStr(message).substr(0, substrlen) << " . . . </pre>\n";
      // get frequency analysis of message
      FrequencyAnalyzer freqan;
      freqan.readBuffer(message);
      freqan.showStats();
      freqan.showCounts();
      // begin attack processing
      Cipher decryptor;
      
      // begin autokey attacks
      // Vigenere pass
      // std::cout << "<div>Vigenere autokey attack pass.</div>\n";
      int p = 1;
      while (!autoKey(table, message, p, false) && p <= C)
         ++p;
      if (p <= C) {
         printPageBottom();
         return 0;
      }
      // Cipherdisk pass
      // std::cout << "<div>Beaufort autokey attack pass.</div>\n";
      p = 1;
      while (!autoKey(table, message, p, true) && p <= C)
         ++p;
      if (p <= C) {
         printPageBottom();
         return 0;
      }
      // end autokey attacks
      // begin regular and progressive key attack
      // std::cout << "<div>Regular and progressive key attack pass.</div>\n";
      int trial_period = 0;
      std::string slice;
      for (int p = 1; p <= C*C; p++) {
         slice = "";
         for (int j = 0; j < message.length(); j += p)
            slice += message[j];
         freqan.readBuffer(slice);
         if (freqan.indexOfCoincidence() >= 1.6) {
            trial_period = p;
            break;
         }
      }
      if (trial_period == 0) {
         printPageBottom();
         return 0;
      }
      else {
         std::cout << "<p>\n";
         std::cout << "<div>period = " << trial_period << "</div>\n";
         std::cout << "<div>index of coincidence = " << freqan.indexOfCoincidence() << "</div>\n";
         std::cout << "</p>\n";
      }
      // if we have a trial period greater than zero or less than C sqaured we shall
      // 'run down' the alphabets
      int n = slice.length();
      std::string plaintext(message.length(), ' ');
      if (!runDown(table, message, trial_period)) {
         errMsg();
         printPageBottom();
         return 1;
      }
      // end attack processing
      // print bottom of page
      printPageBottom();
   }   
   return 0;
}

std::string formatStr(const std::string& str)
{
   // append newlines in core
   std::string fmtstr = str;
   size_t idx = 0;
   while (fmtstr[idx] != '\0') {
      if ((idx+1) % 79 == 0)
         fmtstr.insert(idx, "\n");
      ++idx;
   }
   return fmtstr;
}

float correlation(const float frequencies[], int n)
{
   float result = 0;
   for (int i = 0; i < n; i++)
      result += frequencies[i] * frequencies[i];
   return result;
}

bool runDown(keyrow table[], const std::string& buf, int p)
{
   const float ENG_COR = correlation(english, C);
   int best_a, best_b;
   float ioc, cor, mindev;
   Cipher decryptor;
   FrequencyAnalyzer freqan;
   int n = buf.length();
   std::string plaintext(buf.length(), ' ');
   for (int i = 0; i < p; i++) {
      // do something
      std::string trial_slice;
      for (int j = i; j < n; j += p)
         trial_slice += buf[j];
      decryptor.readBuf(trial_slice);
      // initialize rundown values
      mindev = ENG_COR;
      best_a = 1;
      best_b = 0;
      // construct a trial slice 'runnning down' the affine and linear mappings of the alphabet
      for (int a = 1; a < 26; a += 2) {
         if (a == 13) continue;
         for (int b = 0; b < C; b++) {
            std::string test_slice = decryptor.Affine("decrypt", a, b);
            freqan.readBuffer(test_slice);
            cor = freqan.englishCorrelation();
            float dev = std::abs(ENG_COR - cor);
            if (dev < mindev) {
               mindev = dev;
               best_a = a;
                  best_b = b;
            }
         }
      }
      trial_slice = decryptor.Affine("decrypt", best_a, best_b);
      freqan.readBuffer(trial_slice);
      ioc = freqan.indexOfCoincidence();
      cor = freqan.englishCorrelation();
      if (ioc < 1.6 || cor < .06)
         return false;
      table[i].a = best_a;
      table[i].b = best_b;
      table[i].ioc = ioc;
      table[i].cor = cor;
      table[i].n = trial_slice.length();
      int idx = 0;
      for (int j = i; j < n; j += p)
         plaintext[j] = tolower(trial_slice[idx++]);
   }
   int substrlen = C * C;
   if (plaintext.length() < substrlen)
      substrlen = plaintext.length();
   // emit html output
   std::cout << "<p>trial key values</p>\n";
   // emit trial key table
   std::cout << "<table class = 'gradienttable-mini'>\n";
   std::cout << "<tr>";
   std::cout << "<th>idx</th>";
   std::cout << "<th>a</th>";
   std::cout << "<th>b</th>";
   std::cout << "<th>ioc</th>";
   std::cout << "<th>cor</th>";
   std::cout << "<th>len</th>";
   std::cout << "</tr>\n";
   for (int i = 0; i < p; i++) {
      std::cout << "<tr>";
      std::cout << "<td>" << i + 1 << "</td>";
      std::cout << "<td>" << table[i].a << "</td>";
      std::cout << "<td>" << table[i].b << "</td>";
      std::cout << "<td>" << table[i].ioc << "</td>";
      std::cout << "<td>" << table[i].cor << "</td>";
      std::cout << "<td>" << table[i].n << "</td>";
      std::cout << "</tr>\n";
   }
   std::cout << "</table>\n";
   std::cout << "<p>trial plaintext</p>\n";
   std::cout << "<textarea rows = '12' cols = '80'>\n";
   std::cout << formatStr(plaintext) << std::endl;
   std::cout << "</textarea>\n";
   freqan.readBuffer(plaintext);
   freqan.showStats();
   freqan.showCounts();
   return true;
}

bool autoKey(keyrow table[], const std::string& buf, int p, bool cipherdisk)
{
   const float ENG_COR = correlation(english, C);
   int best_a, best_b;
   float ioc, mindev, cor;
   Cipher decryptor;
   FrequencyAnalyzer freqan;
   int n = buf.length();
   std::string plaintext(buf.length(), ' ');
   for (int i = 0; i < p; i++) {
      // do something
      std::string trial_slice;
      for (int j = i; j < n; j += p)
         trial_slice += buf[j];
      decryptor.readBuf(trial_slice);
      // initialize rundown values
      mindev = ENG_COR;
      best_a = 1;
      best_b = 0;
      // construct a trial slice 'runnning down' the affine and linear mappings of the alphabet
      for (int a = 1; a < 26; a += 2) {
         if (a == 13) continue;
         for (int b = 0; b < C; b++) {
            std::string test_slice = decryptor.AutoAffine("decrypt", a, b, cipherdisk);
            freqan.readBuffer(test_slice);
            cor = freqan.englishCorrelation();
            float dev = std::fabs(ENG_COR - cor);
            if (dev < mindev) {
               mindev = dev;
               best_a = a;
               best_b = b;
            }
         }
      }
      if (mindev > .007)   // bail out if minimum deviation too large
         return false;
      trial_slice = decryptor.AutoAffine("decrypt", best_a, best_b, cipherdisk);
      freqan.readBuffer(trial_slice);
      if (freqan.indexOfCoincidence() < 1.5) // bail out if index of coincidence too small
         return false;
      table[i].a = best_a;
      table[i].b = best_b;
      table[i].ioc = freqan.indexOfCoincidence();
      table[i].cor = freqan.englishCorrelation();
      table[i].n = trial_slice.length();
      int idx = 0;
      for (int j = i; j < n; j += p)
         plaintext[j] = tolower(trial_slice[idx++]);
   }
   int substrlen = C * C;
   if (plaintext.length() < substrlen)
      substrlen = plaintext.length();
   // emit html output
   std::cout << "<p>trial key values</p>\n";
   // emit trial key table
   std::cout << "<table class = 'gradienttable-mini'>\n";
   std::cout << "<tr>";
   std::cout << "<th>idx</th>";
   std::cout << "<th>a</th>";
   std::cout << "<th>b</th>";
   std::cout << "<th>alpha</th>";
   std::cout << "<th>atbash</th>";
   std::cout << "<th>ioc</th>";
   std::cout << "<th>cor</th>";
   std::cout << "<th>len</th>";
   std::cout << "</tr>\n";
   for (int i = 0; i < p; i++) {
      std::cout << "<tr>";
      std::cout << "<td>" << i + 1 << "</td>";
      std::cout << "<td>" << table[i].a << "</td>";
      std::cout << "<td>" << table[i].b << "</td>";
      std::cout << "<td>" << (char)(65+table[i].b) << "</td>";
      std::cout << "<td>" << (char)(64+(26-table[i].b)) << "</td>";
      std::cout << "<td>" << table[i].ioc << "</td>";
      std::cout << "<td>" << table[i].cor << "</td>";
      std::cout << "<td>" << table[i].n << "</td>";
      std::cout << "</tr>\n";
   }
   std::cout << "</table>\n";
   std::cout << "<p>trial plaintext</p>\n";
   std::cout << "<textarea rows = '12' cols = '80'>\n";
   std::cout << formatStr(plaintext) << std::endl;
   std::cout << "</textarea>\n";
   std::cout << "<p></p>\n";
   freqan.readBuffer(plaintext);
   freqan.showStats();
   freqan.showCounts();
   return true;
}

void readData()
{
   std::string data_item;
   getline(std::cin, data_item);
}

void errMsg()
{
   std::cout << "<p>I'm sorry Dave, I'm afraid I can't do that.</p>\n";
}
