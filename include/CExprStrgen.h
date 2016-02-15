#ifndef CExprStrgen_H
#define CExprStrgen_H

std::string CExprInsertEscapeCodes
             (const std::string &str);
bool        CExprStringToNumber
             (const std::string &str, uint *i, long &integer, double &real, bool &is_int);

#endif
