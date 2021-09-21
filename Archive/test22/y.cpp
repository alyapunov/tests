#include <alya.h>
#include <memory>

class Y
{
public:
    Y() { COUTF(__func__); }
    ~Y() { COUTF(__func__); }
};

Y* get0() { return new Y; }



std::shared_ptr<Y> get() { return std::make_shared<Y>(); }


