#include<list>
#include<mutex>
#include<thread>
#include<condition_variable>
#include<iostream>
using namespace std;


template<typename T>
class SyncQueue {
public:
    SyncQueue(int maxSize) : m_maxSize(maxSize), m_needStop(false) {}

    void Put(const T& x) {
        Add(x);
    }

    void Put(T&& x) {
        Add(std::forward<T>(x));
    }

    void Take(T& t) {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notEmpty.wait(locker, [this]{return m_needStop || NotEmpty();});
        if (m_needStop) {
            return;
        }
        t = m_queue.front();
        m_queue.pop_front();
        m_notFull.notify_one();
    }

    void Take(std::list<T>& list) {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notEmpty.wait(locker, [this]{return m_needStop || NotEmpty();});
        if (m_needStop) {
            return;
        }
        list = std::move(m_queue);
        m_notFull.notify_one();
    }

    void Stop() {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_needStop = true;

        // notify_all() 与 notify_one的区别
        m_notFull.notify_all();
        m_notEmpty.notify_all();
    }

    bool Empty() {
        std::unique_lock<std::mutex> locker(m_mutex);
        return m_queue.empty();
    }

    bool Full() {
        std::unique_lock<std::mutex> locker(m_mutex);
        return m_queue.size() == m_maxSize;
    }

    size_t Size() {
        std::unique_lock<std::mutex> locker(m_mutex);
        return m_queue.size();
    }

    // 为什么有size还要有count
    int Count() {
        return m_queue.size();
    }

    // 防止递归用锁，不在此加锁
    bool NotFull() const {
        bool isFull = m_queue.size() >= m_maxSize;
        if(isFull) cout<<"queue is full, please wait"<<endl;
        return !isFull;
    }

    // 防止递归用锁，不在此加锁
    bool NotEmpty() const {
        bool isEmpty = m_queue.size() == 0;
        //if(isEmpty) cout<<"queue is empty....thread id:"<<this_thread::get_id()<<endl;
        return !isEmpty;
    }

    template<typename F>
    void Add(F&& x) {
        std::unique_lock<std::mutex> locker(m_mutex);
        // 保持锁的条件，needStop或者不为空
        m_notFull.wait(locker, [this]{return m_needStop || NotFull();});
        if (m_needStop) {
            return;
        }
        m_queue.push_back(std::forward<F>(x));
        m_notEmpty.notify_one();
    }

private:
    std::list<T> m_queue; //队列缓冲区
    std::mutex m_mutex; // 互斥量
    std::condition_variable m_notEmpty; //条件变量用于通信
    std::condition_variable m_notFull;
    int m_maxSize; //队列最大容量，方式内存占用过多
    bool m_needStop; //停止标志
};
