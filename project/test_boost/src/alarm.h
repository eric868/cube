#include <functional>

struct alarm_info
{
    int hour;
    int minute;
    int second;

    alarm_info()
        : hour(0)
        , minute(0)
        , second(0)
    {}
};

class alarm
{
public:
    alarm(int hour = 0, int minute = 0, int second = 0);
    ~alarm();

public:
    void set_bc(std::function<void(void)> bc);
    void on_timeout();

private:
    void calc_fire_period();
    void set_fire_time();

private:
    alarm_info ainfo_;
    long fire_time_;
    long fire_period_;

    std::function<void(void)> bc_;
};

