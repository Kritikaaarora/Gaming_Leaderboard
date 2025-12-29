#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
using namespace std;

struct Player {
    string name;
    int score;
    int rank;
    
    Player(string n = "", int s = 0) : name(n), score(s), rank(0) {}
};

class AVLNode {
public:
    Player player;
    AVLNode* left;
    AVLNode* right;
    int height;
    int subtreeSize;
    
    AVLNode(Player p) : player(p), left(nullptr), right(nullptr), height(1), subtreeSize(1) {}
};

class Leaderboard {
private:
    AVLNode* root;
    int rotationCount;
    const string filename = "leaderboard.txt";
 
    int getHeight(AVLNode* node) {
        return node ? node->height : 0;
    }
    
    int getSize(AVLNode* node) {
        return node ? node->subtreeSize : 0;
    }
    
    void updateNode(AVLNode* node) {
        if (node) {
            node->height = 1 + max(getHeight(node->left), getHeight(node->right));
            node->subtreeSize = 1 + getSize(node->left) + getSize(node->right);
        }
    }
   
    int getBalance(AVLNode* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }
    
    AVLNode* rightRotate(AVLNode* y) {
        rotationCount++;
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        updateNode(y);
        updateNode(x);
        
        return x;
    }
    
    AVLNode* leftRotate(AVLNode* x) {
        rotationCount++;
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        updateNode(x);
        updateNode(y);
        
        return y;
    }
    
    AVLNode* insertNode(AVLNode* node, Player player) {
        if (!node) return new AVLNode(player);
        
        // Insert based on score (primary) and name (secondary)
        if (player.score < node->player.score)
            node->left = insertNode(node->left, player);
        else if (player.score > node->player.score)
            node->right = insertNode(node->right, player);
        else {
            // Equal scores, sort by name
            if (player.name < node->player.name)
                node->left = insertNode(node->left, player);
            else
                node->right = insertNode(node->right, player);
        }
        
        updateNode(node);
        
        int balance = getBalance(node);
        
        // Left Left Case
        if (balance > 1 && (player.score < node->left->player.score || 
            (player.score == node->left->player.score && player.name < node->left->player.name)))
            return rightRotate(node);
        
        // Right Right Case
        if (balance < -1 && (player.score > node->right->player.score ||
            (player.score == node->right->player.score && player.name > node->right->player.name)))
            return leftRotate(node);
        
        // Left Right Case
        if (balance > 1 && (player.score > node->left->player.score ||
            (player.score == node->left->player.score && player.name > node->left->player.name))) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        
        // Right Left Case
        if (balance < -1 && (player.score < node->right->player.score ||
            (player.score == node->right->player.score && player.name < node->right->player.name))) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        
        return node;
    }  
   
    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current->left) current = current->left;
        return current;
    }
  
    AVLNode* deleteNode(AVLNode* node, string name) {
        if (!node) return node;
            
        if (name < node->player.name)
            node->left = deleteNode(node->left, name);
        else if (name > node->player.name)
            node->right = deleteNode(node->right, name);
        else {
            // Node to be deleted found
            if (!node->left || !node->right) {
                AVLNode* temp = node->left ? node->left : node->right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp;
                }
                delete temp;
            } else {
                // Node with two children: Get the inorder successor
                AVLNode* temp = minValueNode(node->right);
                // FIXED: Only copy name and score, not the entire player (which includes old rank)
                node->player.name = temp->player.name;
                node->player.score = temp->player.score;
                // Don't copy the rank - it will be recalculated by updateRanks()
                node->right = deleteNode(node->right, temp->player.name);
            }
        }
        
        if (!node) return node;       
        updateNode(node);
        
        int balance = getBalance(node);
        
        // Left Left Case
        if (balance > 1 && getBalance(node->left) >= 0)
            return rightRotate(node);
        
        // Left Right Case
        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        
        // Right Right Case
        if (balance < -1 && getBalance(node->right) <= 0)
            return leftRotate(node);
        
        // Right Left Case
        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }   
        return node;
    }
    
    // Search by name
    AVLNode* search(AVLNode* node, string name) {
        if (!node || node->player.name == name)
            return node;
        if (name < node->player.name)
            return search(node->left, name);
        return search(node->right, name);
    }
    
    // Traverse in reverse order (highest to lowest score)
    void inOrderReverse(AVLNode* node, vector<Player>& players) {
        if (!node) return;
        inOrderReverse(node->right, players);
        players.push_back(node->player);
        inOrderReverse(node->left, players);
    }
   
    void updateRanks() {
        vector<Player> players;
        inOrderReverse(root, players);
        
        for (int i = 0; i < players.size(); i++) {
            AVLNode* node = search(root, players[i].name);
            if (node) node->player.rank = i + 1;
        }
    }

    void saveToFileHelper(AVLNode* node, ofstream& file) {
        if (!node) return;
        file << node->player.name << "," << node->player.score << "," << node->player.rank << "\n";
        saveToFileHelper(node->left, file);
        saveToFileHelper(node->right, file);
    }
    
    void clearTree(AVLNode* node) {
        if (!node) return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
    
public:
    Leaderboard() : root(nullptr), rotationCount(0) {
        loadFromFile();
    }
    
    ~Leaderboard() {
        clearTree(root);
    }
    
    // Insert a new player
    void insertPlayer(string name, int score) {
        rotationCount = 0;
        root = insertNode(root, Player(name, score));
        updateRanks();
        saveToFile();
        cout << "Player '" << name << "' inserted with score " << score 
             << " (Rotations: " << rotationCount << ")\n";
    }
    
    // Delete a player
    void deletePlayer(string name) {
        if (!search(root, name)) {
            cout << "Player '" << name << "' not found!\n";
            return;
        }
        rotationCount = 0;
        root = deleteNode(root, name);
        updateRanks();
        saveToFile();
        cout << "Player '" << name << "' deleted (Rotations: " << rotationCount << ")\n";
    }
    
    // Find and display a specific player
    void findPlayer(string name) {
        AVLNode* node = search(root, name);
        if (node) {
            cout << "\n=== Player Found ===\n";
            cout << "Name: " << node->player.name << "\n";
            cout << "Score: " << node->player.score << "\n";
            cout << "Rank: " << node->player.rank << "\n";
        } else {
            cout << "Player '" << name << "' not found!\n";
        }
    }
    
    // Get top N players
    void getTopNPlayers(int n) {
        vector<Player> players;
        inOrderReverse(root, players);
        
        cout << "\n=== Top " << min(n, (int)players.size()) << " Players ===\n";
        cout << left << setw(20) << "Name" << setw(10) << "Score" << setw(10) << "Rank" << "\n";
        cout << string(40, '-') << "\n";
        
        for (int i = 0; i < min(n, (int)players.size()); i++) {
            cout << left << setw(20) << players[i].name 
                 << setw(10) << players[i].score 
                 << setw(10) << players[i].rank << "\n";
        }
    }
    
    // Display all players
    void displayAll() {
        vector<Player> players;
        inOrderReverse(root, players);
        
        if (players.empty()) {
            cout << "\nLeaderboard is empty!\n";
            return;
        }
        
        cout << "\n=== Complete Leaderboard ===\n";
        cout << left << setw(20) << "Name" << setw(10) << "Score" << setw(10) << "Rank" << "\n";
        cout << string(40, '-') << "\n";
        
        for (const auto& p : players) {
            cout << left << setw(20) << p.name 
                 << setw(10) << p.score 
                 << setw(10) << p.rank << "\n";
        }
        cout << "\nTotal Players: " << players.size() << "\n";
    }
    
    // Save to file
    void saveToFile() {
        ofstream file(filename);
        if (!file) {
            cout << "Error opening file for writing!\n";
            return;
        }
        saveToFileHelper(root, file);
        file.close();
    }
    
    // Load from file
    void loadFromFile() {
        ifstream file(filename);
        if (!file) return; 
        
        string line, name;
        int score, rank;
        
        while (getline(file, line)) {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            
            if (pos1 != string::npos && pos2 != string::npos) {
                name = line.substr(0, pos1);
                score = stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
                root = insertNode(root, Player(name, score));
            }
        }
        file.close();
        updateRanks();
    }
    
    // Get statistics
    void getStatistics() {
        vector<Player> players;
        inOrderReverse(root, players);
        
        cout << "\n=== Leaderboard Statistics ===\n";
        cout << "Total Players: " << players.size() << "\n";
        cout << "Tree Height: " << getHeight(root) << "\n";
        
        if (!players.empty()) {
            cout << "Highest Score: " << players[0].score 
                 << " (" << players[0].name << ")\n";
            cout << "Lowest Score: " << players.back().score 
                 << " (" << players.back().name << ")\n";
        }
    }
};


