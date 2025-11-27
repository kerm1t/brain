/*
  Database helper functions for SQLite

  CREATE TABLE notes (id integer, note text, created real, modified real, tags text, link text, img blob);
  
  ALTER TABLE notes ADD COLUMN topic TEXT;

*/
#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <iostream>
#include <stdio.h> // C
#include "sqlite3.h" 
#include <stdlib.h> // C
#include <atlstr.h> // For CStringW and CStringA

namespace db {

  sqlite3* db;
  std::string fld_value;
  std::vector<std::string> rows_val;

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

void escape_escape_sequences(std::string& str) {
  std::pair<char, char> const sequences[]{
    { '\a', 'a' },
    { '\b', 'b' },
    { '\f', 'f' },
    { '\n', 'n' },
    { '\r', 'r' },
    { '\t', 't' },
    { '\v', 'v' },
  };

  for (size_t i = 0; i < str.length(); ++i) {
    char* const c = str.data() + i;

    for (auto const seq : sequences) {
      if (*c == seq.first) {
        *c = seq.second;
        str.insert(i, "\\");
        ++i; // to account for inserted "\\"
        break;
      }
    }
  }
}

void escape_apostrophe(std::string& str) {
  std::pair<char, char> const sequences[]{
    { '\'', '"' }
    };

  for (size_t i = 0; i < str.length(); ++i) {
    char* const c = str.data() + i;

    for (auto const seq : sequences) {
      if (*c == seq.first) {
        *c = seq.second;
//        str.insert(i, "\\");
        ++i; // to account for inserted "\\"
        break;
      }
    }
  }
}
// argc = ncolumns
  // argv[i] = column
  static int callback(void* data, int argc, char** argv, char** azColName) {
    int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for (i = 0; i < argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    printf("\n");
    return 0;
  }
  
  static int callback_fld(void* data, int argc, char** argv, char** azColName) {

    fprintf(stderr, "%s: ", (const char*)data);
    fld_value = argv[0];
    return 0;
  }

  static int callback_rows(void* data, int argc, char** argv, char** azColName) {
    int i;
    fprintf(stderr, "%s: ", (const char*)data);

    for (i = 0; i < argc; i++) {
      rows_val.push_back(argv[i]);
//      rows_val.insert(rows_val.end(), argv[i], argv[i] + strlen(argv[i]));
    }
    return 0;
  }

  void read_sql() {
    char* zErrMsg = 0;
    int rc;

    /* Create SQL statement */
    char* sql = "SELECT * from notes WHERE ID=2;";
    const char* data = "Callback function called";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else {
      fprintf(stdout, "Operation done successfully\n");
    }

  }

  std::string sql_string(const std::string& sql) {
    char* zErrMsg = 0;
    int rc;

    /* Create SQL statement */
//    char* sql = "SELECT * from notes WHERE ID=2;";
    const char* data = "Callback function called";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback_fld, (void*)data, &zErrMsg);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else {
      fprintf(stdout, "Operation done successfully\n");
    }

    return fld_value;
  }

//  std::vector<std::string> sql_rows(const std::string& sql) {
  void sql_rows(const std::string & sql) {
    char* zErrMsg = 0;
    int rc;
    const char* data = "Callback function called";
    
    rows_val.clear();
    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback_rows, (void*)data, &zErrMsg);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else {
      fprintf(stdout, "Operation done successfully\n");
    }

//    return rows_val;
  }


  void write_sql(const std::string& note) {
    char* zErrMsg = 0;
    int rc;
    /* Create SQL statement */
    std::string sql = "INSERT INTO notes (ID,note) "  \
      "VALUES (5, '" + note + "');";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else {
      fprintf(stdout, "Records created successfully\n");
    }
  }

  void sql_update(const std::string& note) {
    char* zErrMsg = 0;
    int rc;
    std::string note_ = note;
    escape_apostrophe(note_);
    /* Create SQL statement */
    std::string sql = "UPDATE notes SET note = '" + note_ + "' WHERE ID = 5;";
//    sql = UTF16toUTF8(sql.c_str()).GetString();
//    escape_escape_sequences(sql);

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else {
      fprintf(stdout, "Records updated successfully\n");
    }
  }

  int db_open(const std::string dbname) {
    char* zErrMsg = 0;
    int rc;
    rc = sqlite3_open(dbname.c_str(), &db);
    if (rc) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
    }
    else {
      fprintf(stderr, "Opened database successfully\n");
    }
  }

  void db_close() {
    sqlite3_close(db);
  }


} // namespace db

#endif // DATABASE_HPP
