#pragma once

namespace MR {

template <typename T>
class UUID {
public:
    UUID() : uuid_(uuid_counter_++) {}

    inline int get() const { return uuid_; }

private:
    int uuid_;
    static int uuid_counter_;
};

template <typename T>
int UUID<T>::uuid_counter_ = 0;

}  // namespace MR