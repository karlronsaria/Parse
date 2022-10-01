#pragma once
#ifndef _FILEREADER_H
#define _FILEREADER_H

#include <fstream>
#include <string>

class FileReader {
    private:
        std::ifstream _stream;
        std::string _fileName;
        FileReader(const std::string & fileName);
    public:
        virtual ~FileReader();
        FileReader() = default;
        FileReader(const FileReader &) = delete;
        FileReader & operator=(const FileReader &) = delete;
        FileReader(FileReader &&);
        FileReader & operator=(FileReader &&);
        std::ifstream & Stream();
        const std::string & FileName() const;
        static bool New(const std::string & fileName, FileReader & reader);
};

int GetDifference(
    std::istream & first,
    std::istream & secnd,
    std::string & lineFirst,
    std::string & lineSecnd
);

int GetNextDifferentLine(
    std::istream & first,
    std::istream & secnd,
    std::string & lineFirst,
    std::string & lineSecnd
);

#endif