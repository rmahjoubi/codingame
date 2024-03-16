#include <iostream>
#include <bits/stdc++.h>
using namespace std;

enum Direction {UP, LEFT, DOWN, RIGHT};

std::map<Direction, std::string> S_directions = {
    {UP, "UP"},
    {DOWN, "DOWN"},
    {LEFT, "LEFT"},
    {RIGHT, "RIGHT"}
};

enum Action {WAIT, MOVE, ATTACK, UPGRADE, DEGRADE, SUICIDE};

std::map<Action, std::string> S_ACTIONS = {
    {WAIT, "WAIT"},
    {MOVE, "MOVE"},
    {ATTACK, "ATTACK"},
    {UPGRADE, "UPGRADE"},
    {DEGRADE, "DEGRADE"},
    {SUICIDE, "SUICIDE"}
};


class WORLD;
class PLAYER;
class SOLDIER; 
class CYCLE;
class ACTION;

class SOLDIER{
    public:
        int id;
        int level{};
        int owner_id{};
        Direction direction{};
        pair<int, int> pos{};
        //WORLD &world;

        array<pair<int, int>, 8> range{};
        //enum Direction {UP, DOWN, LEFT, RIGHT} direction;
        SOLDIER();
        SOLDIER(int id, Direction d, pair<int, int> pos, int owner_id, int level);
        void update_range();
        void update_pos(int x, int y);
        //void move(Direction d);
        void move(WORLD& world, Direction d);
        void clear_range(WORLD &world);
        void affect_range(WORLD &world);
 
};

class PLAYER{
    public:
        int bucks{};
        int blocks_count{};
        int id{};
        bool is_max{true};
        //vector<SOLDIER> v_soldier;
        map<int, SOLDIER> v_soldier{};
        vector<vector<int>> vv_range_map{};

        PLAYER();
        PLAYER(int id, int bucks, bool is_max, int map_size);
        SOLDIER& get_soldier(int id);
        //void restart(int bucks, bool is_max);
        //void restart(int bucks, bool is_max, int map_size);
};


class ACTION{
    public:
        Action action{};
        int player_id{};
        int soldier_id{};
        int opp_soldier_id{};
        Direction direction{};
        string message{};
        int x_owner{};
        pair<int, int> x_pos;
        Direction x_direction;


        SOLDIER& find_soldier(int player_id, int soldier_id, WORLD& world);
        ACTION(Action a, int s_id, int o_s_id, Direction d, string m);
        ACTION(Action a, int s_id, Direction d, string m = "");
        ACTION();
        void apply(WORLD &world,bool out=false);
        void undo(WORLD &world,bool out=false);
        template<typename... Args>
        void print(string action, const Args&... args);
        ACTION(Action a, int s_id);
};




class CYCLE{
    public:
        int count{};
        int last_payement{};
    
        CYCLE(int count=0, int cycle_owner=0);
        int cycle_owner_id{};
        int opponent_id{1};
        void end_cycle(WORLD &world);
        void pay(WORLD &world);
        void revert(WORLD &world);
};



class WORLD{
    public:
        vector<vector<int>> vv_map;
        array<PLAYER, 2> v_players;
        
        int map_size;
        CYCLE cycle;

        //int get_opponent_id(int id);
        WORLD(int size, int cycle_count, int active_player_id, pair<int, int> p1, pair<int, int> p2);
        void update_world(pair<int, int> p1, pair<int, int> p2);
        void populate();
        bool is_legal(Direction d, SOLDIER& S);
        //bool enemy_in_range(int player_id, int opponent_id);
        bool soldier_in_range(int player_id, int opponent_id, int soldier_id);
        bool enemy_in_range(WORLD& world, int player_id, int opponent_id);
        ACTION attack_enemy(int player_id, int opponent_id);
        int occupy(int x, int y, int player_id);
        void unoccupy(int x, int y, int player_id, int x_owner_id);
        //PLAYER& get_player(int id);
        void print_world();
};




