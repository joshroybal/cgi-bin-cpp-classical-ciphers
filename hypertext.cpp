#include <iostream>
#include "hypertext.hpp"

// print top of html page
void printPageTop(const std::string& header)
{
   std::cout << "<!DOCTYPE HTML>\n";
   std::cout << "<html>\n";
   std::cout << "<head>\n";
   std::cout << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
   std::cout << "<link rel='stylesheet' media='all'  href='/includes/gradienttable.css'>\n";
   std::cout << "<title>" << header << "</title>\n";
   std::cout << "</head>\n";
   std::cout << "<body>\n";
   std::cout << "<header><p>" << header << "</p></header>\n";
   std::cout << "<p></p>\n";
   std::cout << "<div><a href=\"/index.php\">Home</a></div>\n";
   std::cout << "<p></p>\n";
}

// print form radio button
void printButton(const std::string& name, const std::string& value,
                  const std::string& msg, bool checked)
{
   std::cout << "<div><label><input type='radio' name='" << name << "' value='" << value;
   if (checked)
      std::cout << "' checked>";
   else
      std::cout << "'>";
   std::cout << value;
   std::cout << "</label></div>\n";
}

// print bottom of html page
void printPageBottom()
{
   std::cout << "<p></p>\n";
   std::cout << "<div><a href=\"/index.php\">Home</a> | <a href=\"/cppciphers.html\">Back</a></div>\n";
   std::cout << "<footer><p>Copyright (c) 2017 Josh Roybal.</p></footer>\n";
   std::cout << "<p></p>\n";
   std::cout << "</body>\n";
   std::cout << "</html>\n";
}
