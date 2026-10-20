Warikan easy ja ? C
/*
 * 割り勘計算プログラム（初心者向け）
 *
 * このプログラムがすること:
 *   1. 人数を聞いて、各人について
 *        名前・立て替えた金額（円）・「重み」
 *        を入力してもらう。
 *        （重み 1.0 = ふつう、0.5 = 途中参加・少なめ、0 = 不参加）
 *   2. 各人が「本来払うべき金額」（負担額）を計算する。
 *   3. 各人の「差額」= 支払額 - 負担額 を計算する。
 *        差額 > 0  -> お金を「もらう」人
 *        差額 < 0  -> お金を「払う」人
 *   4. 誰が誰にいくら送ればよいかを表示する。
 *
 * ビルドと実行（Visual Studio / MSVC の場合はそのままビルドできます）:
 *   ※ scanf_s は MSVC 用です。gcc の場合は scanf に書き換えてください。
 */
 
#include <stdio.h>
 
#define MAX_PEOPLE 20   /* 受け付ける最大人数 */
 
 /* ---------- データ ----------
  * 配列には「1人につき1つ」の値が入ります。
  * 0番目の人は各配列の添字0、1番目の人は添字1 ... に入ります。
  * つまり names[2], paid[2], weight[2] ... は同じ人の情報です。
  */
char   names[MAX_PEOPLE][64];  /* 名前（63バイトまで） */
long   paid[MAX_PEOPLE];       /* 各人が立て替えた金額 */
double weight[MAX_PEOPLE];     /* 各人の重み */
long   share[MAX_PEOPLE];      /* 各人が負担すべき金額 */
long   balance[MAX_PEOPLE];    /* 差額 = 支払額 - 負担額 */
 
 
/* ---------- ステップ2: 各人の負担額を計算する ----------
 * 考え方: 合計金額 × (自分の重み ÷ 全員の重みの合計)
 *
 * 問題点: 円は小数に分けられません。例: 1000円を3人で割ると
 * 333.33...円ずつ。そこで全員を切り捨てて333円（合計999円）にし、
 * 余った1円を誰かに足して、合計がぴったり合うようにします。
 */
void calculate_shares(int n, long total)
{
    double weight_sum = 0;
    long   assigned = 0;     /* ここまでに割り当てた負担額の合計 */
    int    i;
 
    /* 重みをすべて足す */
    for (i = 0; i < n; i++) {
        weight_sum += weight[i];
    }
 
    /* 全員の負担額を切り捨てで計算する
       （double を long に変換すると小数点以下が切り捨てられる） */
    for (i = 0; i < n; i++) {
        share[i] = (long)(total * weight[i] / weight_sum);
        assigned += share[i];
    }
 
    /* 余った円を、参加した人に1円ずつ配る */
    long leftover = total - assigned;
    for (i = 0; i < n && leftover > 0; i++) {
        if (weight[i] > 0) {
            share[i] += 1;
            leftover -= 1;
        }
    }
 
    /* 差額 = 実際に払った額 - 本来払うべき額 */
    for (i = 0; i < n; i++) {
        balance[i] = paid[i] - share[i];
    }
}
 
 
/* ---------- ステップ3: 誰が誰に払うかを決める ----------
 * かんたんな方法:
 *   くり返し:
 *     - いちばん多くもらうべき人を探す
 *     - いちばん多く払うべき人を探す
 *     - 払う人は、2人の金額のうち小さいほうを相手に払う
 *     - これでどちらか一方（または両方）の差額が0になる
 *   全員の差額が0になったら終了。
 */
void print_payment_plan(int n)
{
    int count = 0;   /* 表示した送金の回数 */
 
    while (1) {
        int biggest_creditor = 0;   /* 差額が最大の人の添字（もらう人） */
        int biggest_debtor = 0;     /* 差額が最小の人の添字（払う人） */
        int i;
 
        /* 差額の最大と最小を探す */
        for (i = 1; i < n; i++) {
            if (balance[i] > balance[biggest_creditor]) biggest_creditor = i;
            if (balance[i] < balance[biggest_debtor])   biggest_debtor = i;
        }
 
        /* 最大の差額が0なら、全員の精算が完了 */
        if (balance[biggest_creditor] == 0) {
            break;
        }
 
        /* 金額は「もらう側がもらうべき額」と
           「払う側が払うべき額」のうち小さいほう */
        long owed = balance[biggest_creditor];
        long debt = -balance[biggest_debtor];   /* 正の数にする */
        long amount = (owed < debt) ? owed : debt;
 
        printf("  %s -> %s : %ld 円\n",
            names[biggest_debtor], names[biggest_creditor], amount);
 
        /* 2人の差額を更新する */
        balance[biggest_creditor] -= amount;
        balance[biggest_debtor] += amount;
        count++;
    }
 
    if (count == 0) {
        printf("  送金は必要ありません。\n");
    }
    else {
        printf("送金回数: %d 回\n", count);
    }
}
 
 
/* ---------- メイン: 入力・計算・出力 ---------- */
int main(void)
{
    int  n;           /* 人数 */
    long total = 0;   /* 全員が払った金額の合計 */
    int  i;
 
    printf("=== 割り勘計算プログラム ===\n");
 
    /* ステップ1: 入力 */
    printf("参加人数 (1-%d): ", MAX_PEOPLE);
    if (scanf_s("%d", &n) != 1 || n < 1 || n > MAX_PEOPLE) {
        printf("1 から %d までの数字を入力してください。\n", MAX_PEOPLE);
        return 1;
    }
 
    printf("各人を「名前 支払額 重み」の形式で入力してください\n");
    printf("  例: Tanaka 12000 1.0\n");
    printf("  重み: 1.0 ふつう / 0.5 途中参加 / 0 不参加\n");
 
    for (i = 0; i < n; i++) {
        printf("%d 人目: ", i + 1);
        if (scanf_s("%63s %ld %lf", names[i], 64, &paid[i], &weight[i]) != 3) {
            printf("入力が正しくありません。\n");
            return 1;
        }
        if (paid[i] < 0 || weight[i] < 0) {
            printf("金額と重みは 0 以上にしてください。\n");
            return 1;
        }
        total += paid[i];
    }
 
    /* 重みが1人でも0より大きいか確認する
       （全員0だと、あとで0で割ってしまうため） */
    double weight_sum = 0;
    for (i = 0; i < n; i++) {
        weight_sum += weight[i];
    }
    if (weight_sum <= 0) {
        printf("重みが全員 0 になっています。\n");
        return 1;
    }
 
    /* ステップ2: 計算 */
    calculate_shares(n, total);
 
    /* 一覧表を表示する（項目はタブで区切っています） */
    printf("\n合計金額: %ld 円\n\n", total);
    printf("名前\t支払額\t重み\t負担額\t差額\n");
    for (i = 0; i < n; i++) {
        printf("%s\t%ld\t%.2f\t%ld\t%+ld\n",
            names[i], paid[i], weight[i], share[i], balance[i]);
    }
 
    /* ステップ3: 誰が誰に払うか */
    printf("\n--- 精算方法 ---\n");
    print_payment_plan(n);
 
    return 0;
}