//---------------------------------------------------------------------
//red-action


ACTION::ACTION(Action a, int s_id, int o_s_id, Direction d, string m) : action(a), 
                soldier_id(s_id), opp_soldier_id(o_s_id), direction(d), message(m){}

ACTION::ACTION(Action a, int s_id, Direction d, string m) : action(a), 
                soldier_id(s_id), direction(d), message(m){}

ACTION::ACTION(Action a, int opp_s_id) : action(a), opp_soldier_id(opp_s_id){}


ACTION::ACTION() : action(WAIT){}

void ACTION::apply(WORLD &world, bool out){
    //SOLDIER& s = this->find_soldier(world.cycle.cycle_owner, soldier_id, world);
    SOLDIER& s = world.v_players[world.cycle.cycle_owner_id].v_soldier[soldier_id];
    switch (action){
        case WAIT:
            if (out){
                this->print(S_ACTIONS[action]);
            }
            break;
        case MOVE:
            x_pos = s.pos;
            x_direction = s.direction;
            s.move(world, direction);
            x_owner = world.occupy(s.pos.first, s.pos.second, world.cycle.cycle_owner_id);
            if (out){
                this->print(S_ACTIONS[action], soldier_id, S_directions[direction]);
            }
            break;
        case ATTACK:
            if (out){
                this->print(S_ACTIONS[action], soldier_id, opp_soldier_id);
            }
            break;
        case UPGRADE:
            break;
        case DEGRADE:
            if (out){
                this->print(S_ACTIONS[action], opp_soldier_id);
            }
            break;
        case SUICIDE:
            break;
    }
    world.cycle.end_cycle(world);
    //cerr << "make action: " << S_ACTIONS[action] << " direction " << S_directions[direction] << " soldier id" << soldier_id << " opponent soldier id " << opp_soldier_id << endl;
    //world.print_world();
}
void ACTION::undo(WORLD &world,bool out){
    world.cycle.revert(world);
    //SOLDIER& s = this->find_soldier(world.cycle.cycle_owner, soldier_id, world);
    SOLDIER& s = world.v_players[world.cycle.cycle_owner_id].v_soldier[soldier_id];
    switch (action){
        case WAIT:
            break;
        case MOVE:
            world.unoccupy(s.pos.first, s.pos.second, world.cycle.cycle_owner_id, x_owner);
            s.clear_range(world);
            //s.update_direction(x_direction);
            s.direction = x_direction;
            s.update_pos(x_pos.first, x_pos.second);
            s.update_range();
            s.affect_range(world);
            break;
        case ATTACK:
            break;
        case UPGRADE:
            break;
        case DEGRADE:
            break;
        case SUICIDE:
            break;
    }
    //cerr << "undo action: " << S_ACTIONS[action] << " direction " << S_directions[direction] << " soldier id" << soldier_id << " opponent soldier id " << opp_soldier_id << endl;
    //world.print_world();
}

template<typename... Args>
void ACTION::print(string action, const Args&... args) {
   cout << action;
   if constexpr(sizeof...(args) > 0){
       // Use a fold expression to print each argument followed by a space
       ((std::cout << " " << args), ...);
    }
    std::cout << std::endl;
}



//---------------------------------------------------------------------
//red-soldier

SOLDIER::SOLDIER(int id, Direction d, pair<int, int> pos, int owner_id, int level) : id(id), direction(d), pos(pos), owner_id(owner_id), level(level){
    this->update_range();
}  

SOLDIER::SOLDIER(){}

SOLDIER& PLAYER::get_soldier(int id){
    //for(auto& s : this->v_soldier){
    //    if(s.id == id){
    //        return s;
    //    }
    //}
    return v_soldier[id];
    cerr << "id not found " << id << endl;
    throw std::runtime_error("Soldier with given ID not found.");
}

