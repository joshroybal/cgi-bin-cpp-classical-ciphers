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

// strip cgi control characters from keystring
void stripKeyString(std::string& keystring)
{
   for (int i = 0; i < keystring.length(); i++)
      if (!isalnum(keystring[i]))
         keystring[i] = ' ';
   std::transform(keystring.begin(), keystring.end(), keystring.begin(), ::toupper);
}

// strip cgi control characters from message
void stripMessage(std::string& msg)
{
   // strip special POST form codes from message buffer
   std::string str = "%";
   std::string::size_type pos = msg.find(str);
   while (pos != std::string::npos) {
      msg.erase(pos, 3);
      pos = msg.find(str);
   }
   
   // strip non-alphabetic characters from message
   size_t i = 0;
   size_t len = msg.length();
   while (i < len) {
      if (!isalpha(msg[i]) || msg[i] == ' ' || msg[i] == '\0') {
         msg.erase(i, 1);
         --len;
      } 
      else
         ++i;
   }

   std::transform(msg.begin(), msg.end(), msg.begin(), ::toupper);
}
