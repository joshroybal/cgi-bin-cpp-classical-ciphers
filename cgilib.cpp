#include <cstdlib>
#include <cstring>
#include <cctype>
#include <algorithm>
#include "cgilib.hpp"

void readQueryString(char card[])
{
    strcpy(card, getenv("QUERY_STRING"));
}

// this will go into a basic cgi-bin form parsing library
std::string readFormValue(const std::string& formData, const std::string& formName)
{
   std::size_t pos = formData.find(formName);
   if (pos == std::string::npos)
      return "";
   std::size_t start = pos + formName.length() + 1;     
   std::string tmp = formData.substr(start);
   std::size_t length = tmp.find('&');
   std::string formValue = formData.substr(start, length);
   return formValue;
}

// alternative in place version of above function
void stripMessage(std::string& msg)
{
   // strip special POST form codes from message buffer
   std::string str = "%";
   std::string::size_type pos = msg.find(str);
   while (pos != std::string::npos) {
      msg.erase(pos, 3);
      pos = msg.find(str);
   }
   // strip blanks (encoded with +)
   str = "+";
   pos = msg.find(str);
   while (pos != std::string::npos) {
      msg.erase(pos, 1);
      pos = msg.find(str);
   }
   int n = 0;
   while (msg[n] != '\0') {
      if (!isalpha(msg[n])) {
         msg.erase(n, 1);
      }
      ++n;
   }
   std::transform(msg.begin(), msg.end(), msg.begin(), ::toupper);
}