void SOLDIER::update_range(){
    int x = pos.first; int y = pos.second;
     switch (direction){
        case UP:
            range[0]={x-1,y};range[1]={x-2,y};range[2]={x,y+1};range[3]={x,y+2};
            range[4]={x,y-1};range[5]={x,y-2};range[6]={x-1,y-1};range[7]={x-1,y+1};
            break;
        case DOWN:
            range[0]={x+1,y};range[1]={x+2,y};range[2]={x,y+1};range[3]={x,y+2};
            range[4]={x,y-1};range[5]={x,y-2};range[6]={x+1,y-1};range[7]={x+1,y+1};
            break;
        case LEFT:
            range[0]={x,y-1};range[1]={x,y-2};range[2]={x-1,y};range[3]={x-2,y};
            range[4]={x+1,y};range[5]={x+2,y};range[6]={x-1,y-1};range[7]={x+1,y-1};
            break;
        case RIGHT:
            range[0]={x,y+1};range[1]={x,y+2};range[2]={x-1,y};range[3]={x-2,y};
            range[4]={x+1,y};range[5]={x+2,y};range[6]={x-1,y+1};range[7]={x+1,y+1};
            break;
    }
}
void SOLDIER::update_pos(int x, int y){
    pos = {x, y};
}

void SOLDIER::move(WORLD& world, Direction d){
    clear_range(world);
    switch (d) {
        case UP:
            this->pos.first--;
            break;
        case DOWN:
            this->pos.first++;
            break;
        case LEFT:
            this->pos.second--;
            break;
        case RIGHT:
            this->pos.second++;
            break;
    }
    //world.occupy(pos.first, pos.second, owner_id);
    this->direction = d;
    this->update_range();
    affect_range(world);
}

void SOLDIER::clear_range(WORLD &world){
    for (int i = 0; i < 8; i++){
        if (range[i].first < 0 || range[i].first >= world.map_size || range[i].second < 0 || range[i].second >= world.map_size){
            continue;
        }
        world.v_players[owner_id].vv_range_map[range[i].first][range[i].second] += 1;
    }
}

void SOLDIER::affect_range(WORLD &world){
    for (int i = 0; i < 8; i++){
        if (range[i].first < 0 || range[i].first >= world.map_size || range[i].second < 0 || range[i].second >= world.map_size){
            continue;
        }
        world.v_players[owner_id].vv_range_map[range[i].first][range[i].second] -= 1;
    }
}

//---------------------------------------------------------------------
//red-player

PLAYER::PLAYER(){}

PLAYER::PLAYER(int id, int bucks, bool is_max, int map_size) : id(id), bucks(bucks), is_max(is_max){
    vv_range_map = vector<vector<int>>(map_size, vector<int>(map_size, 0));
    this->v_soldier.clear();
    blocks_count = 0;
}

//void PLAYER::restart(int bucks, bool is_max, int map_size){
//    this->blocks_count = 0;
//    this->bucks = bucks;
//    this->v_soldier.clear();
//    this->is_max = is_max;
//    vv_range_map = vector<vector<int>>(map_size, vector<int>(map_size, -1));
//}



//---------------------------------------------------------------------
//red-cycle


CYCLE::CYCLE(int count, int cycle_owner_id) : count(count), cycle_owner_id(cycle_owner_id){
    opponent_id = cycle_owner_id == 0 ? 1 : 0;
}

void CYCLE::end_cycle(WORLD &world){
    pay(world);
    this->count++;
    //cycle_owner_id = cycle_owner_id == 0 ? 1 : 0;
    swap(cycle_owner_id, opponent_id);
}
void CYCLE::pay(WORLD &world){
    PLAYER& p = world.v_players[cycle_owner_id];
    p.bucks += p.blocks_count * 2;
} 

void CYCLE::revert(WORLD &world){
    swap(cycle_owner_id, opponent_id);
    if (this->count == 0){
        throw std::runtime_error("can't revert cycle 0");
    }
    PLAYER& p = world.v_players[cycle_owner_id];
    p.bucks -= p.blocks_count * 2;

    this->count--;

}



//---------------------------------------------------------------------
//red-world