void displayMenu() {
    cout << "\n------------------------------------\n";
    cout << "  ||  GAMING LEADERBOARD SYSTEM     ||\n";
    cout << "\n------------------------------------\n";
    cout << "1. Insert Player\n";
    cout << "2. Delete Player\n";
    cout << "3. Find Player\n";
    cout << "4. Display Top N Players\n";
    cout << "5. Display All Players\n";
    cout << "6. Get Statistics\n";
    cout << "7. Exit\n";
    cout << "Enter choice: ";
}

int main() {
    Leaderboard leaderboard;
    int choice, score, n;
    string name;
    
    while (true) {
        displayMenu();
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1:
                cout << "Enter player name: ";
                getline(cin, name);
                cout << "Enter score: ";
                cin >> score;
                leaderboard.insertPlayer(name, score);
                break;
                
            case 2:
                cout << "Enter player name to delete: ";
                getline(cin, name);
                leaderboard.deletePlayer(name);
                break;
                
            case 3:
                cout << "Enter player name to find: ";
                getline(cin, name);
                leaderboard.findPlayer(name);
                break;
                
            case 4:
                cout << "Enter number of top players to display: ";
                cin >> n;
                leaderboard.getTopNPlayers(n);
                break;
                
            case 5:
                leaderboard.displayAll();
                break;
                
            case 6:
                leaderboard.getStatistics();
                break;
                
            case 7:
                cout << "Thank you for using Gaming Leaderboard System!\n";
                return 0;
                
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    }
    
    return 0;
}