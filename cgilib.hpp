#ifndef CGI_LIB__
#define CGI_LIB__

#include <string>

void readQueryString(char []);
std::string readFormValue(const std::string& formData, const std::string& formName);
void stripMessage(std::string&);

#endif
