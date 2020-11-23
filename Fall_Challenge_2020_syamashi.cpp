#include <bits/stdc++.h>
using namespace std;
using ll=int;
#define rep(i,n) for (ll i=0; i<n; ++i)
#define all(c) begin(c),end(c)
#define PI acos(-1)
#define oo LLONG_MAX
template<typename T1, typename T2>
bool chmax(T1 &a,T2 b){if(a<b){a=b;return true;}else return false;}
template<typename T1, typename T2>
bool chmin(T1 &a,T2 b){if(a>b){a=b;return true;}else return false;}
/*
Update
++ CASTの順番をライブラリの古いほうから探索。
+ 連続RESTパターン除外
+ castable加点
+ b_inv = 0.1 , inv個数に重みづけ。1個め *= 1.0, 2個め *= 0.9, 3個目 *= 0.8...

? get_tome = 10 -> 9
? b_score = 2.0 -> 2.2
? b_rest = 0.1

Update 11/21 1:00
旧処理数16000 ~ 26500
vector<bool> = 4byte
bool = 1byte
bool A[]を関数に渡すと、同じポインタなので共通変更になってしまう。
構造体渡しだとコピー。

処理数->120000手まで。

--- 1個前67位(80%) --
2日前の復元。

tome = 5で15位まで。
tome = 4で出してる。
今度tome = 6で試したい。
tome = 8も強いかも

tome4 = 35位。92%
//            if (st.score + st.inv[1] + st.inv[2] + st.inv[3] < op.score) score = 0;

やはりライブラリの選別

---7冊で3位まで---

変換効率悪いのに黄色とりすぎるのやばい。
b_invを100->50に
本多めにとるか。8冊で？

9 = 5位
2step pre: bit = 20th

31を2個上まで＝13位

*/
std::chrono::system_clock::time_point  clockstart, clockend;
#define MAX_TONE 20
ll loop = 5;
ll turn = 0;
ll get_tome = 8;
ll g_node = 0;
//ll g_footcut = 20; // 足きり
ll highest;
int b_score = 1250;
int b_inv = 50; // 0-100 inv数が多いほど減点補正。
int b_rest = 50; // 
int potion_count_bonus = 500;
ll blco[4] = {}; // 0=brew, 1=learn, 2=cast, 3=opponent_cast
bool dfsend = false;

int inv0p = 1000;
int inv1p = 2000;
int inv2p = 2900;
int inv3p = 3800;

typedef struct t_data{
    int aid;
    int delta[4];
    int price;
    int tome;
    int tax;
    bool castable;
    int repmax;
    int dif;
}   s_data;
vector<s_data> brew(10);
vector<s_data> learn(10);
vector<s_data> cast(50);
vector<s_data> ocast(10);

typedef struct t_status{
    ll inv[4];
    ll score;
    ll potioncnt;
} s_status;
s_status my, op;

static int	intlen(long n){
	int len;

	len = 0;
	if (n < 0){
		n = -n;
		len++;
	}
	while (n){
		n /= 10;
		len++;
	}
	return (len);
}

string ft_itoa(int n){
	char	str[20];
	int		len;
	long	m;
	int		i;

	len = (n == 0) ? 1 : intlen(n);
	m = n;
	if (n < 0) {
		str[0] = '-';
		m = -m;
	}
	if (n == 0)
		str[0] = '0';
	i = len;
	while (m) {
		str[--i] = m % 10 + '0';
		m /= 10;
	}
	str[len] = '\0';
	return (str);
}

void input_repmax(s_data &Z, bool &repeatable){
    ll total = 0, total_make = 0;
    rep(i, 4){
        if (Z.delta[i] < 0) total += Z.delta[i];
        else if (Z.delta[i] > 0) total_make += Z.delta[i];
    }
    Z.dif = total + total_make;
    if (repeatable) {
        total = -total;
        chmax(total, total_make);
        Z.repmax = 10 / total;
    }
    else{
        Z.repmax = 1;
    }    
}

