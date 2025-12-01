#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
class Queue {
 private:
  int now_time = 0; // 閒置時間
  
  struct Node {
    int OID;
    int Arrival; // 下單時間
    int Duration; // 製作時間
    int Timeout;  // 到期時刻
    int Abort; // 取消時刻
    int Delay; // 延誤時間
    int Departure; // 完成時刻
    int CID = 0; // 廚師編號
    Node* next;
    Node(int o, int a, int d, int t, int abort, int delay, int departure, int cid) {
      OID = o;
      Arrival = a;
      Duration = d;
      Timeout = t;
      Abort = abort;
      Delay = delay;
      Departure = departure;
      CID = cid;
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

  void enquene(int o, int a, int d, int t, int abort, int delay, int departure, int cid) {
    Node* n = new Node(o, a, d, t, abort, delay, departure, cid);
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
        enquene(o, a, d, t, 0, 0, 0, 0);
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

  void write_file2(int file_number, Queue timeout) {
    std::string filename = "One" + std::to_string(file_number) + ".txt";
    std::ofstream outfile(filename);
    Node *temp = head;
    outfile << "OID Delay Abort TimeOut\n";
    while ( temp != NULL ) {
        outfile << temp->OID << "\t"
                << temp->Delay << "\t"
                << temp->Abort << "\t"
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

  void onecook(Queue &cook, Queue &cancel, Queue &Timeout) {
    Node *temp = head;
    while ( temp != NULL ) {
      if ( cook.size() <= 4 ) {
        if ( cook.size() == 1 ) {
          if ( temp->Timeout >= cook.now_time ) { // 工作柱列
            if ( cook.now_time <= temp->Arrival ) {
              cook.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout, 0, 0, 0, 1);
              cook.now_time = temp->Arrival + temp->Duration;
              if ( cook.now_time > temp->Timeout ) { // 做完超過時間
                Timeout.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout, 0, 0, 0, 1);
              }
            }

            else {
              cook.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout, 0, 0, 0, 1);
              cook.now_time = cook.now_time + temp->Duration;
              if ( cook.now_time > temp->Timeout ) { // 做完超過時間
                Timeout.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout, 0, 0, 0, 1);
              }
            }
            cook.dequene();
          }
          
          else {
            int Abort = cook.now_time;
            int Delay = Abort - temp->Arrival;
            cancel.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout, Abort, Delay, 0, 1);
          }
        }// 工作柱列

        else {
          cook.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout, 0, 0, 0, 0);
        }
      }

      else {
        int Abort =  temp->Arrival;
        int Delay = 0;
        cancel.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout, Abort, Delay, 0, 1);
      }
      dequene();
      temp = temp->next;
    }
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

void task2() {
  int file_number;
  std::cout << "Input a file number (e.g., 401, 402, 403, ...): ";
  std::cin >> file_number;
  std::string filename = "sorted" + std::to_string(file_number) + ".txt"; // 轉字串
  std::ifstream infile(filename); // 讀檔
  Queue q1;
  if (infile) {
    q1.Print_original(filename);
    std::ifstream infile2(filename);
    q1.load(infile2);
    int cid = 0;
    Queue cook;
    Queue cancel;
    Queue delay;
    q1.onecook(cook, cancel, delay);
    int file_number = 444;
    cancel.write_file2(file_number, delay);

    
  } else {
    std::cout << "input" << file_number << ".txt does not exist!";
  }
  return;
}

int main() {
  // 範例 1
  std::string filename;
  //task1();
  task2();
  return 0;
}
