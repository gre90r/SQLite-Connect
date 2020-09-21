
#include <iostream>
#include "Sqlite.h" // my sqlite wrapper
#include "Config.h"

/**
 * test sqlite
 */
int main()  {
  gre90r::Sqlite sqlite(Config::DB_NAME);
  
  if (sqlite.isConnected()) {
    std::cout << "sqlite is working" << std::endl;
  } else {
    std::cerr << "sqlite is not initialized correctly"; 
  }

  return 0;
}
