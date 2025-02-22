#ifndef STREAM_H
#define STREAM_H

#include <queue>
#include <string>

// base stream class
class Stream {
public:
    virtual int available() = 0;
    virtual int read() = 0;
    virtual void write(const std::string& data) = 0;

    virtual ~Stream() = default;
};

class MockStream : public Stream {
public:
    MockStream() = default;

    int available() override {
        return buffer.size();
    }

    int read() override {
        if (buffer.empty()) {
            return -1;
        }
        char c = buffer.front();
        buffer.pop();
        return c;
    }

    void write(const std::string& data) override {
        for (char c : data) {
            buffer.push(c);
        }
    }

private:
    std::queue<char> buffer;
};

#endif
