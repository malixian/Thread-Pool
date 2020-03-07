#include"ThreadPool.hpp"

void TestThreadPool() {
    ThreadPool tp;
    std::thread t1([&tp]{
        for (int i=0; i<10; i++) {
            auto tid = this_thread::get_id();
            cout<<"同步线程1添加任务"<<endl;
            tp.AddTask([tid] {
                cout<<"同步线程1正在执行任务"<<tid<<endl;
            });
        }
    });

    std::thread t2([&tp]{
        for (int i=0; i<10; i++) {
            auto tid = this_thread::get_id();
            cout<<"同步线程2添加任务"<<endl;
            tp.AddTask([tid] {
                cout<<"同步线程正在执行任务"<<tid<<endl;
            });
        }
    });

    this_thread::sleep_for(std::chrono::seconds(2));
    getchar();
    t1.join();
    t2.join();
    tp.Stop();
}

int main() {
    cout<<"begin test thread pool"<<endl;
    TestThreadPool();
    cout<<"thread pool test end"<<endl;
}