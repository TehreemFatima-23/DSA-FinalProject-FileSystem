#ifndef FILESYSTEM_H
#define FILESYSTEM_H

// ============================================================
//  filesystem.h  —  FileSystem engine
//
//  Uses ONLY our custom DSA:
//    • DynArray   → children lists
//    • Stack      → Recycle Bin (LIFO)
//    • Queue      → Activity Log (FIFO)
//  NO Qt containers anywhere.
// ============================================================

#include "node.h"
#include <QString>

class FileSystem {
public:
    FileSystem();
    ~FileSystem();

    // tree accessors
    FileSystemNode* root()            const { return m_root; }
    int  totalSizeKB()                const { return m_totalSizeKB; }
    int& totalSizeKBRef()                   { return m_totalSizeKB; }  // writable ref for editor save
    int  diskCapacityKB()             const { return m_diskCapacityKB; }

    // path
    QString pathOf(FileSystemNode* node) const;

    / create
    FileSystemNode* addFolder(FileSystemNode* parent, const QString& name);
    FileSystemNode* addFile  (FileSystemNode* parent,
                            const QString& name,
                            const QString& ext,
                            int sizeKB);

    // delete using Stack (Recycle Bin)
    bool deleteNode(FileSystemNode* node);

    // undo, pop Stack
    bool    undoDelete();
    bool    canUndo()      const { return !m_recycleBin.isEmpty(); }
    QString undoPreview()  const;

    // rename
    bool rename(FileSystemNode* node, const QString& newBaseName);

    // move
    bool move(FileSystemNode* node, FileSystemNode* newParent);

    // copy (deep)
    FileSystemNode* copy(FileSystemNode* node, FileSystemNode* newParent);

    // search  (DFS, returns raw array of results)
    // Caller owns the returned DynArray (stack-allocated, no heap issue)
    DynArray<FileSystemNode*> search(const QString& query) const;

    // activity log
    const Queue<ActivityEntry>& activityLog() const { return m_log; }
    void clearLog();

private:
    FileSystemNode* m_root;
    int             m_totalSizeKB;
    int             m_diskCapacityKB;

    // DSA data structures
    Stack<DeletedItem>   m_recycleBin;   // LIFO recycle bin
    Queue<ActivityEntry> m_log;          // FIFO activity log
    static constexpr int LOG_CAP = 300;

    // helpers
    void            logAction(const QString& msg);
    void            destroySubtree(FileSystemNode* node);
    FileSystemNode* deepCopy(FileSystemNode* src, FileSystemNode* newParent);
    int             subtreeSizeKB(FileSystemNode* node) const;
    bool            isAncestor(FileSystemNode* anc, FileSystemNode* node) const;
    void            dfsSearch(FileSystemNode*          node,
                   const QString&            query,
                   DynArray<FileSystemNode*>& results) const;
};

#endif // FILESYSTEM_H