WORLD::WORLD(int size, int cycle_count, int active_player_id, pair<int, int> p1, pair<int, int> p2) : map_size(size),
                vv_map(vector<vector<int>>(size, vector<int>(size))){
    this->update_world(p1, p2);
    this->cycle = CYCLE(cycle_count, active_player_id);
}

void WORLD::update_world(pair<int, int> p1, pair<int, int> p2) {
    //total update of the world, after each turn
    //TODO: BUG
    //this->v_players[p1.first].restart(p1.second, true, map_size);
    //this->v_players[p2.first].restart(p2.second, false, map_size);
    v_players[p1.first] = PLAYER(p1.first, p1.second, true, map_size);
    v_players[p2.first] = PLAYER(p2.first, p2.second, false, map_size);
    for (int i = 0; i < map_size; i++) {
       for (int j = 0; j < map_size; j++) {
            int block_owner; // The playerId of this box owned player
            int x; // This block's position x
            int y; // This block's position y
            cin >> block_owner >> x >> y; cin.ignore();
            if (block_owner == 0 || block_owner == 1){
                vv_map[y][x] = block_owner;
                //v_players[block_owner].update_blocks_count(1);
                v_players[block_owner].blocks_count += 1;
            }
            else{
                vv_map[y][x] = -1;
            }
       }
    }
    this->populate();
}

void WORLD::populate(){
    int active_soldier_count; // Total no. of active soldier in the game
    cin >> active_soldier_count; cin.ignore();
    //v_players[0].v_soldier.resize(6);
    //v_players[1].v_soldier.resize(6);
    for (int i = 0; i < active_soldier_count; i++) {
        int owner_id; // owner of the soldier
        int x; // This soldier's position x
        int y; // This soldier's position y
        int soldier_id; // The unique identifier of soldier
        int level; // Level of the soldier ignore for first league
        int direction; // The side where the soldier is facing 0 = UP, 1 = LEFT , 2 = DOWN, 3 = RIGHTT
        cin >> owner_id >> x >> y >> soldier_id >> level >> direction; cin.ignore();
        SOLDIER S = SOLDIER(soldier_id, (Direction)direction, {y, x}, owner_id, level);
        //this->v_players[owner_id].v_soldier.push_back(S);
        this->v_players[owner_id].v_soldier[soldier_id] = S;
        this->v_players[owner_id].v_soldier[soldier_id].update_range();
        this->v_players[owner_id].v_soldier[soldier_id].affect_range(*this);
    }
}

bool WORLD::is_legal(Direction d, SOLDIER& S){
    //for(SOLDIER& s : v_players[S.owner_id].v_soldier){
    //    if(s.get_pos() == S.get_pos()){
    //        return false;
    //    }
    //}
    switch (d) {
        case UP:
            if (S.direction == DOWN || S.pos.first==0)
                return false;
            else{
                for (auto &p : v_players[S.owner_id].v_soldier)
                    if (p.second.pos == make_pair(S.pos.first - 1, S.pos.second)) return false;
                return true;
            }
        case DOWN:
            if (S.direction == UP || S.pos.first== map_size - 1)
                return false;
            else{
                for (auto &p : v_players[S.owner_id].v_soldier)
                    if (p.second.pos == make_pair(S.pos.first + 1, S.pos.second)) return false;
                return true;
            }
        case LEFT:
            if (S.direction == RIGHT || S.pos.second == 0)
                return false;
            else{
                for (auto &p : v_players[S.owner_id].v_soldier)
                    if (p.second.pos == make_pair(S.pos.first, S.pos.second - 1)) return false;
                return true;
            }
        case RIGHT:
            if (S.direction == LEFT || S.pos.second==map_size - 1)
                return false;
            else{
                for (auto &p : v_players[S.owner_id].v_soldier)
                    if (p.second.pos == make_pair(S.pos.first, S.pos.second + 1)) return false;
                return true;
            }
    }
    return false;
}

