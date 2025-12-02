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
    std::string line;

    // 先讀掉標題列
    std::getline(infile, line);

    int o, a, d, t;

    while (true) {
        if (!std::getline(infile, line)) break;
        if (line.empty()) continue;

        int num = 0;
        int order = 0;

        o = a = d = t = 0;

        for (size_t i = 0; i <= line.size(); i++) {
            char ch;

            // 最後一欄用 '\t' 作為結束符號（因為 line[i] 已經沒有字）
            if (i < line.size()) {
                ch = line[i];
            } else {
                ch = '\t';
            }

            // 數字 => 持續累積
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
            } 
            // 遇到分隔符（tab 或行尾） => 欄位結束
            else {
                if (order == 0) {
                    o = num;
                } else if (order == 1) {
                    a = num;
                } else if (order == 2) {
                    d = num;
                } else if (order == 3) {
                    t = num;
                }

                if (order == 3) break;  // 四欄都讀好了

                num = 0;
                order++;
            }
        }

        // 讀完四欄，丟進 queue
        enquene(o, a, d, t, 0, 0, 0, 0);
    }
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

  void printo() {
    Node *temp = head;
    std::cout << "OID CID Delay Departure \n";
    while ( temp != NULL ) {
        std::cout << temp->OID << "\t"
                << temp->Arrival << "\t"
                << temp->Duration << "\t"
                << temp->Timeout << "\n";
        temp = temp->next;
    }
  }

  void printc() {
    Node *temp = head;
    std::cout << "OID CID Delay Abort \n";
    while ( temp != NULL ) {
        std::cout << temp->OID << "\t"
                << temp->CID << "\t"
                << temp->Delay << "\t"
                << temp->Abort << "\n";
        temp = temp->next;
    }
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

  int onecook(Queue &cook, Queue &cancel, Queue &Timeout) {

    Node *temp = head;  // 輸入訂單（sorted401.txt）
    int total_delay = 0;
    while (temp != NULL) {

        // =============================
        // 1. 若等待佇列空 → IdleTime 跳到 arrival
        // =============================
        if (cook.size() == 0 && cook.now_time < temp->Arrival) {
            cook.now_time = temp->Arrival;
        }

        // =============================
        // 2. 等待佇列若已滿(≥3) → 取消（規則 4-1）
        // =============================
        if (cook.size() == 3) {
            int Abort = temp->Arrival;
            int Delay = 0;
            cancel.enquene(temp->OID, temp->Arrival, temp->Duration,
                           temp->Timeout, Abort, Delay, 0, 0);

            temp = temp->next;
            continue;
        }

        // =============================
        // 3. 等待佇列未滿 → 將新訂單放進等待佇列
        // =============================
        cook.enquene(temp->OID, temp->Arrival, temp->Duration,
                     temp->Timeout, 0, 0, 0, 0);

        // =============================
        // 4. 如果廚師此時空閒 → 從等待佇列取1筆來做
        // =============================
        while (cook.size() > 0 && cook.now_time <= temp->Arrival) {

            Node *job = cook.head;  // 尚未真正 Dequeue 前的第一筆

            // -------- (A) 取出時逾時 → 取消清單 --------
            if (job->Timeout < cook.now_time) {
                int Abort = cook.now_time;
                int Delay = Abort - job->Arrival;
                total_delay = total_delay + Delay;
                cancel.enquene(job->OID, job->Arrival, job->Duration,
                               job->Timeout, Abort, Delay, 0, 1);

                cook.dequene();
                continue;
            }

            // -------- (B) 可以開始製作 --------
            int startTime = cook.now_time;
            cook.now_time = cook.now_time + job->Duration;   // 完成時間

            // -------- (C) 做完後逾時 → Timeout 清單 --------
            if (job->Timeout < cook.now_time) {
                int Departure = cook.now_time;
                int Delay = startTime - job->Arrival;

                Timeout.enquene(job->OID, job->Arrival, job->Duration,
                                job->Timeout, 0, Delay, Departure, 1);
            }

            // 成功 → 不需記錄
            cook.dequene();
        }

        temp = temp->next;
    }

    // ==========================================
    // 5. 所有訂單讀完後，處理等待佇列內剩餘訂單
    // ==========================================
    while (cook.size() > 0) {

        Node *job = cook.head;

        if (job->Timeout < cook.now_time) {
            int Abort = cook.now_time;
            int Delay = Abort - job->Arrival;

            cancel.enquene(job->OID, job->Arrival, job->Duration, job->Timeout, Abort, Delay, 0, 1);

            cook.dequene();
            continue;
        }

        int startTime = cook.now_time;
        cook.now_time += job->Duration;

        if (job->Timeout < cook.now_time) {
            int Departure = cook.now_time;
            int Delay = startTime - job->Arrival;
            total_delay = total_delay + Delay;
            Timeout.enquene(job->OID, job->Arrival, job->Duration, job->Timeout, 0, Delay, Departure, 1);
        }

        cook.dequene();
    }
    return total_delay;
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
    //q1.Print_original(filename);
    std::ifstream infile2(filename);
    q1.load(infile2);
    // q1.printo();
    Queue cook;
    Queue cancel;
    Queue delay;
    int total_delay = q1.onecook(cook, cancel, delay);
    int file_number = 444;
    //cancel.write_file2(file_number, delay);
    cancel.printc();
    std::cout << "[Total Delay]" << total_delay;

    
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
