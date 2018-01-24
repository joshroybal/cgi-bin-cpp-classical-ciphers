#include <iostream>
#include <cstring>
#include "cipher.hpp"
#include "cgilib.hpp"
#include "freqan.hpp"
#include "hypertext.hpp"

void readData();
void errMsg();

int main()
{
   char card[257];
   std::string cipher, flag, keystr;

   readQueryString(card);
   std::cout << "Content-Type: text/html; charset=utf-8\n\n";
   std::cout << "<!DOCTYPE HTML>\n";
   std::cout << "<html>\n";
   std::cout << "<head>\n";
   std::cout << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
   // std::cout << "<link rel=\"stylesheet\" media=\"all\"  href=\"/includes/style.css\">\n";
   std::cout << "<link rel=\"stylesheet\" media=\"all\"  href=\"/includes/gradienttable.css\">\n";
   std::cout << "<title>cgi-bin C++ classical ciphers</title>\n";
   std::cout << "</head>\n";
   std::cout << "<body>\n";
   std::cout << "<header><p>cgi-bin C++ classical ciphers</p></header>\n";
   std::cout << "<p></p>\n";
   std::cout << "<div><a href=\"/index.php\">Home</a> | <a href=\"/cppciphers.html\">Back</a></div>\n";
   std::cout << "<p></p>\n";

   if (strcmp(card, "step1") == 0) {
      std::string formData;
      getline(std::cin, formData);
      // get cipher
      std::string cipher = readFormValue(formData, "cipher");
      // get keytype
      std::string keytype = readFormValue(formData, "keytype");
      // get action
      std::string action = readFormValue(formData, "action");
      // get keystring
      std::string keystring = readFormValue(formData, "keystring");
      stripKeyString(keystring);
      // get message
      std::string message = readFormValue(formData, "message");
      // std::cout << "<p>" << message << "<p>\n";
      stripMessage(message);

      // trap for error and show user usage message
      if (cipher != "coltran" && cipher != "keytran" && cipher != "keysub"
         &&cipher != "Caesar" && cipher != "Affine" && cipher != "Vigenere"
         && cipher != "Gronsfeld" && cipher != "Porta" && cipher != "Beaufort"
         && cipher != "Variant" && cipher != "SlideI" && cipher != "SlideII" 
         && cipher != "SlideIII")
      {
         errMsg();
         printPageBottom();
         return 1;
      }
      if (action != "encrypt" && action != "decrypt") {
         errMsg();
         printPageBottom();
         return 1;
      }

      // check if user wants auto or progressive key encryption
      bool autokey = false, progkey = false;
      if (keytype == "auto") autokey = true;
      if (keytype == "prog") progkey = true;
      
      // check for empty, or too long, keystring
      if (keystring.length() == 0 || keystring.length() > C) {
         errMsg();
         printPageBottom();
         return 1;
      }


      // ready, steady, go
      Cipher machine;
      // read stdin to input buffer
      machine.readBuf(message);
      // process buffer in core
      machine.processBuf(cipher, action, keystring, autokey, progkey);
      // print html form
      std::cout << "<form action='cppciphers.cgi?step1' method ='POST' accept-charset='utf-8'>\n";
      // print cipher choice radio buttons
      std::cout << "Cipher\n";
      printButton("cipher", "coltran", "", true);
      printButton("cipher", "keytran", "", false);
      printButton("cipher", "Caesar", "", false);
      printButton("cipher", "Affine", "", false);
      printButton("cipher", "keysub", "", false);
      printButton("cipher", "Vigenere", "", false);
      printButton("cipher", "Gronsfeld", "", false);
      printButton("cipher", "Porta", "", false);
      printButton("cipher", "Beaufort", "", false);
      printButton("cipher", "Variant", "", false);
      printButton("cipher", "SlideI", "", false);
      printButton("cipher", "SlideII", "", false);
      printButton("cipher", "SlideIII", "", false);
      // print key options raio buttons
      std::cout << "<div>Key type (Affects only Vigenere, Beaufort and Variant.)</div>\n";
      printButton("keytype", "regular", "", true);
      printButton("keytype", "auto", "", false);
      printButton("keytype", "prog", "", false);
      // print encrypt/decrypt action radio buttons
      std::cout << "<div>Action</div>\n";
      printButton("action", "encrypt", "", true);
      printButton("action", "decrypt", "", false);
      // print key string textbox
      std::cout << "<div>Key: <input type = 'text' name = 'keystring' size='50' value = ''>";
      // print form submit button
      std::cout << " <input type='submit' value='SUBMIT'></div>\n";
      // print message area and message output buffer
      std::cout << "<p></p>\n";
      std::cout << "<textarea name = \"message\" cols = \"80\" rows = \"12\">\n";
      machine.writeBuf();
      std::cout << "</textarea>\n";
      std::cout << "</form>\n";

      // begin frequency analysis
      FrequencyAnalyzer freqan;
      freqan.cipherReport(machine);
      // print bottom of page
      printPageBottom();
   }   
   return 0;
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
