#ifndef MISC_H
#define MISC_H

#include <string>
#include <fstream>

namespace fl{

    inline bool fileExists (const std::string& name) {
        std::ifstream infile(name);
        return infile.good();
    }

    std::string removeExtension(const std::string &filename);
}

#endif
