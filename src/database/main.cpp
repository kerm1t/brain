// https://www.tutorialspoint.com/sqlite/sqlite_c_cpp.htm


#include <stdio.h> // C
#include "sqlite3.h" 
#include <stdlib.h> // C
#include <iostream> // C++
#include <atlstr.h> // For CStringW and CStringA

CStringA UTF16toUTF8(const CStringW& utf16)
{
   CStringA utf8;
   int len = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, 0, 0);
   if (len>1)
   { 
      char *ptr = utf8.GetBuffer(len-1);
      if (ptr) WideCharToMultiByte(CP_UTF8, 0, utf16, -1, ptr, len, 0, 0);
      utf8.ReleaseBuffer();
   }
   return utf8;
}

CStringW UTF8toUTF16(const CStringA& utf8)
{
   CStringW utf16;
   int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
   if (len>1)
   { 
      wchar_t *ptr = utf16.GetBuffer(len-1);
      if (ptr) MultiByteToWideChar(CP_UTF8, 0, utf8, -1, ptr, len);
      utf16.ReleaseBuffer();
   }
   return utf16;
}

// argc = ncolumns
// argv[i] = column
static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   printf("\n");
   return 0;
}

void write_sql(sqlite3 *db) {
   char *zErrMsg = 0;
   int rc;
   /* Create SQL statement */
  char* sql = "INSERT INTO notes (ID,note) "  \
         "VALUES (2, 'Mörderschn1tte');";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Records created successfully\n");
   }
}

int main(int argc, char* argv[]) {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;

   rc = sqlite3_open("brain.db3", &db);
   
   // Greek letter pi
   std::cout << "Unicode: " << '\u03C0' << '\n';
    // Grinning face
//    std::cout << "Beyond BMP: " << '\U0001F600' << '\n';
    // Greek letter lambda
//    std::cout << "UTF-8: " << u8"\u03BB" << '\n';
    std::wcout << "UTF-8: " << "MörderSchn1tte" << '\n';

   char basicChar = 'a';
    wchar_t wideChar = L'\u00E9';
    char32_t utf8Char = U'\u03A9';
    char32_t utf16Char = U'\U0001F60A';
    char32_t utf32Char = U'\U0001F609';

    std::cout << "Basic character: " << basicChar
              << std::endl;
    std::wcout << "Wide character: " << wideChar
               << std::endl;
    std::cout << "UTF-8 character: " << utf8Char
              << std::endl;
    std::wcout << "UTF-16 character: " << utf16Char
               << std::endl;
    std::wcout << "UTF-32 character: " << utf32Char
               << std::endl;


   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stderr, "Opened database successfully\n");
   }
   
   write_sql(db);

   /* Create SQL statement */
   char* sql = "SELECT * from notes WHERE ID=2;";
   const char* data = "Callback function called";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   
   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Operation done successfully\n");
   }

   sqlite3_close(db);
}