void input(){
    int actionCount; // the number of spells and recipes in play
    cin >> actionCount; cin.ignore();
    rep(i, 4) blco[i] = 0;
    for (int i = 0; i < actionCount; i++) {
        s_data Z;
        string actionType; // in the first league: BREW; later: CAST, OPPONENT_CAST, LEARN, BREW
        bool repeatable;
        cin >> Z.aid
            >> actionType
            >> Z.delta[0]
            >> Z.delta[1]
            >> Z.delta[2]
            >> Z.delta[3]
            >> Z.price
            >> Z.tome
            >> Z.tax
            >> Z.castable
            >> repeatable; cin.ignore();
        if (actionType == "BREW"){
            int n = blco[0];
            brew[n] = Z;
            blco[0]++;
        }
        if (actionType == "LEARN"){
            int n = blco[1];
            input_repmax(Z, repeatable);
            learn[n] = Z;
            blco[1]++;
        }
        if (actionType == "CAST"){
            int n = blco[2];
            input_repmax(Z, repeatable);
            cast[n] = Z;
            blco[2]++;
        }
        if (actionType == "OPPONENT_CAST"){
            int n = blco[3];
            ocast[n] = Z;
            blco[3]++;
        }
    }
    for (int i = 0; i < 2; i++) {
        s_status z;
        cin >> z.inv[0] >> z.inv[1] >> z.inv[2] >> z.inv[3] >> z.score; cin.ignore();
        if (i == 0){
            z.potioncnt = my.potioncnt;
            if (my.score < z.score)
                z.potioncnt = my.potioncnt + 1;
            my = z;
        }
        if (i == 1){
            z.potioncnt = op.potioncnt;
            if (op.score < z.score)
                z.potioncnt = op.potioncnt + 1;
            op = z;
        }
    }
}

typedef struct t_ans{
    char type; // cast, brew, rest
    int aid;
    int times;
} s_ans;

typedef struct t_st{
    char type; // cast, brew, rest
    int times;
    int tid;
} s_st;

typedef struct t_br{
    bool brewable[5];
} s_br;

typedef struct t_ca{
    bool castable[MAX_TONE];
} s_ca;

typedef struct t_le{
    bool learned[6];
    bool castable[6];
} s_le;

s_ans ans;

void action_brew(s_st &pre, s_status &st, s_br &br){
    ll id = pre.tid;
    rep(i, 4){
        st.inv[i] += brew[id].delta[i];
    }
    st.score += brew[id].price;
    br.brewable[id] = false;
    st.potioncnt++;
    return ;
}

void action_cast(s_st &pre, s_status &st, s_ca &ca){
    ll id = pre.tid;
    rep(i, 4){
        st.inv[i] += cast[id].delta[i] * pre.times;
    }
    ca.castable[id] = false;
    return ;
}

void action_learn(s_st &pre, s_status &st, s_le &le){
    ll id = pre.tid;
    if (pre.times == 0){
        le.learned[id] = true;
        st.inv[0] -= id;
        st.inv[0] += learn[id].tax;
    }
    else {
        le.castable[id] = false;
        rep(i, 4){
            st.inv[i] += learn[id].delta[i] * pre.times;
        }
    }
}

bool action_rest(s_st &pre, s_ca &ca, s_le &le){
    bool pre_rest = true;
    rep(i, blco[2]){
        if (ca.castable[i] == false) pre_rest = false;
        ca.castable[i] = true;
    }
    rep(i, blco[1]){
        if (le.castable[i] == false) pre_rest = false;
        le.castable[i] = true;
    }
    return pre_rest;
}

int eval(const s_status &st, s_le &le, s_ca &ca, ll &depth){
    int score = 0;
    // inv
    score += (inv0p + (inv0p - st.inv[0] * b_inv)) * st.inv[0] / 2;
    score += (inv1p + (inv1p - st.inv[1] * b_inv * 2)) * st.inv[1] / 2;
    score += (inv2p + (inv2p - st.inv[2] * b_inv * 3)) * st.inv[2] / 2;
    score += (inv3p + (inv3p - st.inv[3] * b_inv * 4)) * st.inv[3] / 2;
    // potion 
    score += st.score * (b_score - depth);
    // rest
    rep(i, blco[2]){
        if (!ca.castable[i]) continue;
        ll restscore = 1;
        if (i > 3){
            ll difpoint = cast[i].delta[0] + cast[i].delta[1] * 2 + cast[i].delta[2] * 3 + cast[i].delta[3] * 4;
            chmax(restscore, difpoint * cast[i].repmax / 2);
        }
        score += restscore * b_rest;
    }
    // learn
    rep(i, blco[1]){
        if (!le.learned[i]) continue;
        if (!le.castable[i]) continue;
        ll difpoint = cast[i].delta[0] + cast[i].delta[1] * 2 + cast[i].delta[2] * 3 + cast[i].delta[3] * 4;
        score += difpoint * cast[i].repmax / 2 * b_rest;
    }
    score += potion_count_bonus * st.potioncnt;
    return score;
}



