#ifndef NODE_H
#define NODE_H

// ============================================================
//  node.h  —  All core data types for the File System Simulator
//
//  DSA structures implemented from scratch (NO QList / QStack /
//  QQueue / QVector / QMap — raw arrays + linked lists only).
// ============================================================

#include <QString>
#include <QDateTime>

//  Forward declaration
struct FileSystemNode;

// ============================================================
//  1.  CUSTOM DYNAMIC ARRAY  (replaces QList / QVector)
//      Used for children list of every node.
// ============================================================
template<typename T>
class DynArray {
public:
    DynArray() : m_data(nullptr), m_size(0), m_cap(0) {}

    ~DynArray() { delete[] m_data; }

    // deep copy
    DynArray(const DynArray& o) : m_data(nullptr), m_size(0), m_cap(0) {
        for (int i = 0; i < o.m_size; ++i) append(o.m_data[i]);
    }
    DynArray& operator=(const DynArray& o) {
        if (this == &o) return *this;
        delete[] m_data; m_data = nullptr; m_size = 0; m_cap = 0;
        for (int i = 0; i < o.m_size; ++i) append(o.m_data[i]);
        return *this;
    }

    void append(const T& val) {
        if (m_size == m_cap) grow();
        m_data[m_size++] = val;
    }

    bool removeOne(const T& val) {
        for (int i = 0; i < m_size; ++i) {
            if (m_data[i] == val) { removeAt(i); return true; }
        }
        return false;
    }

    void removeAt(int idx) {
        if (idx < 0 || idx >= m_size) return;
        for (int i = idx; i < m_size - 1; ++i)
            m_data[i] = m_data[i + 1];
        --m_size;
    }

    void insert(int idx, const T& val) {
        if (m_size == m_cap) grow();
        for (int i = m_size; i > idx; --i)
            m_data[i] = m_data[i - 1];
        m_data[idx] = val;
        ++m_size;
    }

    int indexOf(const T& val) const {
        for (int i = 0; i < m_size; ++i)
            if (m_data[i] == val) return i;
        return -1;
    }

    void clear() { m_size = 0; }

    T&       operator[](int i)       { return m_data[i]; }
    const T& operator[](int i) const { return m_data[i]; }
    int      size()             const { return m_size; }
    bool     isEmpty()          const { return m_size == 0; }

    // iterator support (range-based for)
    T* begin() { return m_data; }
    T* end()   { return m_data + m_size; }
    const T* begin() const { return m_data; }
    const T* end()   const { return m_data + m_size; }

private:
    T*  m_data;
    int m_size;
    int m_cap;

    void grow() {
        int newCap = (m_cap == 0) ? 4 : m_cap * 2;
        T* newData = new T[newCap];
        for (int i = 0; i < m_size; ++i) newData[i] = m_data[i];
        delete[] m_data;
        m_data = newData;
        m_cap  = newCap;
    }
};

// ============================================================
//  2.  CUSTOM STACK  (replaces QStack)
//      Linked-list based, LIFO — used for Recycle Bin.
// ============================================================
template<typename T>
class Stack {
    struct Node { T data; Node* next; };
public:
    Stack() : m_top(nullptr), m_size(0) {}
    ~Stack() { while (!isEmpty()) pop(); }

    void push(const T& val) {
        Node* n = new Node{val, m_top};
        m_top = n;
        ++m_size;
    }

    T pop() {
        T val = m_top->data;
        Node* old = m_top;
        m_top = m_top->next;
        delete old;
        --m_size;
        return val;
    }

    const T& top() const { return m_top->data; }
    bool     isEmpty()   const { return m_size == 0; }
    int      size()      const { return m_size; }

private:
    Node* m_top;
    int   m_size;
};

// ============================================================
//  3.  CUSTOM QUEUE  (replaces QQueue)
//      Linked-list based, FIFO — used for Activity Log.
// ============================================================
template<typename T>
class Queue {
    struct Node { T data; Node* next; };
public:
    Queue() : m_head(nullptr), m_tail(nullptr), m_size(0) {}
    ~Queue() { while (!isEmpty()) dequeue(); }

    void enqueue(const T& val) {
        Node* n = new Node{val, nullptr};
        if (!m_tail) { m_head = m_tail = n; }
        else         { m_tail->next = n; m_tail = n; }
        ++m_size;
    }

