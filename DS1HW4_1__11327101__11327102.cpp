#include <iostream>
#include <fstream>
#include <string>

class Queue {
 private:
  struct Node{
    int OID;
    int Arrival;
    int Duration;
    int Timeout;
    Node* next;
    Node(int o, int a, int d, int t) {
      OID = o;
      Arrival = a;
      Duration = d;
      Timeout = t;
      next = nullptr;
    }
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
  void enquene(int o, int a, int d, int t) {
    Node* n = new Node(o, a, d, t);
    if (tail) {
      tail->next = n;
    }
    tail = n;
    if (!head) {
      head = n;
    }
    len++;
  }
  void dequene() {
    if (!head) {
     return;
    }
    Node* t = head;
    head = head->next;
    if (!head) {
     tail = nullptr;
    }
    delete t;
    len--;
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

void Print_original(Order* arr, int n) {
  std::cout << std::endl;
  std::cout << std::setw(8) << "OID" << std::setw(10) << "Arrival" 
            << std::setw(10) << "Duration" << std::setw(12) << "TimeOut" << std::endl;
  for (int i = 0; i < n; i++) {
    std::cout << "(" << i+1 << ")";
    std::cout << std::setw(6) << arr[i].OID
              << std::setw(8) << arr[i].Arrival
              << std::setw(8) << arr[i].Duration
              << std::setw(10) << arr[i].Timeout
              << std::endl;
  }
  std::cout << std::endl;
}

void task1() {
  int file_number;
  std::cout << "Input a file number (e.g., 401, 402, 403, ...): 401";
  std::cin >> file_number;
  filename = "input" + std::to_string(file_number) + ".txt"; // 轉字串
  std::ifstream infile(filename); // 讀檔
  if (infile) {


  } else {
    std::cout << "input" << file_number << ".txt does not exist!";
  }
 
}

int main() {
  std::string filename;
  Start();
 
  return 0;
}