bool WORLD::enemy_in_range(WORLD& world, int player_id, int opponent_id){

    //for (auto &p : v_players[player_id].v_soldier) {
    //    for (auto &o : v_players[opponent_id].v_soldier) {
    //        auto it = find(begin(p.second.range), end(p.second.range), o.second.pos);
    //        if(it != end(p.second.range)) return true;
    //    }
    //}
    for (auto &p : v_players[opponent_id].v_soldier) {
        if(world.v_players[player_id].vv_range_map[p.second.pos.first][p.second.pos.second] < 0){
            return true;
        }
    }
    //for (SOLDIER S : v_players[player_id].v_soldier){
    //    for (SOLDIER O : v_players[opponent_id].v_soldier){
    //        auto it = find(begin(S.range), end(S.range), O.pos);
    //        if(it != end(S.range)) return true;
    //    }
    //}
    return false;
}

bool WORLD::soldier_in_range(int player_id, int opponent_id, int soldier_id){

    for (auto &p : v_players[player_id].v_soldier) {
        auto it = find(begin(p.second.range), end(p.second.range), v_players[opponent_id].v_soldier[soldier_id].pos);
        if(it != end(p.second.range)) return true;
    }

    //for (SOLDIER S : v_players[player_id].v_soldier){
    //    for (SOLDIER O : v_players[opponent_id].v_soldier){
    //        auto it = find(begin(S.range), end(S.range), O.pos);
    //        if(it != end(S.range)) return true;
    //    }
    //}
    return false;
}


ACTION WORLD::attack_enemy(int player_id, int opponent_id){
    ACTION action; 
    for (auto &p : v_players[player_id].v_soldier) {
        for (auto &o : v_players[opponent_id].v_soldier) {
            auto it = find(begin(p.second.range), end(p.second.range), o.second.pos);
            if(it != end(p.second.range)){
                action = ACTION(ATTACK, p.first, o.first, UP, "");
                return action;
            }

        }
    }

    //for (SOLDIER S : v_players[player_id].v_soldier){
    //    for (SOLDIER O : v_players[opponent_id].v_soldier){
    //        auto it = find(begin(S.range), end(S.range), O.pos);
    //        if(it != end(S.range)){
    //            action = ACTION(ATTACK, S.id, O.id, UP, "");
    //            return action;
    //        }
    //    }
    //}
    return action;
}

int WORLD::occupy(int x, int y, int player_id){
    int op_id = player_id == 0 ? 1 : 0;
    int x_owner_id = vv_map[x][y];
    if(x_owner_id == op_id){
        v_players[op_id].blocks_count -= 1;
        v_players[player_id].blocks_count += 1;
        vv_map[x][y] = player_id;
    }
    else if (x_owner_id == -1){
        v_players[player_id].blocks_count += 1;
        vv_map[x][y] = player_id;
    }
    return x_owner_id;
}

void WORLD::unoccupy(int x, int y, int player_id, int x_owner_id){
    int op_id = player_id == 0 ? 1 : 0;
    if (x_owner_id == op_id){
        v_players[op_id].blocks_count += 1;
        v_players[player_id].blocks_count-=1;
        vv_map[x][y] = op_id;
    }
    else if(x_owner_id == -1){
        v_players[player_id].blocks_count-=1;
        vv_map[x][y] = -1;
    }
}

void WORLD::print_world(){
    for (int i = 0; i < map_size; i++) {
        for (int j = 0; j < map_size; j++) {
            cerr << vv_map[i][j] << " ";
        }
        cerr << endl;
    }
}

//---------------------------------------------------------------------
//red-minimax

int evaluate(WORLD& world){
    int eval=0;
    PLAYER& player = world.v_players[world.cycle.cycle_owner_id];
    PLAYER& opponent = world.v_players[world.cycle.opponent_id];
    //if(world.enemy_in_range(player.id, opponent.id)){
    //    eval += pow(world.map_size, 4) * (player.is_max ? 1 : -1);
    //    //cerr << "player: " << player.id << " can can attack: " << opponent.id << endl;
    //}
    eval = player.blocks_count * (player.is_max ? 1 : -1) + 
            opponent.blocks_count * (opponent.is_max ? 1 : -1);
    return eval;
}

