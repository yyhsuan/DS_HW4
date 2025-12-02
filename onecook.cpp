void onecook(Queue &cook, Queue &cancel, Queue &Timeout) {

    Node *temp = head; // 外部輸入訂單列表

    // =====================================================
    // ① 先處理外部訂單，決定是：直接做 / 排隊 / cancel
    // =====================================================
    while (temp != NULL) {

        // ------ Case 1: cook queue 已滿 --> cancel ------
        if (cook.size() >= 4) {
            cancel.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout,
                           temp->Arrival, 0, 0, 1);
            temp = temp->next;
            continue;
        }

        // ------ Case 2: cook queue 空 → 可直接開始做 ------
        if (cook.size() == 0) {

            int start = max(cook.now_time, temp->Arrival);
            int finish = start + temp->Duration;
            int delay = start - temp->Arrival;

            cook.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout,
                         start, delay, finish, 1);

            cook.now_time = finish;

            // 超時
            if (finish > temp->Timeout) {
                Timeout.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout,
                                start, delay, finish, 1);
            }

            temp = temp->next;
            continue;
        }

        // ------ Case 3: cook queue 有人在做 → 先排隊 ------
        cook.enquene(temp->OID, temp->Arrival, temp->Duration, temp->Timeout,
                     0, 0, 0, 0);

        temp = temp->next;
    }


    // =====================================================
    // ② cook queue 裡還有等待者 → 自動開始做
    // =====================================================
    while (cook.size() > 0) {

        Node *job = cook.front();   // 取得最前面的等待訂單（但先不 pop）

        // 這筆是已經做完的（已經在前段處理過）
        if (job->Done == 1) {
            cook.dequene();
            continue;
        }

        // ---------- 開始執行等待的訂單 ----------
        int start = max(cook.now_time, job->Arrival);
        int finish = start + job->Duration;
        int delay = start - job->Arrival;

        // 更新這筆訂單資料
        job->Start = start;
        job->Delay = delay;
        job->Finish = finish;
        job->Done = 1;

        cook.now_time = finish;

        // ---------- 超時 ----------
        if (finish > job->Timeout) {
            Timeout.enquene(job->OID, job->Arrival, job->Duration, job->Timeout,
                            start, delay, finish, 1);
        }

        // 完成後 pop
        cook.dequene();
    }
}