void dfs(ll depth, s_status st, s_ca ca, s_le le, s_br br, s_st pre, s_ans cand){
//    g_node++;
    clockend = std::chrono::system_clock::now();
    double time = std::chrono::duration_cast<std::chrono::microseconds>(clockend - clockstart).count();
    if (time > 45000) {
        cerr << "timeover" << endl;
        dfsend = true;
    }
    if (dfsend) return;
    if (depth == 1) {
        cand.type = pre.type;
        ll tid = pre.tid;
        if (pre.type == 'B')
            cand.aid = brew[tid].aid;
        if (pre.type == 'L'){
            cand.aid = learn[tid].aid;
            cand.times = pre.times;
        }
        if (pre.type == 'C'){
            cand.aid = cast[tid].aid;
            cand.times = pre.times;
        }
//cerr << "cand : " << cand.type << " " << cand.aid << " " << cand.times << endl;        
    }
    if (depth > 0){
        if (pre.type == 'B') action_brew(pre, st, br);
        if (pre.type == 'C') action_cast(pre, st, ca); // statusとca情報
        if (pre.type == 'L') action_learn(pre, st, le);
        if (pre.type == 'R'){
            bool pre_rest;
            pre_rest = action_rest(pre, ca, le);
            if (pre_rest) return;
        }
    }
//    ll score = eval(st, le, ca, depth);
//    if (depth - g_footcut >= 0 && score < highest[max(depth - g_footcut, 0)]){
//        return ;
//    }
//    chmax(highest[depth], score);
    if (depth == loop || st.potioncnt == 6){
        // if endup, dup score
        ll score = eval(st, le, ca, depth);
        if (st.potioncnt == 6){
            if (st.score + st.inv[1] + st.inv[2] + st.inv[3] < op.score + op.inv[3]) score = 0;
            else score *= (loop - depth + 2);
        }
        if (chmax(highest, score)){
            ans = cand;
//cerr << highest << endl;
        }
        return ;
    }
    ll pattern = blco[0] + blco[1] + blco[2] + 1;
    ll sum = st.inv[0] + st.inv[1] + st.inv[2] + st.inv[3];
    for (ll i = 0; i < pattern; i++){
        if (i < blco[0]){
            pre.type = 'B';
            if (!br.brewable[i]) continue;
            bool get = false;
            rep(j, 4){
                if (st.inv[j] + brew[i].delta[j] < 0) break;
                if (j == 3) get = true;
            }
            if (!get) continue;
            pre.tid = i;
            dfs(depth + 1, st, ca, le, br, pre, cand);
            if (dfsend) return;            
        }
        else if (i < blco[0] + blco[1]){
            pre.type = 'L';
            ll id = i - blco[0];
            pre.tid = id;
            // 1. if not learned && depth == 0,  = learn
            // 2. if learned, castable = cast
            // 3. if learned, not castable = continue
            if (blco[2] < MAX_TONE - 5 && depth == 0 && !le.learned[id] && st.inv[0] >= id){
                pre.times = 0;
                dfs(depth + 1, st, ca, le, br, pre, cand);
                if (dfsend) return;            
            }
            if (le.learned[id] && !le.castable[id]) continue;
            if (le.learned[id] && le.castable[id]){
                pre.tid = id;
                for (ll times = 1; times <= learn[id].repmax; times++){
                    ll nsum = sum + times * learn[id].dif;
                    if (nsum > 10 || nsum <= 0) break;
                    bool ok = false;
                    rep(j, 4){
                        ll need = times * cast[id].delta[j];
                        if (st.inv[j] + need < 0) break;
                        if (j == 3) ok = true;
                    }
                    if (!ok) break;
                    pre.times = times;
                    dfs(depth + 1, st, ca, le, br, pre, cand);
                    if (dfsend) return;
                }
            }
        }
        else if (i < blco[0] + blco[1] + blco[2]){
            pre.type = 'C';
            ll id = blco[0] + blco[1] + blco[2] - 1 - i;
            //1. castable = false
            //2. over 10 or less 0
            //3. less inv
            if (!ca.castable[id]) continue;
            pre.tid = id;
            for (ll times = 1; times <= cast[id].repmax; times++){
                ll nsum = sum + times * cast[id].dif;
                if (nsum > 10 || nsum <= 0) break;
                bool ok = false;
                rep(j, 4){
                    ll need = times * cast[id].delta[j];
                    if (st.inv[j] + need < 0) break;
                    if (j == 3) ok = true;
                }
                if (!ok) break;
                pre.times = times;
                dfs(depth + 1, st, ca, le, br, pre, cand);
                if (dfsend) return;
            }
        }
        else {
            pre.type = 'R';
            dfs(depth + 1, st, ca, le, br, pre, cand);
            if (dfsend) return;
        }
    }
}

//double learnpoint[2][4] = {{-1, -1, -1, 0}, {2, 1, 1, 1}};

// 2分探索
// 6個のライブラリのうち5個をとると仮定。
// 最も評価点が高い組み合わせのうち、idが若いものを取得
// 3位->20位。。。