int minimax(WORLD &world, int max_depth, int last_soldier_id){

    array<Direction, 4> directions = {UP, DOWN, LEFT, RIGHT};
    PLAYER& player = world.v_players[world.cycle.cycle_owner_id];
    PLAYER& opponent = world.v_players[world.cycle.opponent_id];

    //if (world.enemy_in_range(player.id, opponent.id)){
    //    return pow(world.map_size, 4) * (player.is_max ? 1 : -1);
    //}

    //if (world.soldier_in_range(player.id, opponent.id, last_soldier_id)){
    //    return pow(world.map_size, 4) * (player.is_max ? 1 : -1);
    //}
    if (world.enemy_in_range(world, player.id, opponent.id)){
        return pow(world.map_size, 4) * (player.is_max ? 1 : -1) + evaluate(world);
    }

    int score = evaluate(world);

    if (world.cycle.count >= 200){
        return score;
    }
    if (max_depth == 0)
    {
        return score;
    }
    if (player.is_max){
        //int best_score = -pow(world.map_size, 4);
        int best_score = -numeric_limits<int>::max();
        //for (SOLDIER& s : player.v_soldier){
        for (auto &s : player.v_soldier) {
            for(Direction d : directions){
                if(world.is_legal(d, s.second)){
                    ACTION action(MOVE, s.first, d, "");
                    action.apply(world);
                    best_score = max(best_score, minimax(world, max_depth - 1, s.first));
                    action.undo(world);
                    int tmp = 0;
                }
            }
        }
        return best_score;
    }
    else{
        //int best_score = pow(world.map_size, 4);
        int best_score = numeric_limits<int>::max();
        //for (SOLDIER& s : player.v_soldier){
        for (auto &s : player.v_soldier) {
            for(Direction d : directions){
                if(world.is_legal(d, s.second)){
                    ACTION action(MOVE, s.first, d, "");
                    action.apply(world);
                    best_score = min(best_score, minimax(world, max_depth - 1, s.first));
                    action.undo(world);
                    int tmp = 0;
                }
            }
        }
        return best_score;
    }
}

list<ACTION> openinng_book_red = {
    ACTION(MOVE, 1, RIGHT, ""),
    ACTION(MOVE, 1, DOWN, ""),
    ACTION(MOVE, 1, RIGHT, ""),
    ACTION(MOVE, 1, RIGHT, ""),
    ACTION(MOVE, 2, DOWN, ""),
    ACTION(MOVE, 2, DOWN, ""),
    ACTION(MOVE, 2, DOWN, ""),
    ACTION(MOVE, 2, RIGHT, ""),
    ACTION(MOVE, 2, RIGHT, ""),
    ACTION(MOVE, 2, RIGHT, ""),
    ACTION(MOVE, 0, RIGHT, ""),
    ACTION(MOVE, 0, RIGHT, ""),
    ACTION(MOVE, 0, RIGHT, ""),
};

list<ACTION> openinng_book_blue = {
    ACTION(MOVE, 4, LEFT, ""),
    ACTION(MOVE, 4, UP, ""),
    ACTION(MOVE, 4, DOWN, ""),
    ACTION(MOVE, 4, LEFT, ""),
    ACTION(MOVE, 5, UP, ""),
    ACTION(MOVE, 5, UP, ""),
    ACTION(MOVE, 5, UP, ""),
    ACTION(MOVE, 5, LEFT, ""),
    ACTION(MOVE, 5, LEFT, ""),
    ACTION(MOVE, 5, LEFT, ""),
    ACTION(MOVE, 3, LEFT, ""),
    ACTION(MOVE, 3, LEFT, ""),
    ACTION(MOVE, 3, LEFT, ""),
};


