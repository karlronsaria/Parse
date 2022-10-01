#include "FileReader.h"

FileReader::FileReader(const std::string & fileName):
    _fileName(fileName)
{
    _stream.open(fileName.c_str());
}

FileReader::~FileReader() {
    _stream.close();
}

FileReader::FileReader(FileReader && other):
    _fileName(other._fileName),
    _stream(std::move(other._stream)) {}

FileReader & FileReader::operator=(FileReader && other) {
    _fileName = other._fileName;
    _stream = std::move(other._stream);
    return *this;
}

std::ifstream & FileReader::Stream() {
    return _stream;
}

const std::string & FileReader::FileName() const {
    return _fileName;
}

bool FileReader::New(const std::string & fileName, FileReader & reader) {
    reader = std::move(FileReader(fileName));
    return reader.Stream().is_open();
}

int GetDifference(
    std::istream & first,
    std::istream & secnd,
    std::string & lineFirst,
    std::string & lineSecnd
) {
    getline(first, lineFirst);
    getline(secnd, lineSecnd);
    return lineFirst.compare(lineSecnd);
}

int GetNextDifferentLine(
    std::istream & first,
    std::istream & secnd,
    std::string & lineFirst,
    std::string & lineSecnd
) {
    int index = 0;
    int result = GetDifference(first, secnd, lineFirst, lineSecnd);

    while (first && secnd) {
        if (result)
            return index;

        index = index + 1;
        result = GetDifference(first, secnd, lineFirst, lineSecnd);
    }

    return -1;
}