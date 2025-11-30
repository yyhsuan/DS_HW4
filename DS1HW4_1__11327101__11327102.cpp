#include <iostream>
#include <fstream>
#include <string>
 
  
  
  

class Queue {
 private:
  struct Node{
    // 這裡僅提供 Queue 的骨架（任務一不需要完整功能）
    int val;
    Node* next;
    Node(int v): val(v), next(nullptr) {}
  };
  Node *head, *tail;
  int len;
 public:
  Queue() {
   head = nullptr;
   tail = nullptr;
   len = 0;
  }
  ~Queue() {
    while (!empty()) {
      dequene();
    }
  }
  bool empty() {
    return head == nullptr;
  }
  void enquene(int v) {
    Node* n = new Node(v);
    if (tail) tail->next = n;
    tail = n;
    if (!head) head = n;
    ++len;
  }
  void dequene() {
    if (!head) return;
    Node* t = head;
    head = head->next;
    if (!head) tail = nullptr;
    delete t;
    --len;
  }
  int size() { 
   return len; 
  }

};

struct Order {
  int OID;
  int Arrival;
  int Duration;
  int Timeout;
};

void Start() {
  std::cout << "*** (^_^) Data Structure (^o^) ***" << std::endl;
  std::cout << "** Simulate FIFO Queues by SQF ***" << std::endl;
  std::cout << "* 0. Quit                        *" << std::endl;
  std::cout << "* 1. Sort a file                 *" << std::endl;
  std::cout << "* 2. Simulate one FIFO queue     *" << std::endl;
  std::cout << "* 3. Simulate two queues by SQF  *" << std::endl;
  std::cout << "* 4. Simulate some queues by SQF *" << std::endl;
  std::cout << "**********************************" << std::endl;
  std::cout << "Input a command(0, 1, 2, 3, 4): ";
}

int main() {
  std::string filename;
  Start();

  return 0;
}