ACTION find_best_action(WORLD &world, int max_depth, list<ACTION> &openinng){
    array<Direction, 4> directions = {UP, DOWN, LEFT, RIGHT};
    PLAYER& player = world.v_players[world.cycle.cycle_owner_id];
    PLAYER& opponent = world.v_players[world.cycle.opponent_id];

    //int best_score = -pow(world.map_size, 4);
    int best_score = -numeric_limits<int>::max();
    ACTION best_action;

    for (auto & o_s : opponent.v_soldier) {
        if(o_s.second.level > 0){
            ACTION action(DEGRADE, o_s.first);
            return action;
        }
    }

    if (world.enemy_in_range(world, player.id, opponent.id)){
        return world.attack_enemy(player.id, opponent.id);
    }
    //for (SOLDIER& o_s : opponent.v_soldier){
    //for (SOLDIER& s : player.v_soldier){

    for (auto it = openinng.begin(); it != openinng.end();it++){
        ACTION action = *it;
        action.apply(world);
        int action_score = minimax(world, max_depth - 1, 0);
        action.undo(world);
        if(action_score >= 0){
            openinng.erase(it);
            return action;
        } 
    }
    openinng.clear();
    for (auto & s : player.v_soldier) {
        for(Direction d : directions){
            if(world.is_legal(d, s.second)){
                ACTION action(MOVE, s.first, d, "");
                action.apply(world);
                int action_score = minimax(world, max_depth, s.first);
                cerr << "action score: " << action_score << " action: " << S_directions[d] << " soldier id: " << s.first << endl;
                action.undo(world);
                if(action_score > best_score){
                    best_action = action;
                    best_score = action_score;  
                }
            }
        }
    }
    return best_action;
}


//ACTION openning(WORLD &world){

//}


//---------------------------------------------------------------------
//red-main

int main()
{
    int max_depth = 198;
    int my_id; // Your unique player Id
    cin >> my_id; cin.ignore();
    int opp_id = my_id == 1 ? 0 : 1;
    int map_size; // the size of map MapSize*MapSize
    cin >> map_size; cin.ignore();
    list<ACTION> openinng;
    if (my_id == 0){
        openinng = openinng_book_red;
    }
    else{
        openinng = openinng_book_blue;
    }


    // game loop
    while (1) {

        
        int my_bucks; // Your Money
        cin >> my_bucks; cin.ignore();
        int opp_bucks; // Opponent Money
        cin >> opp_bucks; cin.ignore();
        WORLD world(map_size, 0, my_id, {my_id, my_bucks}, {opp_id, opp_bucks});
        clock_t start = clock();
        ACTION action = find_best_action(world, 4, openinng);
        cerr << "Time: " << (clock() - start) / (double)(CLOCKS_PER_SEC / 1000) << " ms" << endl;
        action.apply(world, true);

        //for (int i = 0; i < map_size; i++) {
        //    for (int j = 0; j < map_size; j++) {
        //        int block_owner; // The playerId of this box owned player
        //        int x; // This block's position x
        //        int y; // This block's position y
        //        cin >> block_owner >> x >> y; cin.ignore();
        //    }
        //}
        //int active_soldier_count; // Total no. of active soldier in the game
        //cin >> active_soldier_count; cin.ignore();
        //for (int i = 0; i < active_soldier_count; i++) {
        //    int owner_id; // owner of the soldier
        //    int x; // This soldier's position x
        //    int y; // This soldier's position y
        //    int soldier_id; // The unique identifier of soldier
        //    int level; // Level of the soldier ignore for first league
        //    int direction; // The side where the soldier is facing 0 = UP, 1 = LEFT , 2 = DOWN, 3 = RIGHTT
        //    cin >> owner_id >> x >> y >> soldier_id >> level >> direction; cin.ignore();
        //}

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // print any of actions - WAIT | MOVE <soldierId> <direction> | ATTACK <soldierID> <soldierId to attack on> | LATER > UPGRADE <id> | DEGRADE <opponent id> | SUICIDE <id>
        return 0;
    }
}
