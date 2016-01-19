/**
 * $Id: kvfile.h 685 2014-09-12 08:47:42Z klugeflo $
 * @file kvfile.h
 * @brief Key-Value file for config data
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_KVFILE_H
#define UTILS_KVFILE_H 1

#include <map>
#include <string>
#include <stdint.h>

namespace tmssim {

  /**
   * @brief Class for reading a key-value file.
   *
   * This class reads/maps Unix-style key-value files. File entries have
   * have the form "KEY=VALUE". Comment lines start with '#'.
   */
  class KVFile : public std::map<std::string, std::string> {
  
  public:
    //PerConfig();
    KVFile(std::string& filename);
    KVFile(const char *filename);
    ~KVFile();

    bool containsKey(const std::string& key) const;

    uint8_t getUInt8(const std::string& key);
    uint16_t getUInt16(const std::string& key);
    uint32_t getUInt32(const std::string& key);
    uint64_t getUInt64(const std::string& key);
    
    int8_t getInt8(const std::string& key);
    int16_t getInt16(const std::string& key);
    int32_t getInt32(const std::string& key);
    int64_t getInt64(const std::string& key);

    float getFloat(const std::string& key);
    double getDouble(const std::string& key);
    
    const std::string& getString(const std::string& key);
    
  private:
    void readConfig(const std::string& filename);
    void storeLine(const std::string& line);
    static void trim(std::string &str);
    
  };
  
  
  class KVFileException {
  public:
  KVFileException(uint32_t err) : error(err) {}
  public:
    uint32_t error;
  };
  

} // NS tmssim

#endif // !UTILS_KVFILE_H
