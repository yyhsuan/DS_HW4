// 11327101 陳怡瑄 11327102 林姿妤
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cmath>
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

  void write_file(std::string file_number) {
    std::string filename = "sorted" + file_number + ".txt";
    std::ofstream outfile(filename);
    Node *temp = head;
    outfile << "OID\tArrival\tDuration\tTimeOut\n";
    while ( temp != NULL ) {
        outfile << temp->OID << "\t"
                << temp->Arrival << "\t"
                << temp->Duration << "\t"
                << temp->Timeout << "\n";
        temp = temp->next;
    }
    return;
  }

  void printo() {
    Node *temp = head;
    std::cout << "OID CID Delay Departure\n";
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

  void printd() {
    Node *temp = head;
    std::cout << "\tOID\tCID\tDelay\tDeparture\n";
    while ( temp != NULL ) {
        std::cout << temp->OID << "\t"
                << temp->CID << "\t"
                << temp->Delay << "\t"
                << temp->Departure << "\n";
        temp = temp->next;
    }
  }

  void write_file2(std::string file_number, Queue &timeout, int total, int size) {
    std::string filename = "One" + file_number + ".txt";
    std::ofstream outfile(filename);

    Node *abort_temp = head;
    Node *timeout_temp = timeout.head;

    int i = 1;

    outfile << "\t[Abort List]\n";
    outfile << "\tOID\tCID\tDelay\tAbort\n";

    while (abort_temp != NULL) {
        outfile << "[" << i << "]\t"
                << abort_temp->OID << "\t"
                << abort_temp->CID << "\t"
                << abort_temp->Delay << "\t"
                << abort_temp->Abort << "\n";
        abort_temp = abort_temp->next;
        i++;
    }

    i = 1;
    outfile << "\n\t[Timeout List]\n";
    outfile << "\tOID\tCID\tDelay\tDeparture\n";

    while (timeout_temp != NULL) {
        outfile << "[" << i << "]\t"
                << timeout_temp->OID << "\t"
                << timeout_temp->CID << "\t"
                << timeout_temp->Delay << "\t"
                << timeout_temp->Departure << "\n";
        timeout_temp = timeout_temp->next;
        i++;
    }

    outfile << "\n[Total Delay]\n";
    outfile << total << " min.\n";
    outfile << "[Failure Percentage]\n";
    double fail = len + timeout.len;
    fail = (double)fail / (double)size * 100.00;
    fail = floor(fail * 100) / 100;
    outfile << fail << " %\n";
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
    bool move = false;
    Node *temp = head;  // 輸入訂單（sorted401.txt）
    int total_delay = 0;

    while (temp != NULL) {
        move = false;
        // 1. Idle → 跳到 arrival
        if (cook.size() == 0 && cook.now_time < temp->Arrival) {
            cook.now_time = temp->Arrival;
        }

        // 2. cook queue 滿（size==3）→ 取消
        if (cook.size() == 3) {
            if ( temp->Duration > 0 && temp->Arrival + temp->Duration <= temp->Timeout ) {
              int Abort = temp->Arrival;
              int Delay = 0;
              total_delay = total_delay + Delay;
              cancel.enquene(temp->OID, temp->Arrival, temp->Duration,
                           temp->Timeout, Abort, Delay, 0, 0);

              temp = temp->next;
              move = true;
            }

            else {
              temp = temp->next;
              len--;
              move = true;
            }
        }

        // 3. 接收訂單（一定合法）
        if ( !move ) {
          if ( temp->Duration > 0 && temp->Arrival + temp->Duration <= temp->Timeout ) {
            cook.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout, 0, 0, 0, 1);
            temp = temp->next;
            move = true;
            if ( temp->Duration <= 0 || temp->Arrival + temp->Duration > temp->Timeout ) {
              temp = temp->next;
              len--;
              move = true;
            }
          }

          else {
            temp = temp->next;
            len--;
            move = true;
          }
        }
        // -------------------------------------------------
        // 4. 廚師若空閒 → 做工作
        // -------------------------------------------------
        while (cook.size() > 0 && temp != NULL && cook.now_time <= temp->Arrival) {

            Node *job = cook.head;

            // (A) job 取出時逾時
            if (job->Timeout < cook.now_time) {
                int Abort = cook.now_time;
                int Delay = Abort - job->Arrival;
                total_delay = total_delay + Delay;

                cancel.enquene(job->OID, job->Arrival, job->Duration,
                               job->Timeout, Abort, Delay, 0, 1);

                cook.dequene();
                continue;
            }

            // (B) 開始做菜
            int startTime = cook.now_time;

            if (cook.now_time < job->Arrival) {
              cook.now_time = job->Arrival;
            }

            int duration_time = job->Duration;

            // 在做菜期間：可能有新的訂單到達
            for (int i = 0; i < duration_time; i++) {

                // 新訂單到達
                while (temp != NULL && temp->Arrival == cook.now_time) {
                    if ( temp->Duration <= 0 || temp->Arrival + temp->Duration > temp->Timeout ) {
                      temp = temp->next;
                      len--;
                    }
                    // cook queue 滿 → 新訂單取消
                    if (cook.size() == 4) {
                        cancel.enquene(temp->OID, temp->Arrival, temp->Duration,
                                       temp->Timeout, temp->Arrival, 0, 0, 0);
                        temp = temp->next;
                        continue;
                    }

                    // 新訂單加入 cook queue
                    cook.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout, 0, 0, 0, 1);
                    temp = temp->next;
                }

                // 時間流逝 1 單位
                cook.now_time++;
            }

            // (C) 做完後判斷是否 timeout
            if (job->Timeout < cook.now_time) {
                int Departure = cook.now_time;
                int Delay = startTime - job->Arrival;
                total_delay = total_delay + Delay;

                Timeout.enquene(job->OID, job->Arrival, job->Duration, job->Timeout, 0, Delay, Departure, 1);
            }

            cook.dequene();
        }

        // 若這筆 temp 在這圈沒被動過 → 移到下一筆
        if (!move && temp != NULL) {
          temp = temp->next;
        }
    }

    // -------------------------------------------------
    // 5. 所有輸入處理完 → cook queue 剩下的做完
    // -------------------------------------------------
    while (cook.size() > 0) {

        Node *job = cook.head;

        if ( job->Duration <= 0 || job->Arrival + job->Duration > job->Timeout ) {
          cook.dequene();
          len--;
          continue;
        }
        // timeout → cancel
        if (job->Timeout < cook.now_time) {
            int Abort = cook.now_time;
            int Delay = Abort - job->Arrival;
            total_delay = total_delay + Delay;
            cancel.enquene(job->OID, job->Arrival, job->Duration,
                           job->Timeout, Abort, Delay, 0, 1);

            cook.dequene();
            continue;
        }

        // 做最後的訂單
        int startTime = cook.now_time;
        if (cook.now_time < job->Arrival) {
            cook.now_time = job->Arrival;
        }

        cook.now_time = cook.now_time + job->Duration;

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



  // --- 新增：雙佇列模擬（twocook） ---
  // cook1, cook2: 兩位廚師的佇列（呼叫時由外部傳入）
  // cancel: 取消清單（CID 記 0 若未進入佇列即被取消）
  // Timeout: 逾時清單
  // 回傳值：total_delay（總延誤時間）
    int twocook(Queue &cook1, Queue &cook2, Queue &cancel, Queue &Timeout) {
    const int MAXQ = 3; // 每個佇列最多 3 筆訂單
    Node *it = head;
    int total_delay = 0;
    if (it == nullptr) return 0;

    // 主時間軸，採事件驅動：下一事件是 next arrival 或 next completion
    int cur_time = it->Arrival;
    // 將兩位廚師的 now_time 初始化到第一到達
    if (cook1.now_time < cur_time) cook1.now_time = cur_time;
    if (cook2.now_time < cur_time) cook2.now_time = cur_time;

    // 追蹤目前每位廚師正在處理的 job 的結束時間；-1 表示無工作
    int finish1 = -1, finish2 = -1;
    int start1 = -1, start2 = -1; // 各 job 的實際開始時間（用於 Delay 計算）

    // 幫助函式：把到 cur_time 的 arrivals 一個一個分配（依題意的 SQF）
    auto handle_arrivals_at = [&](int t) {
      while (it != nullptr && it->Arrival == t) {
        // 忽略非法訂單
        if (it->Duration <= 0 || it->Arrival + it->Duration > it->Timeout) {
          it = it->next;
          continue;
        }
        // 判斷閒置：佇列空且 now_time <= arrival
        bool idle1 = (cook1.size() == 0 && cook1.now_time <= it->Arrival);
        bool idle2 = (cook2.size() == 0 && cook2.now_time <= it->Arrival);

        if (idle1 && !idle2) {
          cook1.enquene(it->OID, it->Arrival, it->Duration, it->Timeout, 0, 0, 0, 1);
        } else if (!idle1 && idle2) {
          cook2.enquene(it->OID, it->Arrival, it->Duration, it->Timeout, 0, 0, 0, 2);
        } else if (idle1 && idle2) {
          // both idle -> choose 1
          cook1.enquene(it->OID, it->Arrival, it->Duration, it->Timeout, 0, 0, 0, 1);
        } else {
          // both not idle
          if (cook1.size() < MAXQ || cook2.size() < MAXQ) {
            // choose shorter queue; tie -> cook1
            if (cook1.size() <= cook2.size()) {
              cook1.enquene(it->OID, it->Arrival, it->Duration, it->Timeout, 0, 0, 0, 1);
            } else {
              cook2.enquene(it->OID, it->Arrival, it->Duration, it->Timeout, 0, 0, 0, 2);
            }
          } else {
            // both full -> immediate cancel (CID = 0)
            int Abort = it->Arrival;
            int Delay = 0;
            cancel.enquene(it->OID, it->Arrival, it->Duration, it->Timeout, Abort, Delay, 0, 0);
          }
        }
        it = it->next;
      }
    };

    // 初始處理：把第一批同時間到的訂單放進佇列
    handle_arrivals_at(cur_time);

    // 主要事件迴圈
    while (it != nullptr || cook1.size() > 0 || cook2.size() > 0 || finish1 != -1 || finish2 != -1) {
      // 若某廚師無工作且佇列有工作 -> 取出佇列頭開始做（設定 start/finish）
      if (finish1 == -1 && cook1.size() > 0) {
        Node *job = cook1.head;
        int s = cook1.now_time;
        if (s < job->Arrival) s = job->Arrival;
        if (s < cur_time) s = cur_time;
        start1 = s;
        finish1 = s + job->Duration;
        cook1.now_time = finish1;
      }
      if (finish2 == -1 && cook2.size() > 0) {
        Node *job = cook2.head;
        int s = cook2.now_time;
        if (s < job->Arrival) s = job->Arrival;
        if (s < cur_time) s = cur_time;
        start2 = s;
        finish2 = s + job->Duration;
        cook2.now_time = finish2;
      }

      // 下一個事件時間：下一到達 or 最早完成
      int nextArrival = (it != nullptr) ? it->Arrival : INT_MAX;
      int nextFinish = INT_MAX;
      if (finish1 != -1) nextFinish = std::min(nextFinish, finish1);
      if (finish2 != -1) nextFinish = std::min(nextFinish, finish2);
      int nextTime = std::min(nextArrival, nextFinish);

      // 若下一事件是到達且等於目前時間 -> 先處理到達（避免漏判）
      if (nextTime == cur_time && nextArrival == cur_time) {
        handle_arrivals_at(cur_time);
        // 重新計算下一事件
        nextArrival = (it != nullptr) ? it->Arrival : INT_MAX;
        nextFinish = INT_MAX;
        if (finish1 != -1) nextFinish = std::min(nextFinish, finish1);
        if (finish2 != -1) nextFinish = std::min(nextFinish, finish2);
        nextTime = std::min(nextArrival, nextFinish);
      }

      if (nextTime == INT_MAX) break; // no more events

      // 如果下一個事件是完成，先前進到那個時間並處理完成（可能兩個同時完成）
      // 若下一事件是到達且早於完成，先前進到到達並處理到達
      if (nextTime == nextFinish && nextFinish <= nextArrival) {
        // advance to nextFinish
        cur_time = nextFinish;
        // 處理完成事件（若同時兩個完成，兩個都處理）
        if (finish1 != -1 && finish1 == cur_time && cook1.size() > 0) {
          Node *job = cook1.head;
          int Departure = finish1;
          if (job->Timeout < Departure) {
            int Delay = start1 - job->Arrival;
            if (Delay < 0) Delay = 0;
            total_delay += Delay;
            Timeout.enquene(job->OID, job->Arrival, job->Duration, job->Timeout, 0, Delay, Departure, job->CID);
          }
          cook1.dequene();
          finish1 = -1;
          start1 = -1;
        }
        if (finish2 != -1 && finish2 == cur_time && cook2.size() > 0) {
          Node *job = cook2.head;
          int Departure = finish2;
          if (job->Timeout < Departure) {
            int Delay = start2 - job->Arrival;
            if (Delay < 0) Delay = 0;
            total_delay += Delay;
            Timeout.enquene(job->OID, job->Arrival, job->Duration, job->Timeout, 0, Delay, Departure, job->CID);
          }
          cook2.dequene();
          finish2 = -1;
          start2 = -1;
        }
        // 完成後，**同時間點**也可能有新的到達，要在同時間處理到達（題目規範）
        handle_arrivals_at(cur_time);
      } else {
        // 下一事件為到達（且早於任何完成）
        cur_time = nextArrival;
        handle_arrivals_at(cur_time);
      }
      // loop 回去，會再次安排沒有開始的工作
    } // end while

    return total_delay;
  }
  // --- end twocook ---



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
  std::string file_number;
  std::cout << "Input a file number (e.g., 401, 402, 403, ...): ";
  std::cin >> file_number;
  getchar();
  std::string filename = "input" + file_number + ".txt"; // 轉字串
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
  std::string file_number;
  std::cout << "Input a file number (e.g., 401, 402, 403, ...): ";
  std::cin >> file_number;
  getchar();
  std::string filename = "sorted" + file_number + ".txt"; // 轉字串
  std::ifstream infile(filename); // 讀檔
  Queue q1;
  if (infile) {
    q1.Print_original(filename);
    std::ifstream infile2(filename);
    q1.load(infile2);
    Queue cook;
    Queue cancel;
    Queue delay;
    int total_delay = q1.onecook(cook, cancel, delay);
    int size = q1.size();
    cancel.write_file2(file_number, delay, total_delay, size);
    
  } else {
    std::cout << "input" << file_number << ".txt does not exist!";
  }
  return;
}