/*int learn_search(){
    ll ret = 0;
    double score = 0;
    rep(i, 1 << 6){
        if (__builtin_popcount(i) != 3) continue;
        ll best_id = 6;
        double sumpoint[2][4];
        double tmpscore = 0.0;
        rep(n, 2) rep(m, 4) sumpoint[n][m] = learnpoint[n][m];
        for (ll j = 5; j >= 0; j--){
            if ((1 & (i >> j)) == 0) continue;
            double bonus = 1.0;
            best_id = j;
            if (learn[j].repmax == 5) bonus = 3.0;
            if (learn[j].repmax == 3) bonus = 2.0;
            if (learn[j].repmax == 1 && learn[j].dif == 4) bonus = 2.0;
            rep(k, 4){
                if (learn[j].delta[k] < 0)
                    sumpoint[0][k] += learn[j].delta[k] * bonus;
                else
                    sumpoint[1][k] += learn[j].delta[k] * bonus;
            }
            rep(k, 4){
                tmpscore -= sumpoint[0][k] * sumpoint[1][k];
            }
        }
        if (chmax(score, tmpscore) && my.inv[0] >= best_id){
             ret = best_id;
// cerr << score << endl;
        }
    }
    return ret;
}
*/

/*
void    init_learnpoint(int &id){
    double bonus = 1.0;
    if (learn[id].repmax == 5) bonus = 3.0;
    if (learn[id].repmax == 3) bonus = 2.0;
    if (learn[id].repmax == 1 && learn[id].dif == 4) bonus = 2.0;
    rep(k, 4){
        if (learn[id].delta[k] < 0)
            learnpoint[0][k] += learn[id].delta[k] * bonus;
        else
            learnpoint[1][k] += learn[id].delta[k] * bonus;
    }
}
*/

/*int learn_topid(){
    int id;
    vector<pair<ll, ll>> P(min(my.inv[0] + 1, 6));
    for (ll i = min(my.inv[0], 5); i >= 0; i--){
        ll j = my.inv[0] - i;
        ll aid = learn[j].aid;
        if (aid == 13) aid += 10;
        P[j] = {max(aid - j * 20 + learn[j].tax * 20, 0), j};
    }
    sort(all(P));
    reverse(all(P));
    cerr << P[0].first << endl;
    if (P[0].first <= 4)
        return -1;
    return (P[0].second);
}*/

// actionidが34以上のライブラリがid=1にあったら先取
int learn_topid(){
    if (learn[0].aid > learn[1].aid)
        return 0;
    if (my.inv[0] > 0 && learn[1].aid >= 34)
        return (1);
    return 0;
}

int main()
{
    cin.tie(0);
    ios::sync_with_stdio(0);
    my.potioncnt = 0;
    // game loop
    bool tome_continue = true;
    while (1) {
        g_node = 0;
        dfsend = false;
//        highest.assign(loop + 2, 0); 足きりデータ
        highest = -1;
        turn++;
        input();
//        if (op.potioncnt >= 4 || my.potioncnt >= 4) b_score = 200;
        if (blco[2] > 10) loop = 5;
        else loop = 6;
        clockstart = std::chrono::system_clock::now();
        if (get_tome > 0){
            ll id = learn_topid();
            if (id >= 0){
                cout << "LEARN " << learn[id].aid << endl;
                get_tome--;
                continue;
            }
        }
        s_status st = my;
        s_br br;
        s_ca ca;
        s_st pre;
        s_le le;
        s_ans cand;
        rep(j, blco[0])
            br.brewable[j] = true;
        rep(j, blco[1]) {
            le.learned[j] = false;
            le.castable[j] = true;
        }
        rep(j, blco[2]) {
            ca.castable[j] = cast[j].castable;
        }
//cerr << st.inv[0] << " " << st.inv[1] << " " << st.inv[2] << " " << st.inv[3] << " " << endl;
//cerr << st.potioncnt << " " << st.score << endl;
        dfs(0, st, ca, le, br, pre, cand);
//cerr << "highest : "; rep(k, loop+1) cerr << highest[k] << " "; cerr << endl;
        string output;
        if (ans.type == 'C')
            output = "CAST";
        if (ans.type == 'L')
            output += "LEARN";
        if (ans.type == 'B')
            output = "BREW";
        if (ans.type == 'R')
            output = "REST";
        if (ans.type != 'R'){
            output += " ";
            output += ft_itoa(ans.aid);
        }
        if (ans.type == 'C'){
            output += " ";
            output += ft_itoa(ans.times);
        }
//        output += " ";
//        output += ft_itoa(g_node);
        ll myscore = my.score + my.inv[0] + my.inv[1] * 2 +  my.inv[2] * 3 +  my.inv[3] * 4;
        ll opscore = op.score + op.inv[0] + op.inv[1] * 2 +  op.inv[2] * 3 +  op.inv[3] * 4;
        output += " ";
        output += ft_itoa(myscore - opscore);
        cout << output << endl;
    }
}
