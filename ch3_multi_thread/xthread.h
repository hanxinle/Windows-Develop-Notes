
class xthread {
public:
    bool start();
    void wait();
    virtual void usr_define_task() = 0; //继承类要实现
    xthread();
    virtual ~xthread();

private:
    unsigned int r_thread_;
};

