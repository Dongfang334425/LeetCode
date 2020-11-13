#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace::std;

class BoundedBlockingQueue {
private:
	queue<int> m_qe;
	int m_capacity;
	mutex mtx;
	condition_variable cv;
	
public:
	BoundedBlockingQueue(int capacity):m_capacity(capacity)
	{
	}

	void enqueue(int element)
	{
		unique_lock<mutex> lk(mtx);
		//if size() is equal to m_capacity, block the producer thread.
		//when the producer thread is notified from consumer thread, wake up cv to check. At this time,
		//size()<m_capacity, then proceed.
		cv.wait(lk, [this] {return (size() <= m_capacity); });
		m_qe.push(element);
		lk.unlock();
		//notify the consumer thread.
		cv.notify_all();
	}

	int dequeue()
	{
		unique_lock<mutex> lk(mtx);
		//if size() is 0, block the consumer thread.
		//when the consumer thread is notified from producer thread, wake up cv to check. At this time,
		//size()!=0, then proceed.
		cv.wait(lk, [this] {return(size() != 0); });
		int res = m_qe.front();
		m_qe.pop();
		lk.unlock();
		//notify the producer thread.
		cv.notify_one();
		return res;
	}

	int size()
	{
		return m_qe.size();
	}
};
//An example:
/*
1 producer
1 consumer
["BoundedBlockingQueue","enqueue","dequeue","dequeue","enqueue","enqueue","enqueue","enqueue","dequeue"]
[[2],[1],[],[],[0],[2],[3],[4],[]]
*/

BoundedBlockingQueue myBBQ(2);

void fProducer()
{
	
	myBBQ.enqueue(1);
	myBBQ.enqueue(0);
	myBBQ.enqueue(2);
	myBBQ.enqueue(3);
	myBBQ.enqueue(4);
}
void fConsumer()
{
	myBBQ.dequeue();
	myBBQ.dequeue();
	myBBQ.dequeue();
}

int main()
{
	
	thread 	producer(fProducer);
	thread consumer(fConsumer);
	producer.join();
	consumer.join();
	

	return system("pause");
}