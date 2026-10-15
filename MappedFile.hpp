//
// Created by Lukas on 14.10.2026.
//

#ifndef ITCH_PARSER_MAPPEDFILE_HPP
#define ITCH_PARSER_MAPPEDFILE_HPP


#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

class MappedFile
{
private:
    void* data_ = MAP_FAILED;
    std::size_t size_ = 0;
public:
    explicit MappedFile (const std::string& path)
    {
        int fd = open(path.c_str(), O_RDONLY);
        if (fd == -1)
            throw std::runtime_error("Couldn't open file: " + path);
        struct stat st {};
        if (fstat(fd, &st) == -1)
        {
            close (fd);
            throw std::runtime_error("Couldn't stat file: " + path);
        }
        size_ = static_cast<std::size_t>(st.st_size);

        data_ = mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd, 0);
        close (fd);

        if (data_ == MAP_FAILED)
            throw std::runtime_error("Couldn't map file: " + path);
    }

    ~MappedFile()
    {
        if (data_ != MAP_FAILED)
            munmap(data_, size_);
    }

    MappedFile(const MappedFile&) = delete;
    MappedFile& operator = (const MappedFile&) = delete;

    const uint8_t* data() const {return  static_cast<const uint8_t*>(data_); }
    std::size_t size() const { return size_;}
};

#endif //ITCH_PARSER_MAPPEDFILE_HPP
