#include "filesystem.h"

//  Constructor
FileSystem::FileSystem()
    : m_root(nullptr)
    , m_totalSizeKB(0)
    , m_diskCapacityKB(1024 * 1024)   // 1 GB simulated
{
    m_root = new FileSystemNode("C:", nullptr);

    // seed demo content
    FileSystemNode* docs = addFolder(m_root, "Documents");
    FileSystemNode* pics = addFolder(m_root, "Pictures");
    FileSystemNode* proj = addFolder(m_root, "Projects");

    FileSystemNode* resume = addFile(docs, "resume",    "pdf",  210);
    FileSystemNode* notes  = addFile(docs, "notes",     "txt",   18);
    addFile(docs, "budget", "xlsx", 140);

    if (notes) {
        notes->content = "# My Notes\n\nDouble-click this file to edit.\n";
    }
    if (resume) {
        // PDF not text-editable
    }

    addFile(pics, "vacation",  "png",  870);
    addFile(pics, "profile",   "jpg",  320);

    FileSystemNode* qtProj = addFolder(proj, "QtApp");
    FileSystemNode* mainCpp = addFile(qtProj, "main",       "cpp",  55);
    FileSystemNode* mwCpp   = addFile(qtProj, "mainwindow", "cpp", 120);
    FileSystemNode* mwH     = addFile(qtProj, "mainwindow", "h",    45);

    if (mainCpp) mainCpp->content = "#include <QApplication>\n#include \"mainwindow.h\"\n\nint main(int argc, char* argv[])\n{\n    QApplication app(argc, argv);\n    MainWindow w;\n    w.show();\n    return app.exec();\n}\n";
    if (mwCpp)   mwCpp->content   = "#include \"mainwindow.h\"\n\nMainWindow::MainWindow(QWidget* parent)\n    : QMainWindow(parent)\n{\n    // setup UI here\n}\n";
    if (mwH)     mwH->content     = "#pragma once\n#include <QMainWindow>\n\nclass MainWindow : public QMainWindow {\n    Q_OBJECT\npublic:\n    explicit MainWindow(QWidget* parent = nullptr);\n};\n";

    Q_UNUSED(resume); // suppress warning
}

//  Destructor
FileSystem::~FileSystem()
{
    destroySubtree(m_root);
    while (!m_recycleBin.isEmpty()) {
        DeletedItem item = m_recycleBin.pop();
        destroySubtree(item.node);
    }
}

//  pathOf
QString FileSystem::pathOf(FileSystemNode* node) const
{
    if (!node)          return "";
    if (node == m_root) return m_root->name;

    // Walk up and collect parts
    // We store at most 64 ancestors (more than enough)
    const int MAX_DEPTH = 64;
    FileSystemNode* parts[MAX_DEPTH];
    int depth = 0;

    FileSystemNode* cur = node;
    while (cur && cur != m_root && depth < MAX_DEPTH) {
        parts[depth++] = cur;
        cur = cur->parent;
    }

    QString path = m_root->name;
    for (int i = depth - 1; i >= 0; --i) {
        path += "\\" + parts[i]->displayName();
    }
    return path;
}

//  addFolder
FileSystemNode* FileSystem::addFolder(FileSystemNode* parent, const QString& name)
{
    if (!parent || parent->isFile()) return nullptr;
    if (name.trimmed().isEmpty())    return nullptr;

    // duplicate check
    for (int i = 0; i < parent->children.size(); ++i) {
        FileSystemNode* ch = parent->children[i];
        if (ch->isFolder() &&
            ch->name.compare(name.trimmed(), Qt::CaseInsensitive) == 0)
            return nullptr;
    }

    FileSystemNode* node = new FileSystemNode(name.trimmed(), parent);
    parent->children.append(node);
    logAction(QString("Folder created: %1").arg(pathOf(node)));
    return node;
}

//  addFile
FileSystemNode* FileSystem::addFile(FileSystemNode* parent,
                                    const QString&  name,
                                    const QString&  ext,
                                    int             sizeKB)
{
    if (!parent || parent->isFile()) return nullptr;
    if (name.trimmed().isEmpty())    return nullptr;

    FileSystemNode* node = new FileSystemNode(
        name.trimmed(), ext.trimmed().toLower(), sizeKB, parent);
    parent->children.append(node);
    m_totalSizeKB += sizeKB;
    logAction(QString("File created: %1").arg(pathOf(node)));
    return node;
}

// ============================================================
//  deleteNode, push to Recycle-Bin Stack
// ============================================================
bool FileSystem::deleteNode(FileSystemNode* node)
{
    if (!node || node == m_root) return false;

    FileSystemNode* parent = node->parent;
    if (!parent) return false;

    int idx = parent->children.indexOf(node);
    if (idx < 0) return false;

    QString deletedPath = pathOf(node);

    parent->children.removeAt(idx);
    node->parent = nullptr;

    int freed = subtreeSizeKB(node);
    m_totalSizeKB -= freed;
    if (m_totalSizeKB < 0) m_totalSizeKB = 0;

    m_recycleBin.push(DeletedItem(node, parent, idx));
    logAction(QString("Deleted → Recycle Bin: %1").arg(deletedPath));
    return true;
}

