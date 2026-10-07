/**
 * ============================================================================
 * PROJECT TITLE: LIBRARY BOOK MANAGEMENT SYSTEM
 * LANGUAGE: C++
 * PURPOSE: College Data Structures Project & Viva
 * 
 * DATA STRUCTURES USED:
 *  1. ARRAY       - In-memory storage of book and member records
 *  2. BST         - Binary Search Tree for fast O(log n) Book ID search
 *  3. LINKED LIST - Singly Linked List to track currently issued books
 *  4. RECURSION   - Used in BST Insertion and BST Searching
 *  5. FILE I/O    - Permanent storage in books.txt, members.txt, issued_books.txt
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>

using namespace std;

// Maximum limits
const int MAX_BOOKS = 100;
const int MAX_MEMBERS = 50;

// Text files for permanent storage
const string BOOK_FILE = "books.txt";
const string MEMBER_FILE = "members.txt";
const string ISSUED_FILE = "issued_books.txt";

// ============================================================================
// HELPER FUNCTIONS: Screen Clear and Pause
// ============================================================================
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    string dummy;
    getline(cin, dummy);
}

// String trim helper
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// ============================================================================
// 1. DATA STRUCTURE: RECORD STRUCTURES (ARRAY ELEMENTS)
// ============================================================================
struct Book {
    int id;
    string title;
    string author;
    string category;
    int available; // 1 = Available, 0 = Issued
};

struct Member {
    string id;
    string name;
    string department;
};

// ============================================================================
// 2. DATA STRUCTURE: BINARY SEARCH TREE (BST) FOR FAST BOOK SEARCH
// ============================================================================
struct BSTNode {
    int id;
    string title;
    string author;
    int available;
    BSTNode* left;
    BSTNode* right;

    BSTNode(int bId, string t, string a, int av) {
        id = bId;
        title = t;
        author = a;
        available = av;
        left = nullptr;
        right = nullptr;
    }
};

class BookBST {
public:
    BSTNode* root;

    BookBST() {
        root = nullptr;
    }

    ~BookBST() {
        clear(root);
    }

    void clear(BSTNode* node) {
        if (node == nullptr) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    void reset() {
        clear(root);
        root = nullptr;
    }

    // Recursion: Insert book into BST
    BSTNode* insertRecursive(BSTNode* node, int id, string title, string author, int av) {
        if (node == nullptr) {
            return new BSTNode(id, title, author, av);
        }
        if (id < node->id) {
            node->left = insertRecursive(node->left, id, title, author, av);
        } else if (id > node->id) {
            node->right = insertRecursive(node->right, id, title, author, av);
        }
        return node;
    }

    void insert(int id, string title, string author, int av) {
        root = insertRecursive(root, id, title, author, av);
    }

    // Recursion: Search book by ID in O(log n)
    BSTNode* searchRecursive(BSTNode* node, int id) {
        if (node == nullptr || node->id == id) {
            return node;
        }
        if (id < node->id) {
            return searchRecursive(node->left, id);
        }
        return searchRecursive(node->right, id);
    }

    BSTNode* search(int id) {
        return searchRecursive(root, id);
    }
};

// ============================================================================
// 3. DATA STRUCTURE: SINGLY LINKED LIST FOR ISSUED BOOKS
// ============================================================================
struct IssueNode {
    int bookId;
    string memberId;
    string issueDate;
    IssueNode* next;

    IssueNode(int bId, string mId, string date) {
        bookId = bId;
        memberId = mId;
        issueDate = date;
        next = nullptr;
    }
};

class IssuedLinkedList {
public:
    IssueNode* head;

    IssuedLinkedList() {
        head = nullptr;
    }

    ~IssuedLinkedList() {
        while (head != nullptr) {
            IssueNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void insertIssue(int bId, string mId, string date) {
        IssueNode* newNode = new IssueNode(bId, mId, date);
        if (head == nullptr) {
            head = newNode;
        } else {
            IssueNode* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    bool removeIssue(int bId) {
        if (head == nullptr) return false;

        if (head->bookId == bId) {
            IssueNode* temp = head;
            head = head->next;
            delete temp;
            return true;
        }

        IssueNode* curr = head;
        while (curr->next != nullptr && curr->next->bookId != bId) {
            curr = curr->next;
        }

        if (curr->next != nullptr) {
            IssueNode* temp = curr->next;
            curr->next = temp->next;
            delete temp;
            return true;
        }
        return false;
    }

    bool isBookIssued(int bId) {
        IssueNode* curr = head;
        while (curr != nullptr) {
            if (curr->bookId == bId) return true;
            curr = curr->next;
        }
        return false;
    }

    void display() {
        if (head == nullptr) {
            cout << "\n[!] No books are currently issued.\n";
            return;
        }
        cout << "\n==============================================================\n";
        cout << "            LINKED LIST: CURRENTLY ISSUED BOOKS               \n";
        cout << "==============================================================\n";
        cout << left << setw(10) << "Book ID" << setw(15) << "Member ID" << setw(20) << "Issue Date" << "\n";
        cout << "--------------------------------------------------------------\n";
        IssueNode* curr = head;
        while (curr != nullptr) {
            cout << left << setw(10) << curr->bookId 
                 << setw(15) << curr->memberId 
                 << setw(20) << curr->issueDate << "\n";
            curr = curr->next;
        }
        cout << "==============================================================\n";
    }
};

// ============================================================================
// 4. MAIN LIBRARY SYSTEM CLASS
// ============================================================================
class LibrarySystem {
private:
    Book books[MAX_BOOKS];           // Array for Book records
    int bookCount;

    Member members[MAX_MEMBERS];     // Array for Member records
    int memberCount;

    BookBST bst;                     // BST for Fast ID Search
    IssuedLinkedList issuedList;     // Linked List for Issued Books

    void ensureFilesExist() {
        ifstream bIn(BOOK_FILE);
        if (!bIn.is_open()) {
            ofstream bOut(BOOK_FILE);
            bOut << "101|C++ Programming|Bjarne Stroustrup|Computer Science|1\n";
            bOut << "102|Data Structures|Mark Allen Weiss|Computer Science|1\n";
            bOut << "103|Database Systems|Raghu Ramakrishnan|Database|1\n";
            bOut << "104|Operating Systems|Abraham Silberschatz|Systems|1\n";
            bOut << "105|Computer Networks|Andrew S. Tanenbaum|Networking|1\n";
            bOut.close();
        } else {
            bIn.close();
        }

        ifstream mIn(MEMBER_FILE);
        if (!mIn.is_open()) {
            ofstream mOut(MEMBER_FILE);
            mOut << "M001|Rahul Sharma|Computer Science\n";
            mOut << "M002|Amit Verma|Information Technology\n";
            mOut << "M003|Pooja Patel|Electronics\n";
            mOut.close();
        } else {
            mIn.close();
        }

        ifstream iIn(ISSUED_FILE);
        if (!iIn.is_open()) {
            ofstream iOut(ISSUED_FILE);
            iOut.close();
        } else {
            iIn.close();
        }
    }

public:
    LibrarySystem() {
        bookCount = 0;
        memberCount = 0;
        ensureFilesExist();
        loadAllData();
    }

    // ========================================================================
    // FILE HANDLING: LOAD DATA
    // ========================================================================
    void loadAllData() {
        bookCount = 0;
        memberCount = 0;
        bst.reset();

        // 1. Read books.txt
        ifstream bFile(BOOK_FILE);
        if (bFile.is_open()) {
            string line;
            while (getline(bFile, line)) {
                line = trim(line);
                if (line.empty()) continue;

                stringstream ss(line);
                string idStr, title, author, cat, availStr;
                if (getline(ss, idStr, '|') &&
                    getline(ss, title, '|') &&
                    getline(ss, author, '|') &&
                    getline(ss, cat, '|') &&
                    getline(ss, availStr, '|')) {
                    
                    if (bookCount < MAX_BOOKS) {
                        books[bookCount].id = stoi(trim(idStr));
                        books[bookCount].title = trim(title);
                        books[bookCount].author = trim(author);
                        books[bookCount].category = trim(cat);
                        books[bookCount].available = stoi(trim(availStr));

                        // Also insert into BST
                        bst.insert(books[bookCount].id, books[bookCount].title, 
                                   books[bookCount].author, books[bookCount].available);

                        bookCount++;
                    }
                }
            }
            bFile.close();
        }

        // 2. Read members.txt
        ifstream mFile(MEMBER_FILE);
        if (mFile.is_open()) {
            string line;
            while (getline(mFile, line)) {
                line = trim(line);
                if (line.empty()) continue;

                stringstream ss(line);
                string mId, name, dept;
                if (getline(ss, mId, '|') &&
                    getline(ss, name, '|') &&
                    getline(ss, dept, '|')) {
                    if (memberCount < MAX_MEMBERS) {
                        members[memberCount].id = trim(mId);
                        members[memberCount].name = trim(name);
                        members[memberCount].department = trim(dept);
                        memberCount++;
                    }
                }
            }
            mFile.close();
        }

        // 3. Read issued_books.txt
        ifstream iFile(ISSUED_FILE);
        if (iFile.is_open()) {
            string line;
            while (getline(iFile, line)) {
                line = trim(line);
                if (line.empty()) continue;

                stringstream ss(line);
                string bIdStr, mId, date;
                if (getline(ss, bIdStr, '|') &&
                    getline(ss, mId, '|') &&
                    getline(ss, date, '|')) {
                    issuedList.insertIssue(stoi(trim(bIdStr)), trim(mId), trim(date));
                }
            }
            iFile.close();
        }
    }

    // ========================================================================
    // FILE HANDLING: SAVE DATA
    // ========================================================================
    void saveBooksToFile() {
        ofstream bFile(BOOK_FILE, ios::trunc);
        if (bFile.is_open()) {
            for (int i = 0; i < bookCount; i++) {
                bFile << books[i].id << "|"
                      << books[i].title << "|"
                      << books[i].author << "|"
                      << books[i].category << "|"
                      << books[i].available << "\n";
            }
            bFile.close();
        }
    }

    void saveMembersToFile() {
        ofstream mFile(MEMBER_FILE, ios::trunc);
        if (mFile.is_open()) {
            for (int i = 0; i < memberCount; i++) {
                mFile << members[i].id << "|"
                      << members[i].name << "|"
                      << members[i].department << "\n";
            }
            mFile.close();
        }
    }

    void saveIssuedToFile() {
        ofstream iFile(ISSUED_FILE, ios::trunc);
        if (iFile.is_open()) {
            IssueNode* curr = issuedList.head;
            while (curr != nullptr) {
                iFile << curr->bookId << "|"
                      << curr->memberId << "|"
                      << curr->issueDate << "\n";
                curr = curr->next;
            }
            iFile.close();
        }
    }

    void rebuildBST() {
        bst.reset();
        for (int i = 0; i < bookCount; i++) {
            bst.insert(books[i].id, books[i].title, books[i].author, books[i].available);
        }
    }

    bool memberExists(const string& mId) {
        for (int i = 0; i < memberCount; i++) {
            if (members[i].id == mId) return true;
        }
        return false;
    }

    // ========================================================================
    // 1. ADD NEW BOOK
    // ========================================================================
    void addBook() {
        cout << "========================================\n";
        cout << "            1. ADD NEW BOOK             \n";
        cout << "========================================\n";

        if (bookCount >= MAX_BOOKS) {
            cout << "[!] Library storage is full!\n";
            return;
        }

        int id;
        cout << "Enter Book ID (number): ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid Book ID entered.\n";
            return;
        }
        cin.ignore(1000, '\n');

        // Check for duplicate ID
        for (int i = 0; i < bookCount; i++) {
            if (books[i].id == id) {
                cout << "[!] Error: Book ID " << id << " already exists!\n";
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
        bookCount++;

        saveBooksToFile();
        cout << "\n[+] Success: Book added successfully!\n";
    }

    // ========================================================================
    // 2. DISPLAY ALL BOOKS
    // ========================================================================
    void displayAllBooks() {
        cout << "===========================================================================================\n";
        cout << "                                2. DISPLAY ALL BOOKS                                      \n";
        cout << "===========================================================================================\n";

        if (bookCount == 0) {
            cout << "[!] No books found in the library.\n";
            return;
        }

        cout << left << setw(8) << "ID"
             << setw(32) << "Title"
             << setw(22) << "Author"
             << setw(18) << "Category"
             << setw(12) << "Status" << "\n";
        cout << "-------------------------------------------------------------------------------------------\n";

        for (int i = 0; i < bookCount; i++) {
            cout << left << setw(8) << books[i].id
                 << setw(32) << books[i].title.substr(0, 30)
                 << setw(22) << books[i].author.substr(0, 20)
                 << setw(18) << books[i].category.substr(0, 16)
                 << setw(12) << (books[i].available == 1 ? "Available" : "Issued")
                 << "\n";
        }
        cout << "===========================================================================================\n";
    }

    // ========================================================================
    // 3. SEARCH BOOK (USING BST)
    // ========================================================================
    void searchBookBST() {
        cout << "========================================\n";
        cout << "       3. SEARCH BOOK (USING BST)       \n";
        cout << "========================================\n";

        int id;
        cout << "Enter Book ID to search: ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid Book ID entered.\n";
            return;
        }
        cin.ignore(1000, '\n');

        // Search in Binary Search Tree in O(log n) time
        BSTNode* node = bst.search(id);

        if (node != nullptr) {
            cout << "\n[+] Book found in Binary Search Tree (BST):\n";
            cout << "----------------------------------------\n";
            cout << "Book ID:   " << node->id << "\n";
            cout << "Title:     " << node->title << "\n";
            cout << "Author:    " << node->author << "\n";
            cout << "Status:    " << (node->available == 1 ? "Available" : "Issued") << "\n";
            cout << "----------------------------------------\n";
        } else {
            cout << "\n[!] Book ID " << id << " NOT found in the library BST.\n";
        }
    }

    // ========================================================================
    // 4. UPDATE BOOK DETAILS
    // ========================================================================
    void updateBook() {
        cout << "========================================\n";
        cout << "         4. UPDATE BOOK DETAILS         \n";
        cout << "========================================\n";

        int id;
        cout << "Enter Book ID to update: ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid ID entered.\n";
            return;
        }
        cin.ignore(1000, '\n');

        int index = -1;
        for (int i = 0; i < bookCount; i++) {
            if (books[i].id == id) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            cout << "[!] Book with ID " << id << " not found.\n";
            return;
        }

        cout << "\nCurrent Details:\n";
        cout << "Title:    " << books[index].title << "\n";
        cout << "Author:   " << books[index].author << "\n";
        cout << "Category: " << books[index].category << "\n";

        string title, author, category;
        cout << "\nEnter New Title (Press Enter to keep current): ";
        getline(cin, title);
        cout << "Enter New Author (Press Enter to keep current): ";
        getline(cin, author);
        cout << "Enter New Category (Press Enter to keep current): ";
        getline(cin, category);

        if (!trim(title).empty()) books[index].title = trim(title);
        if (!trim(author).empty()) books[index].author = trim(author);
        if (!trim(category).empty()) books[index].category = trim(category);

        rebuildBST();
        saveBooksToFile();
        cout << "\n[+] Success: Book details updated successfully!\n";
    }

    // ========================================================================
    // 5. DELETE BOOK
    // ========================================================================
    void deleteBook() {
        cout << "========================================\n";
        cout << "             5. DELETE BOOK             \n";
        cout << "========================================\n";

        int id;
        cout << "Enter Book ID to delete: ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid ID entered.\n";
            return;
        }
        cin.ignore(1000, '\n');

        // Check if currently issued
        if (issuedList.isBookIssued(id)) {
            cout << "[!] Error: Book ID " << id << " is currently ISSUED! Return it before deleting.\n";
            return;
        }

        int index = -1;
        for (int i = 0; i < bookCount; i++) {
            if (books[i].id == id) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            cout << "[!] Book with ID " << id << " not found.\n";
            return;
        }

        // Shift elements to delete from array
        for (int i = index; i < bookCount - 1; i++) {
            books[i] = books[i + 1];
        }
        bookCount--;

        rebuildBST();
        saveBooksToFile();
        cout << "\n[+] Success: Book ID " << id << " deleted from library catalog!\n";
    }

    // ========================================================================
    // 6. ADD MEMBER
    // ========================================================================
    void addMember() {
        cout << "========================================\n";
        cout << "             6. ADD MEMBER              \n";
        cout << "========================================\n";

        if (memberCount >= MAX_MEMBERS) {
            cout << "[!] Member capacity reached.\n";
            return;
        }

        string id, name, dept;
        cout << "Enter Member ID (e.g. M004): ";
        getline(cin, id);
        id = trim(id);

        if (memberExists(id)) {
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

    // ========================================================================
    // 7. DISPLAY MEMBERS
    // ========================================================================
    void displayMembers() {
        cout << "==============================================================\n";
        cout << "                    7. DISPLAY MEMBERS                        \n";
        cout << "==============================================================\n";

        if (memberCount == 0) {
            cout << "[!] No members registered yet.\n";
            return;
        }

        cout << left << setw(12) << "Member ID" << setw(25) << "Name" << setw(25) << "Department" << "\n";
        cout << "--------------------------------------------------------------\n";
        for (int i = 0; i < memberCount; i++) {
            cout << left << setw(12) << members[i].id 
                 << setw(25) << members[i].name 
                 << setw(25) << members[i].department << "\n";
        }
        cout << "==============================================================\n";
    }

    // ========================================================================
    // 8. ISSUE BOOK
    // ========================================================================
    void issueBook() {
        cout << "========================================\n";
        cout << "             8. ISSUE BOOK              \n";
        cout << "========================================\n";

        int bId;
        cout << "Enter Book ID to issue: ";
        if (!(cin >> bId)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid Book ID.\n";
            return;
        }
        cin.ignore(1000, '\n');

        int index = -1;
        for (int i = 0; i < bookCount; i++) {
            if (books[i].id == bId) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            cout << "[!] Error: Book ID " << bId << " not found in library.\n";
            return;
        }

        if (books[index].available == 0) {
            cout << "[!] Notice: Book \"" << books[index].title << "\" is ALREADY ISSUED to someone else.\n";
            return;
        }

        string mId;
        cout << "Enter Member ID: ";
        getline(cin, mId);
        mId = trim(mId);

        if (!memberExists(mId)) {
            cout << "[!] Error: Member ID \"" << mId << "\" is not registered.\n";
            return;
        }

        string issueDate;
        cout << "Enter Issue Date (DD-MM-YYYY): ";
        getline(cin, issueDate);

        // Mark as issued
        books[index].available = 0;

        // Add to Singly Linked List
        issuedList.insertIssue(bId, mId, trim(issueDate));

        rebuildBST();
        saveBooksToFile();
        saveIssuedToFile();

        cout << "\n[+] Success: Book ID " << bId << " issued to Member " << mId << "!\n";
    }

    // ========================================================================
    // 9. RETURN BOOK
    // ========================================================================
    void returnBook() {
        cout << "========================================\n";
        cout << "             9. RETURN BOOK             \n";
        cout << "========================================\n";

        int bId;
        cout << "Enter Book ID to return: ";
        if (!(cin >> bId)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[!] Invalid Book ID.\n";
            return;
        }
        cin.ignore(1000, '\n');

        int index = -1;
        for (int i = 0; i < bookCount; i++) {
            if (books[i].id == bId) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            cout << "[!] Error: Book ID " << bId << " does not exist in catalog.\n";
            return;
        }

        // Remove from Linked List
        bool removed = issuedList.removeIssue(bId);
        if (!removed) {
            cout << "[!] Notice: This book is NOT currently marked as issued.\n";
            return;
        }

        // Mark available again
        books[index].available = 1;

        rebuildBST();
        saveBooksToFile();
        saveIssuedToFile();

        cout << "\n[+] Success: Book ID " << bId << " returned successfully and is now Available!\n";
    }
};

// ============================================================================
// MAIN FUNCTION & MENU CONTROLLER
// ============================================================================
int main() {
    LibrarySystem library;
    int choice;

    do {
        clearScreen(); // Clears screen before showing menu

        cout << "========================================\n";
        cout << "       LIBRARY BOOK MANAGEMENT          \n";
        cout << "========================================\n";
        cout << " 1.  Add New Book\n";
        cout << " 2.  Display All Books\n";
        cout << " 3.  Search Book (using BST)\n";
        cout << " 4.  Update Book Details\n";
        cout << " 5.  Delete Book\n";
        cout << " 6.  Add Member\n";
        cout << " 7.  Display Members\n";
        cout << " 8.  Issue Book\n";
        cout << " 9.  Return Book\n";
        cout << " 0.  Exit\n";
        cout << "========================================\n";
        cout << "Enter your choice (0-9): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        cin.ignore(1000, '\n'); // Clean buffer

        // Clear screen immediately after selecting any option
        clearScreen();

        switch (choice) {
            case 1:
                library.addBook();
                pauseScreen();
                break;
            case 2:
                library.displayAllBooks();
                pauseScreen();
                break;
            case 3:
                library.searchBookBST();
                pauseScreen();
                break;
            case 4:
                library.updateBook();
                pauseScreen();
                break;
            case 5:
                library.deleteBook();
                pauseScreen();
                break;
            case 6:
                library.addMember();
                pauseScreen();
                break;
            case 7:
                library.displayMembers();
                pauseScreen();
                break;
            case 8:
                library.issueBook();
                pauseScreen();
                break;
            case 9:
                library.returnBook();
                pauseScreen();
                break;
            case 0:
                // Only this exit message will show in terminal upon exit
                cout << "======================================================\n";
                cout << "   Thank you for using Library Book Management System! \n";
                cout << "            All data saved safely to files.            \n";
                cout << "======================================================\n";
                break;
            default:
                cout << "[!] Invalid choice! Please select an option between 0 and 9.\n";
                pauseScreen();
                break;
        }
    } while (choice != 0);

    return 0;
}
