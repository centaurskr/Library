///
/// String n cmpare
/// @file strcmp.c
/// @date 2024. 01. 02. (화) 14:56:36 KST
/// @author Cento 
///
#include <string.h>
////////////////////////////////////////////////////////////////////////////////
/// String(null terminated)을 비교한다. a vs b and b vs a  
/// @fn       int Stringcmp(char *a, char *b) 
/// @brief    string 비교
/// @param    a A string
/// @param    b B string
/// @return   같음 0 
/// @return   다름 -n or n
////////////////////////////////////////////////////////////////////////////////
int Stringcmp(char *a, char *b)
{
	if(strlen(a) != strlen(b)) return -1;
	return strncmp(a, b, strlen(a));
}
