#include"ThreadPool.hpp"

void TestThreadPool() {
    ThreadPool tp;
    std::thread t1([&tp]{
        for (int i=0; i<10; i++) {
            auto tid = this_thread::get_id();
            tp.AddTask([tid] {
                cout<<"同步线程1的线程ID:"<<tid<<endl;
            });
        }
    });

    std::thread t2([&tp]{
        for (int i=0; i<10; i++) {
            auto tid = this_thread::get_id();
            tp.AddTask([tid] {
                cout<<"同步线程2的线程ID:"<<tid<<endl;
            });
        }
    });

    this_thread::sleep_for(std::chrono::seconds(2));
    getchar();
    tp.Stop();
    t1.join();
    t2.join();
}

int main() {
    cout<<"begin test thread pool"<<endl;
    TestThreadPool();
    cout<<"thread pool test end"<<endl;
}