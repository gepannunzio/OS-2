#include <mutex>

#ifndef LIGHTSWITCH_HPP
#define LIGHTSWITCH_HPP

class Lightswitch {
    private:
        unsigned int _counter;
        std::mutex _m;

    public:
    
    Lightswitch() : _counter(0) {};
    
    void lock(std::mutex &roomEmpty) {
        std::unique_lock<std::mutex> lock(_m);
        _counter++;
        if (_counter == 1)
            roomEmpty.lock();
        lock.unlock();
    };

    void unlock(std::mutex &roomEmpty) {
        std::unique_lock<std::mutex> lock(_m);
        _counter--;
        if (_counter == 0)
            roomEmpty.unlock();
        lock.unlock();
    };
};

#endif