/**
 * $Id: kvfile.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * @file kvfile.cpp
 * @brief Key-Value file for config data
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

/**
 * @todo replace atoi/strtoX by <string> header functions stoX.
 */

#include <utils/kvfile.h>

#include <cstdlib> // remove when switched to stoX functions
#include <fstream>
#include <stdexcept>

using namespace std;

#define BUFFLEN 256



namespace tmssim {


  KVFile::KVFile(std::string& filename)
    : map<string, string>()
  {
    readConfig(filename);
  }

  
  KVFile::KVFile(const char *filename)
    : map<string, string>()
  {
    readConfig(string(filename));
  }

  
  KVFile::~KVFile() {
  }


  bool KVFile::containsKey(const std::string& key) const {
    bool rv = true;
    try {
      const string& value = this->at(key);
    }
    catch (const out_of_range& oor) {
      rv = false;
    }
    return rv;
  }


  uint8_t KVFile::getUInt8(const string& key) {
    string& str = (*this)[key];
    uint32_t val = atoi(str.c_str());
    return (uint8_t) val;
  }
  

  uint16_t KVFile::getUInt16(const string& key) {
    string& str = (*this)[key];
    uint32_t val = atoi(str.c_str());
    return (uint16_t) val;
  }


  uint32_t KVFile::getUInt32(const string& key) {
    string& str = (*this)[key];
    uint32_t val = atoi(str.c_str());
    return (uint32_t) val;
  }
  

  uint64_t KVFile::getUInt64(const string& key) {
    string& str = (*this)[key];
    uint64_t val = atoll(str.c_str());
    return val;
  }
  

  int8_t KVFile::getInt8(const string& key) {
    string& str = (*this)[key];
    int32_t val = atoi(str.c_str());
    return (int8_t) val;
  }
  
  int16_t KVFile::getInt16(const string& key) {
    string& str = (*this)[key];
    int32_t val = atoi(str.c_str());
    return (int16_t) val;
  }
  
  
  int32_t KVFile::getInt32(const string& key) {
    string& str = (*this)[key];
    int32_t val = atoi(str.c_str());
    return (int32_t) val;
  }
  
  
  int64_t KVFile::getInt64(const string& key) {
    string& str = (*this)[key];
    int64_t val = atoll(str.c_str());
    return val;
  }
  

  float KVFile::getFloat(const std::string& key) {
    string& str = (*this)[key];
    float val = strtof(str.c_str(), NULL);
    return val;
  }


  double KVFile::getDouble(const std::string& key) {
    string& str = (*this)[key];
    double val = strtof(str.c_str(), NULL);
    return val;
  }
  

  const std::string& KVFile::getString(const std::string& key) {
    return (*this)[key];
  }

  
  void KVFile::readConfig(const string& filename) {
    ifstream fin(filename.c_str());
    string line;
    char buffer[BUFFLEN];
    uint32_t state;
    while (!fin.eof()) {
      fin.getline(buffer, BUFFLEN);
      state = fin.rdstate();
      if (state & ios::eofbit)
	break;
      if (state != 0) {
	fin.close();
	throw KVFileException(state);
      }
      line = buffer;
      trim(line);
      // ignore blank lines...
      if (line.length() == 0)
	continue;
      // ...and comments
      if (line[0] == '#')
	continue;
      //cout << line << endl;;
      storeLine(line);
    }
    fin.close();
  }
  
  void KVFile::storeLine(const string& line) {
    size_t pos;
    string key, value;
    pos = line.find("=");
    key = line.substr(0, pos);
    trim(key);
    value = line.substr(pos+1, line.length()-pos);
    trim(value);
    (*this)[key] = value;
  }
  
  
  void KVFile::trim(string &str) {
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");  
    // if all spaces or empty return an empty string
    if(( string::npos == startpos ) || ( string::npos == endpos))
      str = "";
    else
      str = str.substr( startpos, endpos-startpos+1 );
  }
  

} // NS tmssim
