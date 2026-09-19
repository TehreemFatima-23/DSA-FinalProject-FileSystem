#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// ============================================================
//  mainwindow.h, Qt Widgets GUI (no .ui file)
// ============================================================

#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QSpinBox>
#include <QProgressBar>
#include <QGroupBox>
#include <QSplitter>
#include <QScrollArea>
#include <QMenu>
#include <QDialog>

#include "filesystem.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // toolbar / button actions
    void onAddFolder();
    void onAddFile();
    void onDelete();
    void onUndoDelete();
    void onRename();
    void onSearch();
    void onMove();
    void onCopy();
    void onClearLog();

    // ── tree ──────────────────────────────────────────────
    void onTreeSelectionChanged();
    void onTreeItemDoubleClicked(QTreeWidgetItem* item, int col);
    void onTreeContextMenu(const QPoint& pos);

private:
    // ── data model ───────────────────────────────────────
    FileSystem m_fs;

    // ── build UI ─────────────────────────────────────────
    void    buildUI();
    QWidget* buildControlPanel();
    QWidget* buildLogPanel();

    // ── tree ─────────────────────────────────────────────
    void            refreshTree();
    void            populateItem(QTreeWidgetItem* parentItem,
                      FileSystemNode*  node);
    FileSystemNode* nodeFromItem(QTreeWidgetItem* item) const;
    FileSystemNode* selectedNode() const;

    // ── file viewer / editor ─────────────────────────────
    void openFileViewer(FileSystemNode* file);

    // ── log / disk ───────────────────────────────────────
    void refreshLog();
    void updateDiskUsage();

    // ── style helper ─────────────────────────────────────
    QString btnStyle(const QString& hexColor) const;

    // ── widgets ──────────────────────────────────────────
    QTreeWidget*  m_tree;

    // Input section
    QLineEdit*    m_nameEdit;
    QLineEdit*    m_extEdit;
    QSpinBox*     m_sizeSpin;

    // Search section (own widget — NOT m_nameEdit)
    QLineEdit*    m_searchEdit;

    // Log
    QTextEdit*    m_logEdit;

    // Status
    QLabel*       m_statusLabel;

    // Disk
    QLabel*       m_diskLabel;
    QProgressBar* m_diskBar;

    // Undo button (needs enable/disable from multiple places)
    QPushButton*  m_undoBtn;
};

#endif // MAINWINDOW_H
