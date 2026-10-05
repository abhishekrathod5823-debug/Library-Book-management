/**
 * ============================================================================
 * PROJECT TITLE: LIBRARY BOOK MANAGEMENT SYSTEM
 * LANGUAGE: C++
 * PURPOSE: College Data Structures Project & Viva
 *
 * CORE DATA STRUCTURES DEMONSTRATED IN THIS PROJECT:
 *  1. ARRAY              - Fixed/Dynamic in-memory storage of book records
 *  2. 2D ARRAY           - Physical library shelf grid (Shelves x Positions)
 *  3. SINGLY LINKED LIST - Dynamic tracking of currently issued books
 *  4. STACK              - LIFO storage of recently returned books (for reshelving)
 *  5. QUEUE              - FIFO book reservation / waiting list
 *  6. CIRCULAR QUEUE     - Library counter / token queue management
 *  7. BINARY SEARCH TREE - Fast O(log n) search by Book ID
 *  8. RECURSION          - Used in BST Insertion, Search, and Traversals
 *  9. SEARCHING          - Linear Search (Title/Author/Category) & BST Search (ID)
 * 10. SORTING            - Selection Sort by ID, Title, and Author
 * 11. FILE HANDLING      - Persistent storage in books.txt, members.txt, etc.
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <algorithm>

using namespace std;

// Maximum capacities for static structures
const int MAX_BOOKS = 100;
const int MAX_MEMBERS = 50;
const int NUM_SHELVES = 5;
const int SHELF_CAPACITY = 10;
const int MAX_TOKENS = 10;

// Permanent File Storage Names
const string BOOK_FILE = "books.txt";
const string MEMBER_FILE = "members.txt";
const string ISSUED_FILE = "issued_books.txt";
const string RESERVATION_FILE = "reservations.txt";

// ============================================================================
// HELPER UTILITIES: String Processing
// ============================================================================
string toLower(const string &str)
{
    string res = str;
    transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

string trim(const string &str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// ============================================================================
// 1. DATA STRUCTURE: ARRAY ELEMENT (Structure for Book & Member Records)
// ============================================================================
struct Book
{
    int id;
    string title;
    string author;
    string category;
    int available; // 1 = Available, 0 = Issued
};

struct Member
{
    string id;
    string name;
    string department;
};

// ============================================================================
// 2. DATA STRUCTURE: 2D ARRAY (Library Shelf Matrix Layout)
// Row = Shelf Number (0 to NUM_SHELVES - 1)
// Col = Slot Position on Shelf (0 to SHELF_CAPACITY - 1)
// Demonstrates 2D Matrix indexing and Row-Major representation
// ============================================================================
class ShelfMatrix
{
private:
    int grid[NUM_SHELVES][SHELF_CAPACITY];

public:
    ShelfMatrix()
    {
        for (int i = 0; i < NUM_SHELVES; i++)
        {
            for (int j = 0; j < SHELF_CAPACITY; j++)
            {
                grid[i][j] = 0; // 0 indicates empty slot
            }
        }
    }

    void placeBook(int bookId)
    {
        // Place book in the first available slot
        for (int i = 0; i < NUM_SHELVES; i++)
        {
            for (int j = 0; j < SHELF_CAPACITY; j++)
            {
                if (grid[i][j] == bookId)
                    return; // Already on shelf
                if (grid[i][j] == 0)
                {
                    grid[i][j] = bookId;
                    return;
                }
            }
        }
    }

    void removeBook(int bookId)
    {
        for (int i = 0; i < NUM_SHELVES; i++)
        {
            for (int j = 0; j < SHELF_CAPACITY; j++)
            {
                if (grid[i][j] == bookId)
                {
                    grid[i][j] = 0;
                    return;
                }
            }
        }
    }

    bool findPosition(int bookId, int &shelfRow, int &shelfCol)
    {
        for (int i = 0; i < NUM_SHELVES; i++)
        {
            for (int j = 0; j < SHELF_CAPACITY; j++)
            {
                if (grid[i][j] == bookId)
                {
                    shelfRow = i + 1; // 1-based indexing for display
                    shelfCol = j + 1;
                    return true;
                }
            }
        }
        return false;
    }

    void display()
    {
        cout << "\n================================================================\n";
        cout << "               2D ARRAY: PHYSICAL SHELF MATRIX                  \n";
        cout << "================================================================\n";
        cout << "  Shelf \\ Slot |";
        for (int j = 0; j < SHELF_CAPACITY; j++)
        {
            cout << setw(5) << ("S" + to_string(j + 1)) << " ";
        }
        cout << "\n---------------+";
        for (int j = 0; j < SHELF_CAPACITY; j++)
            cout << "------";
        cout << "\n";

        for (int i = 0; i < NUM_SHELVES; i++)
        {
            cout << "  Shelf " << setw(2) << (i + 1) << "     |";
            for (int j = 0; j < SHELF_CAPACITY; j++)
            {
                if (grid[i][j] == 0)
                {
                    cout << setw(5) << "[.]" << " ";
                }
                else
                {
                    cout << setw(5) << grid[i][j] << " ";
                }
            }
            cout << "\n";
        }
        cout << "----------------------------------------------------------------\n";
        cout << "Note: [.] = Empty Slot | Numbers = Book IDs placed on shelves\n";
    }
};

// ============================================================================
// 3. DATA STRUCTURE: SINGLY LINKED LIST (Issued Books Tracker)
// Dynamic linear collection of nodes tracking actively borrowed books.
// ============================================================================
struct IssueNode
{
    int bookId;
    string memberId;
    string issueDate;
    IssueNode *next;

    IssueNode(int bId, string mId, string date)
        : bookId(bId), memberId(mId), issueDate(date), next(nullptr) {}
};

class IssuedLinkedList
{
private:
    IssueNode *head;

public:
    IssuedLinkedList() : head(nullptr) {}

    ~IssuedLinkedList()
    {
        while (head != nullptr)
        {
            IssueNode *temp = head;
            head = head->next;
            delete temp;
        }
    }

    void insertIssue(int bId, string mId, string date)
    {
        IssueNode *newNode = new IssueNode(bId, mId, date);
        if (head == nullptr)
        {
            head = newNode;
        }
        else
        {
            IssueNode *temp = head;
            while (temp->next != nullptr)
            {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    bool removeIssue(int bId, string &outMemberId, string &outDate)
    {
        if (head == nullptr)
            return false;

        if (head->bookId == bId)
        {
            IssueNode *temp = head;
            outMemberId = head->memberId;
            outDate = head->issueDate;
            head = head->next;
            delete temp;
            return true;
        }

        IssueNode *curr = head;
        while (curr->next != nullptr && curr->next->bookId != bId)
        {
            curr = curr->next;
        }

        if (curr->next != nullptr)
        {
            IssueNode *temp = curr->next;
            outMemberId = temp->memberId;
            outDate = temp->issueDate;
            curr->next = temp->next;
            delete temp;
            return true;
        }
        return false;
    }

    bool isBookIssued(int bId) const
    {
        IssueNode *curr = head;
        while (curr != nullptr)
        {
            if (curr->bookId == bId)
                return true;
            curr = curr->next;
        }
        return false;
    }

    void display() const
    {
        if (head == nullptr)
        {
            cout << "\n[!] No books are currently issued.\n";
            return;
        }
        cout << "\n==============================================================\n";
        cout << "            LINKED LIST: CURRENTLY ISSUED BOOKS               \n";
        cout << "==============================================================\n";
        cout << left << setw(10) << "Book ID" << setw(15) << "Member ID" << setw(20) << "Issue Date" << "\n";
        cout << "--------------------------------------------------------------\n";
        IssueNode *curr = head;
        while (curr != nullptr)
        {
            cout << left << setw(10) << curr->bookId
                 << setw(15) << curr->memberId
                 << setw(20) << curr->issueDate << "\n";
            curr = curr->next;
        }
        cout << "==============================================================\n";
    }

    IssueNode *getHead() const { return head; }
};

// ============================================================================
// 4. DATA STRUCTURE: STACK (Recently Returned Books - LIFO Order)
// When books are returned, they are pushed onto the stack.
// The librarian inspects / reshelves the most recently returned book first.
// ============================================================================
struct StackNode
{
    int bookId;
    string title;
    string returnDate;
    StackNode *next;

    StackNode(int bId, string t, string d)
        : bookId(bId), title(t), returnDate(d), next(nullptr) {}
};

class ReturnedBookStack
{
private:
    StackNode *topNode;

public:
    ReturnedBookStack() : topNode(nullptr) {}

    ~ReturnedBookStack()
    {
        while (!isEmpty())
        {
            StackNode *temp = topNode;
            topNode = topNode->next;
            delete temp;
        }
    }

    bool isEmpty() const
    {
        return topNode == nullptr;
    }

    void push(int bId, string title, string returnDate)
    {
        StackNode *newNode = new StackNode(bId, title, returnDate);
        newNode->next = topNode;
        topNode = newNode;
    }

    bool pop(int &bId, string &title, string &returnDate)
    {
        if (isEmpty())
            return false;
        StackNode *temp = topNode;
        bId = temp->bookId;
        title = temp->title;
        returnDate = temp->returnDate;
        topNode = topNode->next;
        delete temp;
        return true;
    }

    void peek() const
    {
        if (isEmpty())
        {
            cout << "\n[!] Return Stack is empty. No recent returns.\n";
            return;
        }
        cout << "\n--- TOP OF STACK (Most Recently Returned Book) ---\n";
        cout << "Book ID: " << topNode->bookId << "\n";
        cout << "Title:   " << topNode->title << "\n";
        cout << "Date:    " << topNode->returnDate << "\n";
        cout << "--------------------------------------------------\n";
    }

    void display() const
    {
        if (isEmpty())
        {
            cout << "\n[!] Return Stack is empty.\n";
            return;
        }
        cout << "\n==============================================================\n";
        cout << "          STACK: RECENTLY RETURNED BOOKS (LIFO ORDER)         \n";
        cout << "==============================================================\n";
        cout << left << setw(10) << "Book ID" << setw(30) << "Book Title" << setw(15) << "Return Date" << "\n";
        cout << "--------------------------------------------------------------\n";
        StackNode *curr = topNode;
        int rank = 1;
        while (curr != nullptr)
        {
            cout << left << setw(10) << curr->bookId
                 << setw(30) << curr->title.substr(0, 28)
                 << setw(15) << curr->returnDate;
            if (rank == 1)
                cout << " <- TOP (Latest)";
            cout << "\n";
            curr = curr->next;
            rank++;
        }
        cout << "==============================================================\n";
    }
};

// ============================================================================
// 5. DATA STRUCTURE: QUEUE (Book Reservation Waiting List - FIFO Order)
// When a book is unavailable, students join the FIFO waiting list.
// The first student to join gets priority upon book return.
// ============================================================================
struct QueueNode
{
    int bookId;
    string memberId;
    QueueNode *next;

    QueueNode(int bId, string mId) : bookId(bId), memberId(mId), next(nullptr) {}
};

class ReservationQueue
{
private:
    QueueNode *frontNode;
    QueueNode *rearNode;

public:
    ReservationQueue() : frontNode(nullptr), rearNode(nullptr) {}

    ~ReservationQueue()
    {
        while (frontNode != nullptr)
        {
            QueueNode *temp = frontNode;
            frontNode = frontNode->next;
            delete temp;
        }
    }

    bool isEmpty() const
    {
        return frontNode == nullptr;
    }

    void enqueue(int bId, string mId)
    {
        QueueNode *newNode = new QueueNode(bId, mId);
        if (rearNode == nullptr)
        {
            frontNode = rearNode = newNode;
        }
        else
        {
            rearNode->next = newNode;
            rearNode = newNode;
        }
    }

    bool dequeueFirstReservationForBook(int bId, string &outMemberId)
    {
        if (isEmpty())
            return false;

        // Check if front node matches
        if (frontNode->bookId == bId)
        {
            QueueNode *temp = frontNode;
            outMemberId = frontNode->memberId;
            frontNode = frontNode->next;
            if (frontNode == nullptr)
                rearNode = nullptr;
            delete temp;
            return true;
        }

        // Search remaining queue
        QueueNode *curr = frontNode;
        while (curr->next != nullptr && curr->next->bookId != bId)
        {
            curr = curr->next;
        }

        if (curr->next != nullptr)
        {
            QueueNode *temp = curr->next;
            outMemberId = temp->memberId;
            curr->next = temp->next;
            if (temp == rearNode)
                rearNode = curr;
            delete temp;
            return true;
        }

        return false;
    }

    void display() const
    {
        if (isEmpty())
        {
            cout << "\n[!] No active book reservations in the waiting list.\n";
            return;
        }
        cout << "\n==============================================================\n";
        cout << "        QUEUE: BOOK RESERVATION WAITING LIST (FIFO)           \n";
        cout << "==============================================================\n";
        cout << left << setw(10) << "Position" << setw(12) << "Book ID" << setw(15) << "Member ID" << "\n";
        cout << "--------------------------------------------------------------\n";
        QueueNode *curr = frontNode;
        int pos = 1;
        while (curr != nullptr)
        {
            cout << left << setw(10) << pos
                 << setw(12) << curr->bookId
                 << setw(15) << curr->memberId;
            if (pos == 1)
                cout << " <- FRONT";
            if (curr->next == nullptr)
                cout << " <- REAR";
            cout << "\n";
            curr = curr->next;
            pos++;
        }
        cout << "==============================================================\n";
    }

    QueueNode *getFront() const { return frontNode; }
};

// ============================================================================
// 6. DATA STRUCTURE: CIRCULAR QUEUE (Counter Token System)
// Implements fixed buffer with modulo arithmetic (rear = (rear+1) % SIZE)
// ============================================================================
class TokenCircularQueue
{
private:
    int arr[MAX_TOKENS];
    int front;
    int rear;
    int count;

public:
    TokenCircularQueue() : front(-1), rear(-1), count(0) {}

    bool isFull() const
    {
        return count == MAX_TOKENS;
    }

    bool isEmpty() const
    {
        return count == 0;
    }

    bool issueToken(int &issuedTokenId)
    {
        if (isFull())
            return false;

        static int tokenCounter = 100;
        issuedTokenId = ++tokenCounter;

        if (isEmpty())
        {
            front = rear = 0;
        }
        else
        {
            rear = (rear + 1) % MAX_TOKENS; // Modulo arithmetic
        }
        arr[rear] = issuedTokenId;
        count++;
        return true;
    }

    bool serveNextToken(int &servedTokenId)
    {
        if (isEmpty())
            return false;

        servedTokenId = arr[front];
        if (front == rear)
        {
            front = rear = -1;
        }
        else
        {
            front = (front + 1) % MAX_TOKENS; // Modulo arithmetic
        }
        count--;
        return true;
    }

    void display() const
    {
        if (isEmpty())
        {
            cout << "\n[!] Token Counter Queue is currently empty.\n";
            return;
        }
        cout << "\n==============================================================\n";
        cout << "    CIRCULAR QUEUE: LIBRARY SERVICE COUNTER TOKENS            \n";
        cout << "==============================================================\n";
        cout << "Active Tokens Count: " << count << " / " << MAX_TOKENS << "\n";
        cout << "Token Flow: ";
        int idx = front;
        for (int i = 0; i < count; i++)
        {
            cout << "[" << arr[idx] << "]";
            if (i == 0)
                cout << "(Front)";
            if (i == count - 1)
                cout << "(Rear)";
            if (i < count - 1)
                cout << " -> ";
            idx = (idx + 1) % MAX_TOKENS;
        }
        cout << "\n==============================================================\n";
    }
};

// ============================================================================
// 7. DATA STRUCTURE: BINARY SEARCH TREE (BST) & RECURSION
// Keyed on Book ID for fast O(log n) lookups and recursive traversals.
// ============================================================================
struct BSTNode
{
    int id;
    string title;
    string author;
    int available;
    BSTNode *left;
    BSTNode *right;

    BSTNode(int bId, string t, string a, int av)
        : id(bId), title(t), author(a), available(av), left(nullptr), right(nullptr) {}
};

class BookBST
{
private:
    BSTNode *root;

    // RECURSION: Insert into BST
    BSTNode *insertRecursive(BSTNode *node, int id, const string &title, const string &author, int av)
    {
        if (node == nullptr)
        {
            return new BSTNode(id, title, author, av);
        }
        if (id < node->id)
        {
            node->left = insertRecursive(node->left, id, title, author, av);
        }
        else if (id > node->id)
        {
            node->right = insertRecursive(node->right, id, title, author, av);
        }
        return node;
    }

    // RECURSION: Search in BST
    BSTNode *searchRecursive(BSTNode *node, int id) const
    {
        if (node == nullptr || node->id == id)
        {
            return node;
        }
        if (id < node->id)
        {
            return searchRecursive(node->left, id);
        }
        return searchRecursive(node->right, id);
    }

    // RECURSION: Inorder Traversal (L -> Root -> R: Sorted keys)
    void inorderRecursive(BSTNode *node) const
    {
        if (node == nullptr)
            return;
        inorderRecursive(node->left);
        cout << left << setw(10) << node->id
             << setw(32) << node->title.substr(0, 30)
             << setw(22) << node->author.substr(0, 20)
             << setw(12) << (node->available ? "Available" : "Issued") << "\n";
        inorderRecursive(node->right);
    }

    // RECURSION: Preorder Traversal (Root -> L -> R)
    void preorderRecursive(BSTNode *node) const
    {
        if (node == nullptr)
            return;
        cout << "[" << node->id << ": " << node->title << "] ";
        preorderRecursive(node->left);
        preorderRecursive(node->right);
    }

    // RECURSION: Postorder Traversal (L -> R -> Root)
    void postorderRecursive(BSTNode *node) const
    {
        if (node == nullptr)
            return;
        postorderRecursive(node->left);
        postorderRecursive(node->right);
        cout << "[" << node->id << ": " << node->title << "] ";
    }

    void destroyTree(BSTNode *node)
    {
        if (node == nullptr)
            return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

public:
    BookBST() : root(nullptr) {}

    ~BookBST()
    {
        clear();
    }

    void clear()
    {
        destroyTree(root);
        root = nullptr;
    }

    void insert(int id, const string &title, const string &author, int av)
    {
        root = insertRecursive(root, id, title, author, av);
    }

    BSTNode *search(int id) const
    {
        return searchRecursive(root, id);
    }

    void inorder() const
    {
        if (root == nullptr)
        {
            cout << "\n[!] BST is empty.\n";
            return;
        }
        cout << "\n========================================================================\n";
        cout << "        BST INORDER TRAVERSAL (L-Root-R: Sorted by Book ID)             \n";
        cout << "========================================================================\n";
        cout << left << setw(10) << "Book ID" << setw(32) << "Title" << setw(22) << "Author" << setw(12) << "Status" << "\n";
        cout << "------------------------------------------------------------------------\n";
        inorderRecursive(root);
        cout << "========================================================================\n";
    }

    void preorder() const
    {
        if (root == nullptr)
        {
            cout << "\n[!] BST is empty.\n";
            return;
        }
        cout << "\n--- BST PREORDER TRAVERSAL (Root-L-R) ---\n";
        preorderRecursive(root);
        cout << "\n";
    }

    void postorder() const
    {
        if (root == nullptr)
        {
            cout << "\n[!] BST is empty.\n";
            return;
        }
        cout << "\n--- BST POSTORDER TRAVERSAL (L-R-Root) ---\n";
        postorderRecursive(root);
        cout << "\n";
    }
};

// ============================================================================
// 8. MASTER SYSTEM CLASS: LIBRARY SYSTEM
// Integrates File Handling, Searching, Sorting, and Data Structure Workflows
// ============================================================================
class LibrarySystem
{
private:
    Book books[MAX_BOOKS]; // 1D Array for loaded books
    int bookCount;

    Member members[MAX_MEMBERS]; // 1D Array for registered members
    int memberCount;

    ShelfMatrix shelf;             // 2D Array for shelf layout
    IssuedLinkedList issuedList;   // Singly Linked List for issued books
    ReturnedBookStack returnStack; // Stack for recent returns (LIFO)
    ReservationQueue reserveQueue; // Queue for reservations (FIFO)
    TokenCircularQueue tokenQueue; // Circular Queue for counter tokens
    BookBST bst;                   // Binary Search Tree for O(log n) lookups

    void ensureFilesExist()
    {
        ifstream bIn(BOOK_FILE);
        if (!bIn.is_open())
        {
            ofstream bOut(BOOK_FILE);
            bOut << "101|C++ Programming|Bjarne Stroustrup|Computer Science|1\n";
            bOut << "102|Data Structures and Algorithms|Mark Allen Weiss|Computer Science|1\n";
            bOut << "103|Database Management Systems|Raghu Ramakrishnan|Database|1\n";
            bOut << "104|Operating System Concepts|Abraham Silberschatz|Systems|1\n";
            bOut << "105|Computer Networks|Andrew S. Tanenbaum|Networking|1\n";
            bOut.close();
        }
        else
        {
            bIn.close();
        }

        ifstream mIn(MEMBER_FILE);
        if (!mIn.is_open())
        {
            ofstream mOut(MEMBER_FILE);
            mOut << "M001|Rahul Sharma|Computer Science\n";
            mOut << "M002|Amit Verma|Information Technology\n";
            mOut << "M003|Pooja Patel|Electronics\n";
            mOut.close();
        }
        else
        {
            mIn.close();
        }

        ifstream iIn(ISSUED_FILE);
        if (!iIn.is_open())
        {
            ofstream iOut(ISSUED_FILE);
            iOut.close();
        }
        else
        {
            iIn.close();
        }

        ifstream rIn(RESERVATION_FILE);
        if (!rIn.is_open())
        {
            ofstream rOut(RESERVATION_FILE);
            rOut.close();
        }
        else
        {
            rIn.close();
        }
    }

public:
    LibrarySystem() : bookCount(0), memberCount(0)
    {
        ensureFilesExist();
        loadAllData();
    }

    // ========================================================================
    // FILE HANDLING: LOADING FROM FILES
    // ========================================================================
    void loadAllData()
    {
        bookCount = 0;
        memberCount = 0;
        bst.clear();

        // 1. Read books.txt into Array, BST, and 2D Shelf
        ifstream bFile(BOOK_FILE);
        if (bFile.is_open())
        {
            string line;
            while (getline(bFile, line))
            {
                line = trim(line);
                if (line.empty())
                    continue;

                stringstream ss(line);
                string idStr, title, author, cat, availStr;

                if (getline(ss, idStr, '|') &&
                    getline(ss, title, '|') &&
                    getline(ss, author, '|') &&
                    getline(ss, cat, '|') &&
                    getline(ss, availStr, '|'))
                {

                    if (bookCount < MAX_BOOKS)
                    {
                        books[bookCount].id = stoi(trim(idStr));
                        books[bookCount].title = trim(title);
                        books[bookCount].author = trim(author);
                        books[bookCount].category = trim(cat);
                        books[bookCount].available = stoi(trim(availStr));

                        // Insert into BST for O(log n) lookup
                        bst.insert(books[bookCount].id, books[bookCount].title,
                                   books[bookCount].author, books[bookCount].available);

                        // Place book onto 2D Array shelf
                        shelf.placeBook(books[bookCount].id);

                        bookCount++;
                    }
                }
            }
            bFile.close();
        }

        // 2. Read members.txt
        ifstream mFile(MEMBER_FILE);
        if (mFile.is_open())
        {
            string line;
            while (getline(mFile, line))
            {
                line = trim(line);
                if (line.empty())
                    continue;

                stringstream ss(line);
                string mId, name, dept;
                if (getline(ss, mId, '|') &&
                    getline(ss, name, '|') &&
                    getline(ss, dept, '|'))
                {
                    if (memberCount < MAX_MEMBERS)
                    {
                        members[memberCount].id = trim(mId);
                        members[memberCount].name = trim(name);
                        members[memberCount].department = trim(dept);
                        memberCount++;
                    }
                }
            }
            mFile.close();
        }

        // 3. Read issued_books.txt into Singly Linked List
        ifstream iFile(ISSUED_FILE);
        if (iFile.is_open())
        {
            string line;
            while (getline(iFile, line))
            {
                line = trim(line);
                if (line.empty())
                    continue;

                stringstream ss(line);
                string bIdStr, mId, date;
                if (getline(ss, bIdStr, '|') &&
                    getline(ss, mId, '|') &&
                    getline(ss, date, '|'))
                {
                    issuedList.insertIssue(stoi(trim(bIdStr)), trim(mId), trim(date));
                }
            }
            iFile.close();
        }

        // 4. Read reservations.txt into FIFO Queue
        ifstream rFile(RESERVATION_FILE);
        if (rFile.is_open())
        {
            string line;
            while (getline(rFile, line))
            {
                line = trim(line);
                if (line.empty())
                    continue;

                stringstream ss(line);
                string bIdStr, mId;
                if (getline(ss, bIdStr, '|') &&
                    getline(ss, mId, '|'))
                {
                    reserveQueue.enqueue(stoi(trim(bIdStr)), trim(mId));
                }
            }
            rFile.close();
        }
    }

    // ========================================================================
    // FILE HANDLING: SAVING TO FILES
    // ========================================================================
    void saveBooksToFile()
    {
        ofstream bFile(BOOK_FILE, ios::trunc);
        if (bFile.is_open())
        {
            for (int i = 0; i < bookCount; i++)
            {
                bFile << books[i].id << "|"
                      << books[i].title << "|"
                      << books[i].author << "|"
                      << books[i].category << "|"
                      << books[i].available << "\n";
            }
            bFile.close();
        }
    }

    void saveMembersToFile()
    {
        ofstream mFile(MEMBER_FILE, ios::trunc);
        if (mFile.is_open())
        {
            for (int i = 0; i < memberCount; i++)
            {
                mFile << members[i].id << "|"
                      << members[i].name << "|"
                      << members[i].department << "\n";
            }
            mFile.close();
        }
    }

    void saveIssuedToFile()
    {
        ofstream iFile(ISSUED_FILE, ios::trunc);
        if (iFile.is_open())
        {
            IssueNode *curr = issuedList.getHead();
            while (curr != nullptr)
            {
                iFile << curr->bookId << "|"
                      << curr->memberId << "|"
                      << curr->issueDate << "\n";
                curr = curr->next;
            }
            iFile.close();
        }
    }

    void saveReservationsToFile()
    {
        ofstream rFile(RESERVATION_FILE, ios::trunc);
        if (rFile.is_open())
        {
            QueueNode *curr = reserveQueue.getFront();
            while (curr != nullptr)
            {
                rFile << curr->bookId << "|"
                      << curr->memberId << "\n";
                curr = curr->next;
            }
            rFile.close();
        }
    }

    void rebuildBST()
    {
        bst.clear();
        for (int i = 0; i < bookCount; i++)
        {
            bst.insert(books[i].id, books[i].title, books[i].author, books[i].available);
        }
    }

    // ========================================================================
    // BOOK CRUD OPERATIONS
    // ========================================================================
    void addBook()
    {
        if (bookCount >= MAX_BOOKS)
        {
            cout << "\n[!] Error: Library capacity limit reached!\n";
            return;
        }

        int id;
        cout << "\nEnter Book ID (numeric, e.g. 106): ";
        if (!(cin >> id))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid Book ID entered.\n";
            return;
        }
        cin.ignore();

        for (int i = 0; i < bookCount; i++)
        {
            if (books[i].id == id)
            {
                cout << "[!] Error: Book ID " << id << " already exists in the system!\n";
                return;
            }
        }

        string title, author, category;
        cout << "Enter Book Title: ";
        getline(cin, title);
        cout << "Enter Author Name: ";
        getline(cin, author);
        cout << "Enter Category: ";
        getline(cin, category);

        books[bookCount].id = id;
        books[bookCount].title = trim(title);
        books[bookCount].author = trim(author);
        books[bookCount].category = trim(category);
        books[bookCount].available = 1;

        bst.insert(id, books[bookCount].title, books[bookCount].author, 1);
        shelf.placeBook(id);
        bookCount++;
        saveBooksToFile();
        cout << "\n[+] Success: Book \"" << title << "\" (ID: " << id << ") successfully added!\n";
    }

    void displayAllBooks()
    {
        if (bookCount == 0)
        {
            cout << "\n[!] No books found in the library.\n";
            return;
        }
        cout << "\n===========================================================================================\n";
        cout << "                            LIBRARY CATALOG (TOTAL BOOKS: " << bookCount << ")\n";
        cout << "===========================================================================================\n";
        cout << left << setw(8) << "ID"
             << setw(32) << "Title"
             << setw(22) << "Author"
             << setw(18) << "Category"
             << setw(12) << "Status" << "\n";
        cout << "-------------------------------------------------------------------------------------------\n";
        for (int i = 0; i < bookCount; i++)
        {
            cout << left << setw(8) << books[i].id
                 << setw(32) << books[i].title.substr(0, 30)
                 << setw(22) << books[i].author.substr(0, 20)
                 << setw(18) << books[i].category.substr(0, 16)
                 << setw(12) << (books[i].available == 1 ? "Available" : "Issued")
                 << "\n";
        }
        cout << "===========================================================================================\n";
    }

    void updateBook()
    {
        int id;
        cout << "\nEnter Book ID to update: ";
        if (!(cin >> id))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid ID.\n";
            return;
        }
        cin.ignore();
        int idx = -1;
        for (int i = 0; i < bookCount; i++)
        {
            if (books[i].id == id)
            {
                idx = i;
                break;
            }
        }
        if (idx == -1)
        {
            cout << "[!] Book with ID " << id << " not found.\n";
            return;
        }
        cout << "\nCurrent Details:\n";
        cout << "Title:    " << books[idx].title << "\n";
        cout << "Author:   " << books[idx].author << "\n";
        cout << "Category: " << books[idx].category << "\n";

        string title, author, category;
        cout << "\nEnter New Title (Leave blank to keep current): ";
        getline(cin, title);
        cout << "Enter New Author (Leave blank to keep current): ";
        getline(cin, author);
        cout << "Enter New Category (Leave blank to keep current): ";
        getline(cin, category);

        if (!trim(title).empty())
            books[idx].title = trim(title);
        if (!trim(author).empty())
            books[idx].author = trim(author);
        if (!trim(category).empty())
            books[idx].category = trim(category);

        rebuildBST();
        saveBooksToFile();
        cout << "\n[+] Success: Book details updated successfully!\n";
    }

    void deleteBook()
    {
        int id;
        cout << "\nEnter Book ID to delete: ";
        if (!(cin >> id))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid ID.\n";
            return;
        }
        cin.ignore();

        if (issuedList.isBookIssued(id))
        {
            cout << "[!] Error: Book ID " << id << " is currently ISSUED! Return it before deletion.\n";
            return;
        }

        int idx = -1;
        for (int i = 0; i < bookCount; i++)
        {
            if (books[i].id == id)
            {
                idx = i;
                break;
            }
        }
        if (idx == -1)
        {
            cout << "[!] Book with ID " << id << " not found.\n";
            return;
        }

        for (int i = idx; i < bookCount - 1; i++)
        {
            books[i] = books[i + 1];
        }
        bookCount--;

        shelf.removeBook(id);
        rebuildBST();
        saveBooksToFile();
        cout << "\n[+] Success: Book ID " << id << " permanently removed from the catalog.\n";
    }

    // ========================================================================
    // SEARCHING ALGORITHMS (Linear Search by Title, Author, Category)
    // ========================================================================
    void searchByTitle()
    {
        cin.ignore();
        string query;
        cout << "\nEnter Book Title (or keyword): ";
        getline(cin, query);
        query = toLower(trim(query));

        bool found = false;
        cout << "\n--- Search Results for Title containing \"" << query << "\" ---\n";
        for (int i = 0; i < bookCount; i++)
        {
            if (toLower(books[i].title).find(query) != string::npos)
            {
                cout << "ID: " << books[i].id
                     << " | Title: " << books[i].title
                     << " | Author: " << books[i].author
                     << " | Status: " << (books[i].available ? "Available" : "Issued") << "\n";
                found = true;
            }
        }
        if (!found)
            cout << "[!] No books matched your search query.\n";
    }

    void searchByAuthor()
    {
        cin.ignore();
        string query;
        cout << "\nEnter Author Name: ";
        getline(cin, query);
        query = toLower(trim(query));

        bool found = false;
        cout << "\n--- Search Results for Author containing \"" << query << "\" ---\n";
        for (int i = 0; i < bookCount; i++)
        {
            if (toLower(books[i].author).find(query) != string::npos)
            {
                cout << "ID: " << books[i].id
                     << " | Title: " << books[i].title
                     << " | Author: " << books[i].author
                     << " | Status: " << (books[i].available ? "Available" : "Issued") << "\n";
                found = true;
            }
        }
        if (!found)
            cout << "[!] No books found for the given author.\n";
    }

    void searchByCategory()
    {
        cin.ignore();
        string query;
        cout << "\nEnter Category Name: ";
        getline(cin, query);
        query = toLower(trim(query));

        bool found = false;
        cout << "\n--- Search Results for Category: \"" << query << "\" ---\n";
        for (int i = 0; i < bookCount; i++)
        {
            if (toLower(books[i].category).find(query) != string::npos)
            {
                cout << "ID: " << books[i].id
                     << " | Title: " << books[i].title
                     << " | Author: " << books[i].author
                     << " | Status: " << (books[i].available ? "Available" : "Issued") << "\n";
                found = true;
            }
        }
        if (!found)
            cout << "[!] No books found under this category.\n";
    }

    // ========================================================================
    // MEMBER MANAGEMENT
    // ========================================================================
    bool memberExists(const string &mId)
    {
        for (int i = 0; i < memberCount; i++)
        {
            if (members[i].id == mId)
                return true;
        }
        return false;
    }

    void addMember()
    {
        if (memberCount >= MAX_MEMBERS)
        {
            cout << "[!] Member limit reached.\n";
            return;
        }
        cin.ignore();
        string id, name, dept;
        cout << "\nEnter Member ID (e.g. M004): ";
        getline(cin, id);
        id = trim(id);
        if (memberExists(id))
        {
            cout << "[!] Error: Member ID " << id << " already exists!\n";
            return;
        }
        cout << "Enter Member Name: ";
        getline(cin, name);
        cout << "Enter Department: ";
        getline(cin, dept);

        members[memberCount].id = id;
        members[memberCount].name = trim(name);
        members[memberCount].department = trim(dept);
        memberCount++;

        saveMembersToFile();
        cout << "\n[+] Success: Member \"" << name << "\" (" << id << ") registered successfully!\n";
    }

    void displayMembers()
    {
        if (memberCount == 0)
        {
            cout << "\n[!] No registered members found.\n";
            return;
        }
        cout << "\n==============================================================\n";
        cout << "                 REGISTERED LIBRARY MEMBERS                   \n";
        cout << "==============================================================\n";
        cout << left << setw(12) << "Member ID" << setw(25) << "Name" << setw(25) << "Department" << "\n";
        cout << "--------------------------------------------------------------\n";
        for (int i = 0; i < memberCount; i++)
        {
            cout << left << setw(12) << members[i].id
                 << setw(25) << members[i].name
                 << setw(25) << members[i].department << "\n";
        }
        cout << "==============================================================\n";
    }

    // ========================================================================
    // ISSUE & RETURN OPERATIONS
    // ========================================================================
    void issueBook()
    {
        int bId;
        cout << "\nEnter Book ID to Issue: ";
        if (!(cin >> bId))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid Book ID.\n";
            return;
        }
        cin.ignore();

        int bIdx = -1;
        for (int i = 0; i < bookCount; i++)
        {
            if (books[i].id == bId)
            {
                bIdx = i;
                break;
            }
        }
        if (bIdx == -1)
        {
            cout << "[!] Error: Book ID " << bId << " does not exist in library.\n";
            return;
        }

        if (books[bIdx].available == 0)
        {
            cout << "\n[!] Notice: Book \"" << books[bIdx].title << "\" is ALREADY ISSUED.\n";
            cout << "Would you like to reserve this book in the waiting queue? (y/n): ";
            char ch;
            cin >> ch;
            if (ch == 'y' || ch == 'Y')
                reserveBook(bId);
            return;
        }

        string mId;
        cout << "Enter Member ID: ";
        getline(cin, mId);
        mId = trim(mId);

        if (!memberExists(mId))
        {
            cout << "[!] Error: Member ID \"" << mId << "\" is not registered in system.\n";
            return;
        }

        string issueDate;
        cout << "Enter Issue Date (DD-MM-YYYY): ";
        getline(cin, issueDate);

        books[bIdx].available = 0;
        issuedList.insertIssue(bId, mId, trim(issueDate));

        rebuildBST();
        saveBooksToFile();
        saveIssuedToFile();

        cout << "\n[+] Success: Book ID " << bId << " (\"" << books[bIdx].title
             << "\") issued to Member " << mId << "!\n";
    }

    void returnBook()
    {
        int bId;
        cout << "\nEnter Book ID to Return: ";
        if (!(cin >> bId))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid Book ID.\n";
            return;
        }
        cin.ignore();

        int bIdx = -1;
        for (int i = 0; i < bookCount; i++)
        {
            if (books[i].id == bId)
            {
                bIdx = i;
                break;
            }
        }
        if (bIdx == -1)
        {
            cout << "[!] Error: Book ID " << bId << " does not exist in catalog.\n";
            return;
        }

        string retMemberId, retIssueDate;
        bool removed = issuedList.removeIssue(bId, retMemberId, retIssueDate);
        if (!removed)
        {
            cout << "[!] Notice: This book is NOT currently marked as issued.\n";
            return;
        }

        string returnDate;
        cout << "Enter Return Date (DD-MM-YYYY): ";
        getline(cin, returnDate);

        books[bIdx].available = 1;
        returnStack.push(bId, books[bIdx].title, trim(returnDate));

        string nextMember;
        if (reserveQueue.dequeueFirstReservationForBook(bId, nextMember))
        {
            cout << "\n**************************************************************\n";
            cout << " [RESERVATION ALERT]: Member " << nextMember
                 << " was in the waiting QUEUE for this book!\n";
            cout << " Please notify them or assign this book next.\n";
            cout << "**************************************************************\n";
            saveReservationsToFile();
        }

        rebuildBST();
        saveBooksToFile();
        saveIssuedToFile();

        cout << "\n[+] Success: Book ID " << bId << " successfully returned!\n";
        cout << "[+] Pushed to 'Recently Returned Books' Stack for librarian shelving.\n";
    }

    void reserveBook(int defaultBookId = 0)
    {
        int bId = defaultBookId;
        if (bId == 0)
        {
            cout << "\nEnter Book ID to Reserve: ";
            if (!(cin >> bId))
            {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "[!] Invalid ID.\n";
                return;
            }
            cin.ignore();
        }

        bool exists = false;
        for (int i = 0; i < bookCount; i++)
        {
            if (books[i].id == bId)
            {
                exists = true;
                break;
            }
        }
        if (!exists)
        {
            cout << "[!] Book ID " << bId << " not found in library.\n";
            return;
        }

        string mId;
        cout << "Enter Member ID for Reservation: ";
        getline(cin, mId);
        mId = trim(mId);

        if (!memberExists(mId))
        {
            cout << "[!] Member ID " << mId << " does not exist. Please register first.\n";
            return;
        }

        reserveQueue.enqueue(bId, mId);
        saveReservationsToFile();
        cout << "\n[+] Success: Reservation added to FIFO Queue for Book ID "
             << bId << " under Member " << mId << "!\n";
    }

    // ========================================================================
    // SORTING ALGORITHMS (Selection Sort)
    // ========================================================================
    void sortBooksById()
    {
        for (int i = 0; i < bookCount - 1; i++)
        {
            int minIdx = i;
            for (int j = i + 1; j < bookCount; j++)
            {
                if (books[j].id < books[minIdx].id)
                    minIdx = j;
            }
            if (minIdx != i)
                swap(books[i], books[minIdx]);
        }
        saveBooksToFile();
        cout << "\n[+] Books successfully sorted by Book ID (Ascending) using Selection Sort.\n";
        displayAllBooks();
    }

    void sortBooksByTitle()
    {
        for (int i = 0; i < bookCount - 1; i++)
        {
            int minIdx = i;
            for (int j = i + 1; j < bookCount; j++)
            {
                if (toLower(books[j].title) < toLower(books[minIdx].title))
                    minIdx = j;
            }
            if (minIdx != i)
                swap(books[i], books[minIdx]);
        }
        saveBooksToFile();
        cout << "\n[+] Books successfully sorted by Title (Alphabetical) using Selection Sort.\n";
        displayAllBooks();
    }

    void sortBooksByAuthor()
    {
        for (int i = 0; i < bookCount - 1; i++)
        {
            int minIdx = i;
            for (int j = i + 1; j < bookCount; j++)
            {
                if (toLower(books[j].author) < toLower(books[minIdx].author))
                    minIdx = j;
            }
            if (minIdx != i)
                swap(books[i], books[minIdx]);
        }
        saveBooksToFile();
        cout << "\n[+] Books successfully sorted by Author Name using Selection Sort.\n";
        displayAllBooks();
    }

    // ========================================================================
    // SHELF MANAGEMENT (2D Array)
    // ========================================================================
    void displayShelf() { shelf.display(); }

    void locateBookOnShelf()
    {
        int bId;
        cout << "\nEnter Book ID to locate on physical shelf: ";
        if (!(cin >> bId))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid ID.\n";
            return;
        }

        int sRow = 0, sCol = 0;
        if (shelf.findPosition(bId, sRow, sCol))
        {
            cout << "\n[+] Physical Shelf Location for Book " << bId << ":\n";
            cout << "    -> Shelf Row:    Shelf " << sRow << "\n";
            cout << "    -> Slot Position: Slot " << sCol << "\n";
        }
        else
        {
            cout << "[!] Book ID " << bId << " is not placed on any active shelf.\n";
        }
    }

    void displayIssuedList() { issuedList.display(); }
    void displayWaitingList() { reserveQueue.display(); }
    void displayReturnStack() { returnStack.display(); }
    void peekReturnStack() { returnStack.peek(); }

    void searchBST()
    {
        int bId;
        cout << "\n[BST SEARCH] Enter Book ID to search: ";
        if (!(cin >> bId))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid ID.\n";
            return;
        }
        BSTNode *node = bst.search(bId);
        if (node != nullptr)
        {
            cout << "\n[+] Book found in Binary Search Tree!\n";
            cout << "    ID:     " << node->id << "\n";
            cout << "    Title:  " << node->title << "\n";
            cout << "    Author: " << node->author << "\n";
            cout << "    Status: " << (node->available ? "Available" : "Issued") << "\n";
        }
        else
        {
            cout << "[!] Book with ID " << bId << " NOT found in BST.\n";
        }
    }

    void showBSTInorder() { bst.inorder(); }
    void showBSTPreorder() { bst.preorder(); }
    void showBSTPostorder() { bst.postorder(); }

    void handleTokenQueue()
    {
        int opt;
        do
        {
            cout << "\n--- CIRCULAR QUEUE: COUNTER TOKEN SYSTEM ---\n";
            cout << "1. Issue New Token\n";
            cout << "2. Serve Next Token\n";
            cout << "3. Display Active Tokens\n";
            cout << "0. Back to Main Menu\n";
            cout << "Enter choice: ";
            if (!(cin >> opt))
            {
                cin.clear();
                cin.ignore(1000, '\n');
                continue;
            }
            if (opt == 1)
            {
                int tId;
                if (tokenQueue.issueToken(tId))
                {
                    cout << "[+] New Token Issued: #" << tId << "\n";
                }
                else
                {
                    cout << "[!] Queue is full! Please wait until tokens are served.\n";
                }
            }
            else if (opt == 2)
            {
                int served;
                if (tokenQueue.serveNextToken(served))
                {
                    cout << "[+] Counter now serving Token: #" << served << "\n";
                }
                else
                {
                    cout << "[!] No tokens to serve. Queue is empty.\n";
                }
            }
            else if (opt == 3)
            {
                tokenQueue.display();
            }
        } while (opt != 0);
    }
};

// ============================================================================
// MAIN APPLICATION ENTRY POINT & MENU CONTROLLER
// ============================================================================
int main()
{
    LibrarySystem library;
    int choice;

    do
    {
        cout << "\n========================================\n";
        cout << "       LIBRARY BOOK MANAGEMENT          \n";
        cout << "========================================\n";
        cout << " 1.  Add New Book\n";
        cout << " 2.  Display All Books\n";
        cout << " 3.  Search Book (Linear / Criteria)\n";
        cout << " 4.  Update Book Details\n";
        cout << " 5.  Delete Book\n";
        cout << " 6.  Add Member\n";
        cout << " 7.  Display Members\n";
        cout << " 8.  Issue Book\n";
        cout << " 9.  Return Book\n";
        cout << " 10. Display Issued Books (Linked List)\n";
        cout << " 11. Reserve Book (FIFO Queue)\n";
        cout << " 12. Display Waiting List (Queue)\n";
        cout << " 13. Recently Returned Books (Stack)\n";
        cout << " 14. BST Book Search [O(log n)]\n";
        cout << " 15. Tree Traversals (In/Pre/Post-order)\n";
        cout << " 16. Sort Books (Selection Sort)\n";
        cout << " 17. Shelf Management (2D Array)\n";
        cout << " 18. Service Counter Tokens (Circular Queue)\n";
        cout << " 0.  Exit\n";
        cout << "========================================\n";
        cout << "Enter your choice (0-18): ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "\n[!] Invalid input! Please enter a number between 0 and 18.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            library.addBook();
            break;
        case 2:
            library.displayAllBooks();
            break;
        case 3:
        {
            cout << "\n--- SEARCH OPTIONS ---\n";
            cout << "1. Search by Title\n";
            cout << "2. Search by Author\n";
            cout << "3. Search by Category\n";
            cout << "Enter search type (1-3): ";
            int st;
            if (cin >> st)
            {
                if (st == 1)
                    library.searchByTitle();
                else if (st == 2)
                    library.searchByAuthor();
                else if (st == 3)
                    library.searchByCategory();
                else
                    cout << "[!] Invalid choice.\n";
            }
            else
            {
                cin.clear();
                cin.ignore(1000, '\n');
            }
            break;
        }
        case 4:
            library.updateBook();
            break;
        case 5:
            library.deleteBook();
            break;
        case 6:
            library.addMember();
            break;
        case 7:
            library.displayMembers();
            break;
        case 8:
            library.issueBook();
            break;
        case 9:
            library.returnBook();
            break;
        case 10:
            library.displayIssuedList();
            break;
        case 11:
            library.reserveBook();
            break;
        case 12:
            library.displayWaitingList();
            break;
        case 13:
        {
            cout << "\n--- RECENTLY RETURNED BOOKS (STACK) ---\n";
            cout << "1. View All Returned Books in Stack\n";
            cout << "2. Peek Top of Stack (Latest Returned)\n";
            cout << "Enter choice: ";
            int sc;
            if (cin >> sc)
            {
                if (sc == 1)
                    library.displayReturnStack();
                else if (sc == 2)
                    library.peekReturnStack();
                else
                    cout << "[!] Invalid choice.\n";
            }
            else
            {
                cin.clear();
                cin.ignore(1000, '\n');
            }
            break;
        }
        case 14:
            library.searchBST();
            break;
        case 15:
        {
            cout << "\n--- BINARY SEARCH TREE TRAVERSALS ---\n";
            cout << "1. Inorder Traversal   (Sorted by Book ID)\n";
            cout << "2. Preorder Traversal  (Root -> Left -> Right)\n";
            cout << "3. Postorder Traversal (Left -> Right -> Root)\n";
            cout << "Enter choice (1-3): ";
            int tc;
            if (cin >> tc)
            {
                if (tc == 1)
                    library.showBSTInorder();
                else if (tc == 2)
                    library.showBSTPreorder();
                else if (tc == 3)
                    library.showBSTPostorder();
                else
                    cout << "[!] Invalid choice.\n";
            }
            else
            {
                cin.clear();
                cin.ignore(1000, '\n');
            }
            break;
        }
        case 16:
        {
            cout << "\n--- SORTING OPTIONS (Selection Sort) ---\n";
            cout << "1. Sort by Book ID\n";
            cout << "2. Sort by Book Title\n";
            cout << "3. Sort by Author Name\n";
            cout << "Enter choice (1-3): ";
            int srt;
            if (cin >> srt)
            {
                if (srt == 1)
                    library.sortBooksById();
                else if (srt == 2)
                    library.sortBooksByTitle();
                else if (srt == 3)
                    library.sortBooksByAuthor();
                else
                    cout << "[!] Invalid choice.\n";
            }
            else
            {
                cin.clear();
                cin.ignore(1000, '\n');
            }
            break;
        }
        case 17:
        {
            cout << "\n--- SHELF MANAGEMENT (2D ARRAY) ---\n";
            cout << "1. Display 2D Shelf Matrix\n";
            cout << "2. Locate Book on Physical Shelf\n";
            cout << "Enter choice (1-2): ";
            int shc;
            if (cin >> shc)
            {
                if (shc == 1)
                    library.displayShelf();
                else if (shc == 2)
                    library.locateBookOnShelf();
                else
                    cout << "[!] Invalid choice.\n";
            }
            else
            {
                cin.clear();
                cin.ignore(1000, '\n');
            }
            break;
        }
        case 18:
            library.handleTokenQueue();
            break;
        case 0:
            cout << "\n======================================================\n";
            cout << "   Thank you for using Library Book Management System! \n";
            cout << "          All changes saved safely to files.          \n";
            cout << "======================================================\n";
            break;
        default:
            cout << "\n[!] Invalid choice! Please select an option from 0 to 18.\n";
            break;
        }
    } while (choice != 0);

    return 0;
}
