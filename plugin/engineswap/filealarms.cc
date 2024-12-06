#include "filealarms.h"

namespace rocksdb{
void raiseAlarm(std::string alarm){
    std::cout << "please be alarmed, " << alarm << " was called and marked as not implemented\n";
    std::fstream fs;
    fs.open(ALARM_FILE_DUMMY, std::ios::app);
    if (!fs.is_open())
    {
        fs << alarm << "\n";
    }
    else{
        std::cout << "failed to write";
    }
    fs.close();
}

}