void task3() {
  std::string file_number;
  std::cout << "Input a file number (e.g., 401, 402, 403, ...): ";
  std::cin >> file_number;
  getchar();
  std::string filename = "sorted" + file_number + ".txt"; // 轉字串
  std::ifstream infile(filename); // 讀檔
  Queue q1;
  if (infile) {
    std::ifstream infile2(filename);
    q1.load(infile2);
    Queue cook;
    Queue cancel;
    Queue delay;
    Queue cook2;
    int total_delay = q1.twocook(cook, cook2, cancel, delay);
    int size = q1.size();
    cancel.write_file2(file_number, delay, total_delay, size);
    
  } else {
    std::cout << "input" << file_number << ".txt does not exist!";
  }
  return;
}

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
  char ch;
  int command = 0;
  bool has_2 = false;
  Start(); // 印選單
  while (true) {
    command = 0;
    bool has_digit = false; // 已經有數字
    bool invalid = false; // 非法輸入
    bool aaaaa = false;
    while ((ch = getchar()) != '\n') {
      if (ch == ' ' || ch == '\t') { // 跳過空白跟tab
        continue;
      } else if (ch >= '0' && ch <= '4') {
        if (has_digit) {
          invalid = true; // 多個數字
        } else {
          command = ch - '0';
          has_digit = true;
        }
      } else if (ch >= '5' && ch <= '9') {
        has_digit = true;
      } else { // 非法字元
        aaaaa = true;
      }
    }

    if (aaaaa) {
      return 0;
    }

    if (invalid) {
      std::cout << "Command does not exist!" << std::endl << std::endl;
      Start();
      continue;
    }

    if ( (command == 3 || command == 4) && !has_2 ) {
      std::cout << "\n### Execute command 2 first! ###" << std::endl << std::endl;
      Start();
      continue;
    }

    if (command == 0) {
      return 0;
    }
    if (command == 1) {
      task1();
    } else if (command == 2) {
      has_2 = true;
      task2();
    } else if (command == 3) {
      task3();
    } else if (command == 4) {
      //task4();
    }
    Start();
  }
  return 0;
}
