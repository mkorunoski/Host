#include "files.h"

#include <sstream>

void Files::newFile(const std::string &filename)
{
    if (!mFiles.empty())
    {
        mCurrFile.close();

        if (mFiles.size() >= mMaxNumFiles)
        {
            remove(mFiles[0].c_str());
            mFiles.erase(mFiles.begin());
        }
    }

    mCurrFile = std::ofstream(filename);
    mFiles.push_back(filename);
}

void Files::write(double value)
{
    mCurrFile << value << std::endl;t
}

std::string Files::currFilename()
{
     return mFiles.back();
}