    T dequeue() {
        T val = m_head->data;
        Node* old = m_head;
        m_head = m_head->next;
        if (!m_head) m_tail = nullptr;
        delete old;
        --m_size;
        return val;
    }

    const T& front() const { return m_head->data; }
    bool     isEmpty()     const { return m_size == 0; }
    int      size()        const { return m_size; }

    // indexed access (for log display)
    T& at(int idx) {
        Node* cur = m_head;
        for (int i = 0; i < idx; ++i) cur = cur->next;
        return cur->data;
    }
    const T& at(int idx) const {
        Node* cur = m_head;
        for (int i = 0; i < idx; ++i) cur = cur->next;
        return cur->data;
    }

private:
    Node* m_head;
    Node* m_tail;
    int   m_size;
};

// ============================================================
//  4.  NodeType
// ============================================================
enum class NodeType { File, Folder };

// ============================================================
//  5.  FileSystemNode  —  one node in the Tree
// ============================================================
struct FileSystemNode {

    QString   name;
    NodeType  type;

    // file-only
    QString   extension;
    int       sizeKB;
    QString   content;       // editable text content

    // common
    QDateTime createdAt;

    // tree links
    FileSystemNode*           parent;
    DynArray<FileSystemNode*> children;   // ← uses our custom DynArray

    // ── Folder constructor ──
    FileSystemNode(const QString& folderName,
                   FileSystemNode* parentNode = nullptr)
        : name(folderName)
        , type(NodeType::Folder)
        , sizeKB(0)
        , createdAt(QDateTime::currentDateTime())
        , parent(parentNode)
    {}

    // ── File constructor ──
    FileSystemNode(const QString& fileName,
                   const QString& ext,
                   int            size,
                   FileSystemNode* parentNode = nullptr)
        : name(fileName)
        , type(NodeType::File)
        , extension(ext.toLower())
        , sizeKB(size)
        , createdAt(QDateTime::currentDateTime())
        , parent(parentNode)
    {}

    bool isFolder() const { return type == NodeType::Folder; }
    bool isFile()   const { return type == NodeType::File;   }

    // Is this a text-editable file type?
    bool isTextFile() const {
        static const char* textExts[] = {
            "txt","cpp","h","c","py","java","js","ts",
            "html","css","xml","json","md","ini","cfg",
            "bat","sh","sql","csv","log","hpp","cc", nullptr
        };
        QString ext = extension.toLower();
        for (int i = 0; textExts[i]; ++i)
            if (ext == textExts[i]) return true;
        return false;
    }

    bool isImageFile() const {
        static const char* imgExts[] = {
            "png","jpg","jpeg","bmp","gif","ico","svg",
            "tiff","webp","raw", nullptr
        };
        QString ext = extension.toLower();
        for (int i = 0; imgExts[i]; ++i)
            if (ext == imgExts[i]) return true;
        return false;
    }

    QString displayName() const {
        if (isFile() && !extension.isEmpty())
            return name + "." + extension;
        return name;
    }

    QString metaString() const {
        QString ts = createdAt.toString("dd-MMM-yyyy  hh:mm AP");
        if (isFile())
            return QString("%1  |  %2 KB  |  Created: %3")
                .arg(displayName()).arg(sizeKB).arg(ts);
        return QString("[Folder]  %1  |  Created: %2").arg(name).arg(ts);
    }
};

// ============================================================
//  6.  DeletedItem  —  what the Recycle-Bin Stack stores
// ============================================================
struct DeletedItem {
    FileSystemNode* node;
    FileSystemNode* originalParent;
    int             originalIndex;

    DeletedItem()
        : node(nullptr), originalParent(nullptr), originalIndex(0) {}

    DeletedItem(FileSystemNode* n, FileSystemNode* p, int idx)
        : node(n), originalParent(p), originalIndex(idx) {}
};

// ============================================================
//  7.  ActivityEntry  —  one entry in the Activity-Log Queue
// ============================================================
struct ActivityEntry {
    QString   message;
    QDateTime timestamp;

    ActivityEntry() {}

    ActivityEntry(const QString& msg)
        : message(msg)
        , timestamp(QDateTime::currentDateTime())
    {}

    QString formatted() const {
        return QString("[%1]  %2")
        .arg(timestamp.toString("hh:mm:ss AP"))
            .arg(message);
    }
};

#endif // NODE_H