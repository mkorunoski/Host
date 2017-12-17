#ifndef FILES_H
#define FILES_H

#include <fstream>
#include <string>
#include <vector>

class Files
{
public:
    explicit Files(size_t maxNumFiles) : mMaxNumFiles(maxNumFiles) { }

    void newFile(const std::string &);
    void write(double);
    std::string currFilename();

private:
    size_t mMaxNumFiles;
    std::ofstream mCurrFile;
    std::vector<std::string> mFiles;

};

#endif // FILES_H