// ============================================================
//  undoDelete, pop Stack, restore
// ============================================================
bool FileSystem::undoDelete()
{
    if (m_recycleBin.isEmpty()) return false;

    DeletedItem item = m_recycleBin.pop();

    FileSystemNode* parent = item.originalParent;
    int idx = item.originalIndex;
    if (idx > parent->children.size()) idx = parent->children.size();

    parent->children.insert(idx, item.node);
    item.node->parent = parent;
    m_totalSizeKB += subtreeSizeKB(item.node);

    logAction(QString("Restored from Recycle Bin: %1")
                  .arg(pathOf(item.node)));
    return true;
}

QString FileSystem::undoPreview() const
{
    if (m_recycleBin.isEmpty()) return "";
    return m_recycleBin.top().node->displayName();
}

// ============================================================
//  rename
// ============================================================
bool FileSystem::rename(FileSystemNode* node, const QString& newBaseName)
{
    if (!node || node == m_root)       return false;
    if (newBaseName.trimmed().isEmpty()) return false;

    QString oldName = node->displayName();
    node->name = newBaseName.trimmed();
    logAction(QString("Renamed: \"%1\"  →  \"%2\"")
                  .arg(oldName).arg(node->displayName()));
    return true;
}

// ============================================================
//  move
// ============================================================
bool FileSystem::move(FileSystemNode* node, FileSystemNode* newParent)
{
    if (!node || !newParent)             return false;
    if (node == m_root)                  return false;
    if (!newParent->isFolder())          return false;
    if (node->parent == newParent)       return false;
    if (isAncestor(node, newParent))     return false;

    QString srcPath = pathOf(node);

    node->parent->children.removeOne(node);
    node->parent = newParent;
    newParent->children.append(node);

    logAction(QString("Moved: %1  →  %2").arg(srcPath).arg(pathOf(node)));
    return true;
}

// ============================================================
//  copy
// ============================================================
FileSystemNode* FileSystem::copy(FileSystemNode* node, FileSystemNode* newParent)
{
    if (!node || !newParent || !newParent->isFolder()) return nullptr;
    if (isAncestor(node, newParent)) return nullptr;

    QString srcPath = pathOf(node);
    FileSystemNode* cloned = deepCopy(node, newParent);
    newParent->children.append(cloned);
    m_totalSizeKB += subtreeSizeKB(cloned);
    logAction(QString("Copied: %1  →  %2").arg(srcPath).arg(pathOf(cloned)));
    return cloned;
}

// ============================================================
//  search  (DFS)
// ============================================================
DynArray<FileSystemNode*> FileSystem::search(const QString& query) const
{
    DynArray<FileSystemNode*> results;
    if (query.trimmed().isEmpty()) return results;
    dfsSearch(m_root, query.trimmed(), results);

    return results;
}

void FileSystem::dfsSearch(FileSystemNode*            node,
                           const QString&             query,
                           DynArray<FileSystemNode*>& results) const
{
    if (!node) return;
    if (node != m_root) {
        if (node->displayName().contains(query, Qt::CaseInsensitive) ||
            node->name.contains(query, Qt::CaseInsensitive))
        {
            results.append(node);
        }
    }
    for (int i = 0; i < node->children.size(); ++i)
        dfsSearch(node->children[i], query, results);
}

// ============================================================
//  clearLog
// ============================================================
void FileSystem::clearLog()
{
    while (!m_log.isEmpty()) m_log.dequeue();
}

// ============================================================
//  Private helpers
// ============================================================

void FileSystem::logAction(const QString& msg)
{
    if (m_log.size() >= LOG_CAP) m_log.dequeue();   // drop oldest (FIFO cap)
    m_log.enqueue(ActivityEntry(msg));
}

void FileSystem::destroySubtree(FileSystemNode* node)
{
    if (!node) return;
    for (int i = 0; i < node->children.size(); ++i)
        destroySubtree(node->children[i]);
    node->children.clear();
    delete node;
}

FileSystemNode* FileSystem::deepCopy(FileSystemNode* src,
                                     FileSystemNode* newParent)
{
    FileSystemNode* clone;
    if (src->isFile()) {
        clone = new FileSystemNode(src->name, src->extension, src->sizeKB, newParent);
        clone->content = src->content;
    } else {
        clone = new FileSystemNode(src->name, newParent);
        for (int i = 0; i < src->children.size(); ++i) {
            FileSystemNode* childClone = deepCopy(src->children[i], clone);
            clone->children.append(childClone);
        }
    }
    return clone;
}

int FileSystem::subtreeSizeKB(FileSystemNode* node) const
{
    if (!node) return 0;
    if (node->isFile()) return node->sizeKB;
    int total = 0;
    for (int i = 0; i < node->children.size(); ++i)
        total += subtreeSizeKB(node->children[i]);
    return total;
}

bool FileSystem::isAncestor(FileSystemNode* anc, FileSystemNode* node) const
{
    FileSystemNode* cur = node->parent;
    while (cur) {
        if (cur == anc) return true;
        cur = cur->parent;
    }
    return false;
}