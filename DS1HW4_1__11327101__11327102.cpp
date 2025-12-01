#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
class Queue {
 private:
  int now_time; // 閒置時間
  int Abort; // 取消時刻
  int Delay; // 延誤時間
  int Departure; // 完成時刻
  int CID = 0; // 廚師編號
  struct Node {
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
  void load(std::ifstream &infile) {
    char ch;
    std::string s;
    std::getline(infile, s);
    int num = 0;
    int order = 0;
    int o, a, d, t;
    infile.get(ch);
    while ( !infile.eof() ) {
      if ( ch == '\n' ) {
        order = 0;
        infile.get(ch);
      }

      if ( ch == '\t' ) {
        infile.get(ch);
      }
      
      if ( ch != '\n' ) {
        order++;
        num = 0;
        while ( ch != '\t' && ch != '\n' ) {
          num = (10 * num) + (ch - '0');
          infile.get(ch);
        }
      }

      if ( order == 1 ) {
        o = num;
      }

      if ( order == 2 ) {
        a = num;
      }

      if ( order == 3 ) {
        d = num;
      }

      if ( order == 4 ) {
        t = num;
        enquene(o, a, d, t);
      }
    }
    return;
  }

  

  // 安全版 turnindex (0-based)，index 非法回傳 nullptr
  Node* turnindex(int index) {
    Node* temp = head;
    int i = 0;
    while (i < index) {
      temp = temp->next;
      i++;
    }
    return temp;
  }

  // print list content (index from 0)
  void print() {
    Node* cur = head;
    int i = 0;
    while (cur) {
      std::cout << "(" << i << ") "
                << cur->OID << " "
                << cur->Arrival << " "
                << cur->Duration << " "
                << cur->Timeout << "\n";
      cur = cur->next;
      ++i;
    }
    std::cout << "-----\n";
  }

  void sell_sore2() {
    int size = len;
    if (size <= 1) {
      return;
    }
    int gap = size / 2;
    while (gap != 0) {
      for (int i = 0; i < size; i++) { // 跑下一組
        for (int j = i + gap; j < size; j = j + gap) { // 跑下一個這個組的成員
          Node* temp1 = turnindex(j);
          int tempOID      = temp1->OID;
          int tempArrival  = temp1->Arrival;
          int tempDuration = temp1->Duration;
          int tempTimeout  = temp1->Timeout;
          int k = j - gap;
          while (k >= 0) {
            Node* k_temp = turnindex(k);
            bool needmove = false;
            if (k_temp->Arrival > tempArrival) {
              needmove = true;
            } else if (k_temp->Arrival == tempArrival && k_temp->OID > tempOID) {
              needmove = true;
            }
            if (!needmove) {
              break;
            }
            Node* target = turnindex(k + gap);
            target->OID      = k_temp->OID;
            target->Arrival  = k_temp->Arrival;
            target->Duration = k_temp->Duration;
            target->Timeout  = k_temp->Timeout;
            k = k - gap;
          }
          Node* insert = turnindex(k + gap);
          if (insert != nullptr) {
            insert->OID      = tempOID;
            insert->Arrival  = tempArrival;
            insert->Duration = tempDuration;
            insert->Timeout  = tempTimeout;
          }
        }
      }
      gap = gap / 2;
    }
  }

  void write_file(int file_number) {
    std::string filename = "sorted" + std::to_string(file_number) + ".txt";
    std::ofstream outfile(filename);
    Node *temp = head;
    outfile << "OID Arrival Duration TimeOut\n";
    while ( temp != NULL ) {
        outfile << temp->OID << "\t"
                << temp->Arrival << "\t"
                << temp->Duration << "\t"
                << temp->Timeout << "\n";
        temp = temp->next;
    }
    outfile.close();
  }

  void Print_original(std::string filename) {
    std::ifstream infile(filename); // 讀檔
    char ch;
    while ( infile.get(ch) ) {
      std::cout << ch;
    }
    std::cout << std::endl;
    infile.close();
  }


}; // end Queue

struct AbortEntry {
  int OID; // 訂單編號
  int Abort; // 取消時刻
  int Delay; // 延誤時間
};

struct TimeoutEntry {
  int OID; // 訂單編號
  int Departure; // 完成時刻
  int Delay; // 延誤時間
};

void task1() {
  int file_number;
  std::cout << "Input a file number (e.g., 401, 402, 403, ...): ";
  std::cin >> file_number;
  std::string filename = "input" + std::to_string(file_number) + ".txt"; // 轉字串
  std::ifstream infile(filename); // 讀檔
  Queue q1;
  if (infile) {
    q1.Print_original(filename);
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream infile2(filename);
    q1.load(infile2);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Reading data: " << duration.count() << " us." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    q1.sell_sore2();
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Sorting data: " << duration.count() << " us." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    q1.write_file(file_number);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Writing data: " << duration.count() << " us." << std::endl;
    
  } else {
    std::cout << "input" << file_number << ".txt does not exist!";
  }
  return;
}

int main() {
  // 範例 1
  std::string filename;
  task1();
  return 0;
}
