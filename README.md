# Gaming_Leaderboard

🎮 Gaming Leaderboard using AVL Tree (C++)
📌 Project Overview

This project implements a Gaming Leaderboard System using an AVL Tree data structure in C++.
The leaderboard efficiently manages player records by maintaining a self-balancing binary search tree, ensuring fast insertion, deletion, searching, and ranking operations.

The project demonstrates practical application of Data Structures and Algorithms (DSA) concepts such as:
AVL Trees
Tree rotations
Time complexity optimization
Real-world problem solving

🧠 Why AVL Tree?

A gaming leaderboard requires:
Fast updates when scores change
Sorted order of players by score
Efficient retrieval of top player

AVL Tree guarantees:

O(log n) time complexity for insert, delete, and search
Automatically balanced tree after every operation

🛠️ Functional Description
1️⃣ Insert Player

Adds a new player to the leaderboard
Maintains AVL balance using rotations

2️⃣ Delete Player

Removes a player from the leaderboard
Tree re-balances automatically after deletion

3️⃣ Find Player

Searches for a specific player efficiently
Displays player details if found

4️⃣ Display Top N Players

Displays the top N players based on score
Uses reverse in-order traversal for ranking

5️⃣ Display All Players

Shows all players in sorted order
Demonstrates tree traversal

6️⃣ Get Statistics

Displays useful statistics such as:
Total number of players
Height of the AVL tree
Tree balance information

7️⃣ Exit

Safely exits the program

🎯 Learning Outcomes

Hands-on understanding of AVL Trees
Real-world implementation of self-balancing BST
Improved problem-solving and DSA skills
Experience with C++ project structuring
