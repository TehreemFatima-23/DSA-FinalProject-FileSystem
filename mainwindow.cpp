#include "mainwindow.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QLabel>
#include <QScrollBar>
#include <QStatusBar>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QSizePolicy>
#include <QKeySequence>
#include <QShortcut>

//  Constructor
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("File System Simulator  —  DSA Semester Project");
    setMinimumSize(1150, 700);
    resize(1300, 780);

    buildUI();
    refreshTree();
    refreshLog();
    updateDiskUsage();
}

MainWindow::~MainWindow() {}

//  buildUI  —  assembles the entire window
void MainWindow::buildUI()
{
    // global dark theme
    qApp->setStyle("Fusion");
    qApp->setStyleSheet(R"(
        QWidget {
            background-color: #1e1e2e;
            color: #cdd6f4;
            font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif;
            font-size: 13px;
        }
        QMainWindow { background-color: #1e1e2e; }

        /* ── GroupBox ── */
        /* Each QGroupBox uses setStyleSheet() directly for title-safe margins.
           Only set background here so child widgets inherit the dark theme. */
        QGroupBox { background-color: #1e1e2e; }

        /* ── Tree ── */
        QTreeWidget {
            background-color: #181825;
            alternate-background-color: #1e1e2e;
            border: 1px solid #45475a;
            border-radius: 5px;
            color: #cdd6f4;
            outline: none;
        }
        QTreeWidget::item {
            height: 26px;
            padding-left: 2px;
        }
        QTreeWidget::item:selected {
            background-color: #313244;
            color: #89b4fa;
            border-left: 3px solid #89b4fa;
        }
        QTreeWidget::item:hover:!selected {
            background-color: #242433;
        }
        QHeaderView::section {
            background-color: #181825;
            color: #6c7086;
            border: none;
            border-bottom: 1px solid #45475a;
            padding: 4px 8px;
            font-weight: bold;
            font-size: 11px;
        }

        /* ── Inputs ── */
        QLineEdit, QSpinBox {
            background-color: #181825;
            border: 1px solid #45475a;
            border-radius: 5px;
            padding: 5px 10px;
            color: #cdd6f4;
            min-height: 30px;
            selection-background-color: #45475a;
        }
        QLineEdit:focus, QSpinBox:focus {
            border: 1px solid #89b4fa;
            background-color: #1e1e3e;
        }
        QLineEdit::placeholder { color: #6c7086; }
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: #313244;
            border: none;
            width: 18px;
        }
        QSpinBox::up-button:hover, QSpinBox::down-button:hover {
            background-color: #45475a;
        }

        /* ── TextEdit (log) ── */
        QTextEdit {
            background-color: #11111b;
            border: 1px solid #313244;
            border-radius: 5px;
            color: #a6e3a1;
            font-family: "Cascadia Code", "Consolas", "Courier New", monospace;
            font-size: 12px;
        }

        /* ── Progress bar ── */
        QProgressBar {
            border: 1px solid #45475a;
            border-radius: 5px;
            background-color: #181825;
            text-align: center;
            color: #cdd6f4;
            font-size: 11px;
            min-height: 20px;
        }
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #89b4fa, stop:1 #cba6f7);
            border-radius: 4px;
        }

        /* ── Scrollbar ── */
        QScrollBar:vertical {
            background: #181825;
            width: 7px;
            border-radius: 4px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: #45475a;
            border-radius: 4px;
            min-height: 24px;
        }
        QScrollBar::handle:vertical:hover { background: #585b70; }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical { height: 0; }

        /* ── Context menu ── */
        QMenu {
            background-color: #181825;
            border: 1px solid #45475a;
            border-radius: 5px;
            padding: 4px;
            color: #cdd6f4;
        }
        QMenu::item {
            padding: 6px 24px 6px 12px;
            border-radius: 3px;
        }
        QMenu::item:selected { background-color: #313244; color: #89b4fa; }
        QMenu::separator {
            height: 1px;
            background: #45475a;
            margin: 3px 8px;
        }

        /* ── Dialog ── */
        QDialog {
            background-color: #1e1e2e;
        }
        QDialogButtonBox QPushButton {
            min-width: 80px;
            min-height: 30px;
        }

        /* ── Status bar ── */
        QStatusBar {
            background-color: #181825;
            color: #6c7086;
            border-top: 1px solid #313244;
            font-size: 11px;
        }

        /* ── Splitter handle ── */
        QSplitter::handle { background-color: #313244; }
    )");

    // Central splitter
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(3);

    // ══════════════════════════════════════════════════════
    //  LEFT PANEL  —  Tree Explorer
    // ══════════════════════════════════════════════════════
    QWidget*     leftPanel = new QWidget;
    QVBoxLayout* leftVL    = new QVBoxLayout(leftPanel);
    leftVL->setContentsMargins(10, 10, 5, 10);
    leftVL->setSpacing(8);

    // Header label
    QLabel* explorerLabel = new QLabel("  📁  File System Explorer");
    explorerLabel->setStyleSheet(
        "font-size:15px; font-weight:bold; color:#89b4fa;"
        "padding:6px 4px; border-bottom:1px solid #313244;");
    leftVL->addWidget(explorerLabel);

    // Tree widget
    m_tree = new QTreeWidget;
    m_tree->setColumnCount(3);
    m_tree->setHeaderLabels({"  Name", "Size", "Created"});
    m_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tree->header()->setSectionResizeMode(1, QHeaderView::Fixed);
    m_tree->header()->setSectionResizeMode(2, QHeaderView::Fixed);
    m_tree->header()->resizeSection(1, 80);
    m_tree->header()->resizeSection(2, 130);
    m_tree->setAlternatingRowColors(true);
    m_tree->setAnimated(true);
    m_tree->setExpandsOnDoubleClick(false);
    m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->setIndentation(18);
    leftVL->addWidget(m_tree);

    connect(m_tree, &QTreeWidget::itemSelectionChanged,
            this,   &MainWindow::onTreeSelectionChanged);
    connect(m_tree, &QTreeWidget::itemDoubleClicked,
            this,   &MainWindow::onTreeItemDoubleClicked);
    connect(m_tree, &QTreeWidget::customContextMenuRequested,
            this,   &MainWindow::onTreeContextMenu);

    // ══════════════════════════════════════════════════════
    //  RIGHT PANEL  —  Controls (scrollable) + Log (fixed bottom)
    // ══════════════════════════════════════════════════════
    QWidget*     rightPanel = new QWidget;
    rightPanel->setMinimumWidth(360);
    QVBoxLayout* rightVL    = new QVBoxLayout(rightPanel);
    rightVL->setContentsMargins(6, 8, 10, 8);
    rightVL->setSpacing(6);

    // Control panel wrapped in a QScrollArea 
    // This ensures that when the window is short, controls
    // scroll instead of overlapping each other.
    QScrollArea* ctrlScroll = new QScrollArea;
    ctrlScroll->setWidgetResizable(true);
    ctrlScroll->setFrameShape(QFrame::NoFrame);
    ctrlScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ctrlScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ctrlScroll->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollArea > QWidget > QWidget { background: transparent; }");
    ctrlScroll->setWidget(buildControlPanel());

    // Log panel always visible at the bottom 
    QWidget* logWidget = buildLogPanel();
    logWidget->setMinimumHeight(180);

    // Splitter so user can resize control vs log areas
    QSplitter* rightSplit = new QSplitter(Qt::Vertical);
    rightSplit->setHandleWidth(4);
    rightSplit->setChildrenCollapsible(false);
    rightSplit->addWidget(ctrlScroll);
    rightSplit->addWidget(logWidget);
    // Control panel gets ~65% of the right column height
    rightSplit->setStretchFactor(0, 65);
    rightSplit->setStretchFactor(1, 35);

    rightVL->addWidget(rightSplit, 1);

    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 55);
    splitter->setStretchFactor(1, 45);

    setCentralWidget(splitter);

    // Status bar
    m_statusLabel = new QLabel("  Select a file or folder in the tree.");
    statusBar()->addWidget(m_statusLabel, 1);

    // Delete shortcut
    QShortcut* delShortcut = new QShortcut(QKeySequence::Delete, m_tree);
    connect(delShortcut, &QShortcut::activated, this, &MainWindow::onDelete);
}

// ============================================================
//  buildControlPanel
// ============================================================
QWidget* MainWindow::buildControlPanel()
{
    // This widget lives inside a QScrollArea, so it should
    // size itself to its preferred height and let the scroll
    // area handle clipping.
    QWidget*     panel = new QWidget;
    panel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QVBoxLayout* vl = new QVBoxLayout(panel);
    vl->setSpacing(12);              // space BETWEEN group boxes
    vl->setContentsMargins(4, 6, 4, 12);

    // ── Helper: returns a properly-configured QGroupBox ──
    // Qt renders the title inside margin-top space.
    // We always set: margin-top ≥ title font height (~18 px).
    // Then padding-top inside the box ≥ 6 px so first widget
    // never collides with the border.
    auto makeGroup = [](const QString& title) -> QGroupBox* {
        QGroupBox* g = new QGroupBox(title);
        // Override the global QGroupBox stylesheet locally
        // so every group gets exactly the right title clearance.
        g->setStyleSheet(R"(
            QGroupBox {
                font-size: 12px;
                font-weight: bold;
                color: #89b4fa;
                border: 1px solid #45475a;
                border-radius: 6px;
                margin-top: 20px;
                padding-top: 4px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                left: 10px;
                top: 0px;
                padding: 0 6px;
                background-color: #1e1e2e;
            }
        )");
        return g;
    };

    //  1. INPUT GROUP
    QGroupBox*   inputBox  = makeGroup("  ✏  Input");
    QGridLayout* inputGrid = new QGridLayout(inputBox);
    inputGrid->setSpacing(10);
    inputGrid->setContentsMargins(12, 14, 12, 12);
    inputGrid->setColumnMinimumWidth(0, 80);
    inputGrid->setColumnStretch(1, 1);

    auto makeLabel = [](const QString& text) -> QLabel* {
        QLabel* l = new QLabel(text);
        l->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        l->setStyleSheet("color: #bac2de; font-size: 12px; background: transparent;");
        return l;
    };

    inputGrid->addWidget(makeLabel("Name:"),      0, 0);
    m_nameEdit = new QLineEdit;
    m_nameEdit->setPlaceholderText("Enter file or folder name…");
    m_nameEdit->setMinimumHeight(32);
    inputGrid->addWidget(m_nameEdit, 0, 1);

    inputGrid->addWidget(makeLabel("Extension:"), 1, 0);
    m_extEdit = new QLineEdit;
    m_extEdit->setPlaceholderText("txt / cpp / png  (files only)");
    m_extEdit->setMinimumHeight(32);
    inputGrid->addWidget(m_extEdit, 1, 1);

    inputGrid->addWidget(makeLabel("Size (KB):"), 2, 0);
    m_sizeSpin = new QSpinBox;
    m_sizeSpin->setRange(1, 999999);
    m_sizeSpin->setValue(50);
    m_sizeSpin->setSuffix(" KB");
    m_sizeSpin->setMinimumHeight(32);
    m_sizeSpin->setMaximumWidth(150);
    inputGrid->addWidget(m_sizeSpin, 2, 1, Qt::AlignLeft);

    vl->addWidget(inputBox);

    //  2. CREATE GROUP
    QGroupBox*   createBox = makeGroup("  ➕  Create");
    QHBoxLayout* createHL  = new QHBoxLayout(createBox);
    createHL->setSpacing(10);
    createHL->setContentsMargins(12, 14, 12, 12);

    QPushButton* addFolderBtn = new QPushButton("📁  Add Folder");
    addFolderBtn->setFixedHeight(40);
    addFolderBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    addFolderBtn->setStyleSheet(btnStyle("#89b4fa"));

    QPushButton* addFileBtn = new QPushButton("📄  Add File");
    addFileBtn->setFixedHeight(40);
    addFileBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    addFileBtn->setStyleSheet(btnStyle("#a6e3a1"));

    createHL->addWidget(addFolderBtn);
    createHL->addWidget(addFileBtn);
    vl->addWidget(createBox);

    connect(addFolderBtn, &QPushButton::clicked, this, &MainWindow::onAddFolder);
    connect(addFileBtn,   &QPushButton::clicked, this, &MainWindow::onAddFile);

    // ────────────────────────────────────────────────────
    //  3. MANAGE GROUP
    // ────────────────────────────────────────────────────
    QGroupBox*   manageBox = makeGroup("  ⚙  Manage");
    QGridLayout* mg        = new QGridLayout(manageBox);
    mg->setSpacing(8);
    mg->setContentsMargins(12, 14, 12, 12);
    mg->setColumnStretch(0, 1);
    mg->setColumnStretch(1, 1);

    auto mkBtn = [&](const QString& text, const QString& color) -> QPushButton* {
        QPushButton* b = new QPushButton(text);
        b->setFixedHeight(40);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        b->setStyleSheet(btnStyle(color));
        return b;
    };

    QPushButton* deleteBtn = mkBtn("🗑  Delete",      "#f38ba8");
    m_undoBtn              = mkBtn("↩  Undo Delete",  "#fab387");
    m_undoBtn->setEnabled(false);
    QPushButton* renameBtn = mkBtn("✏  Rename",       "#f9e2af");
    QPushButton* moveBtn   = mkBtn("✂  Move To",      "#cba6f7");
    QPushButton* copyBtn   = mkBtn("📋  Copy To",      "#94e2d5");

    mg->addWidget(deleteBtn, 0, 0);
    mg->addWidget(m_undoBtn, 0, 1);
    mg->addWidget(renameBtn, 1, 0);
    mg->addWidget(moveBtn,   1, 1);
    mg->addWidget(copyBtn,   2, 0, 1, 2);   // spans both columns

    vl->addWidget(manageBox);

    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(m_undoBtn, &QPushButton::clicked, this, &MainWindow::onUndoDelete);
    connect(renameBtn, &QPushButton::clicked, this, &MainWindow::onRename);
    connect(moveBtn,   &QPushButton::clicked, this, &MainWindow::onMove);
    connect(copyBtn,   &QPushButton::clicked, this, &MainWindow::onCopy);

    // ────────────────────────────────────────────────────
    //  4. SEARCH GROUP
    // ────────────────────────────────────────────────────
    QGroupBox*   searchBox = makeGroup("  🔍  Search  (DFS Traversal)");
    QHBoxLayout* searchHL  = new QHBoxLayout(searchBox);
    searchHL->setSpacing(8);
    searchHL->setContentsMargins(12, 14, 12, 12);

    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("Type name to search…");
    m_searchEdit->setMinimumHeight(36);

    QPushButton* searchBtn = new QPushButton("🔍  Search");
    searchBtn->setFixedHeight(36);
    searchBtn->setMinimumWidth(110);
    searchBtn->setStyleSheet(btnStyle("#89dceb"));

    searchHL->addWidget(m_searchEdit, 1);
    searchHL->addWidget(searchBtn);
    vl->addWidget(searchBox);

    connect(searchBtn,    &QPushButton::clicked,     this, &MainWindow::onSearch);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &MainWindow::onSearch);

    // ────────────────────────────────────────────────────
    //  5. DISK USAGE GROUP
    // ────────────────────────────────────────────────────
    QGroupBox*   diskBox = makeGroup("  💾  Disk Usage  (Simulated 1 GB)");
    QVBoxLayout* diskVL  = new QVBoxLayout(diskBox);
    diskVL->setSpacing(8);
    diskVL->setContentsMargins(12, 14, 12, 12);

    m_diskLabel = new QLabel("0.00 MB  of  1 GB  used");
    m_diskLabel->setAlignment(Qt::AlignCenter);
    m_diskLabel->setStyleSheet("font-size: 11px; color: #6c7086; background: transparent;");

    m_diskBar = new QProgressBar;
    m_diskBar->setRange(0, 100);
    m_diskBar->setValue(0);
    m_diskBar->setFormat("%p%");
    m_diskBar->setTextVisible(true);
    m_diskBar->setFixedHeight(22);

    diskVL->addWidget(m_diskLabel);
    diskVL->addWidget(m_diskBar);
    vl->addWidget(diskBox);

    // Bottom padding — no addStretch() here because the widget
    // lives inside a QScrollArea with Preferred size policy.
    vl->addSpacing(8);

    return panel;
}

// ============================================================
//  buildLogPanel
// ============================================================
QWidget* MainWindow::buildLogPanel()
{
    QGroupBox*   logBox = new QGroupBox("  📜  Activity Log  (Queue — FIFO)");
    // Same title-safe style as control panel groups
    logBox->setStyleSheet(R"(
        QGroupBox {
            font-size: 12px;
            font-weight: bold;
            color: #89b4fa;
            border: 1px solid #45475a;
            border-radius: 6px;
            margin-top: 20px;
            padding-top: 4px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 10px;
            top: 0px;
            padding: 0 6px;
            background-color: #1e1e2e;
        }
    )");

    QVBoxLayout* logVL = new QVBoxLayout(logBox);
    logVL->setSpacing(8);
    logVL->setContentsMargins(10, 14, 10, 10);

    m_logEdit = new QTextEdit;
    m_logEdit->setReadOnly(true);
    m_logEdit->setMinimumHeight(100);
    logVL->addWidget(m_logEdit, 1);

    QPushButton* clearBtn = new QPushButton("Clear Log");
    clearBtn->setFixedHeight(30);
    clearBtn->setMinimumWidth(100);
    clearBtn->setMaximumWidth(120);
    clearBtn->setStyleSheet(btnStyle("#6c7086"));

    QHBoxLayout* row = new QHBoxLayout;
    row->setContentsMargins(0, 0, 0, 0);
    row->addStretch();
    row->addWidget(clearBtn);
    logVL->addLayout(row);

    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearLog);
    return logBox;
}

// ============================================================
//  Tree population
// ============================================================
void MainWindow::refreshTree()
{
    // remember expanded paths
    QStringList expandedPaths;
    QTreeWidgetItemIterator it(m_tree, QTreeWidgetItemIterator::HasChildren);
    while (*it) {
        if ((*it)->isExpanded()) {
            FileSystemNode* n = nodeFromItem(*it);
            if (n) expandedPaths << m_fs.pathOf(n);
        }
        ++it;
    }

    m_tree->blockSignals(true);
    m_tree->clear();

    // root item
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(m_tree);
    rootItem->setText(0, "  💾  " + m_fs.root()->name);
    rootItem->setText(1, "");
    rootItem->setText(2, "");
    rootItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)m_fs.root()));
    rootItem->setExpanded(true);

    for (int i = 0; i < m_fs.root()->children.size(); ++i)
        populateItem(rootItem, m_fs.root()->children[i]);

    // restore expanded states
    QTreeWidgetItemIterator it2(m_tree, QTreeWidgetItemIterator::HasChildren);
    while (*it2) {
        FileSystemNode* n = nodeFromItem(*it2);
        if (n && expandedPaths.contains(m_fs.pathOf(n)))
            (*it2)->setExpanded(true);
        ++it2;
    }

    m_tree->blockSignals(false);
}

void MainWindow::populateItem(QTreeWidgetItem* parentItem, FileSystemNode* node)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);

    if (node->isFolder()) {
        item->setText(0, "  📁  " + node->name);
        item->setText(1, "  —");
        item->setForeground(0, QColor("#89b4fa"));
    } else {
        // choose icon by type
        QString icon = "📄";
        if (node->isImageFile())     icon = "🖼";
        else if (node->extension == "pdf") icon = "📕";
        else if (node->extension == "cpp" ||
                 node->extension == "h"   ||
                 node->extension == "c")   icon = "⚙";

        item->setText(0, "  " + icon + "  " + node->displayName());
        item->setText(1, QString("  %1 KB").arg(node->sizeKB));
        item->setForeground(0, QColor("#cdd6f4"));
    }

    item->setText(2, "  " + node->createdAt.toString("dd-MMM-yy hh:mm"));
    item->setData(0, Qt::UserRole, QVariant::fromValue((void*)node));
    item->setToolTip(0, node->metaString());

    for (int i = 0; i < node->children.size(); ++i)
        populateItem(item, node->children[i]);
}

FileSystemNode* MainWindow::nodeFromItem(QTreeWidgetItem* item) const
{
    if (!item) return nullptr;
    return static_cast<FileSystemNode*>(
        item->data(0, Qt::UserRole).value<void*>());
}

FileSystemNode* MainWindow::selectedNode() const
{
    return nodeFromItem(m_tree->currentItem());
}

// ============================================================
//  Context menu  (Right-click on tree item)
// ============================================================
void MainWindow::onTreeContextMenu(const QPoint& pos)
{
    QTreeWidgetItem* item = m_tree->itemAt(pos);
    FileSystemNode*  node = nodeFromItem(item);

    QMenu menu(this);

    if (node && node != m_fs.root()) {
        // ── Open (files only) ──
        if (node->isFile()) {
            QAction* openAct = menu.addAction("🔍  Open / View File");
            connect(openAct, &QAction::triggered, this,
                    [this, node]{ openFileViewer(node); });
            menu.addSeparator();
        }

        // ── Add inside (folders only) ──
        if (node->isFolder()) {
            QAction* addFolAct  = menu.addAction("📁  Add Folder Here");
            QAction* addFileAct = menu.addAction("📄  Add File Here");
            menu.addSeparator();
            connect(addFolAct,  &QAction::triggered, this, &MainWindow::onAddFolder);
            connect(addFileAct, &QAction::triggered, this, &MainWindow::onAddFile);
        }

        // ── Standard ops ──
        QAction* renameAct = menu.addAction("✏  Rename");
        QAction* moveAct   = menu.addAction("✂  Move To…");
        QAction* copyAct   = menu.addAction("📋  Copy To…");
        menu.addSeparator();
        QAction* deleteAct = menu.addAction("🗑  Delete");
        deleteAct->setShortcut(QKeySequence::Delete);

        connect(renameAct, &QAction::triggered, this, &MainWindow::onRename);
        connect(moveAct,   &QAction::triggered, this, &MainWindow::onMove);
        connect(copyAct,   &QAction::triggered, this, &MainWindow::onCopy);
        connect(deleteAct, &QAction::triggered, this, &MainWindow::onDelete);

    } else if (!node || node == m_fs.root()) {
        // Clicked on root or blank area
        QAction* addFolAct  = menu.addAction("📁  Add Folder Here");
        QAction* addFileAct = menu.addAction("📄  Add File Here");
        connect(addFolAct,  &QAction::triggered, this, &MainWindow::onAddFolder);
        connect(addFileAct, &QAction::triggered, this, &MainWindow::onAddFile);
    }

    // Undo — always visible if stack non-empty
    if (m_fs.canUndo()) {
        menu.addSeparator();
        QAction* undoAct = menu.addAction(
            QString("↩  Undo Delete  \"%1\"").arg(m_fs.undoPreview()));
        connect(undoAct, &QAction::triggered, this, &MainWindow::onUndoDelete);
    }

    if (!menu.isEmpty())
        menu.exec(m_tree->viewport()->mapToGlobal(pos));
}

// ============================================================
//  Tree selection
// ============================================================
void MainWindow::onTreeSelectionChanged()
{
    FileSystemNode* n = selectedNode();
    if (n)
        m_statusLabel->setText("  " + m_fs.pathOf(n) + "    |    " + n->metaString());
    else
        m_statusLabel->setText("  Select a file or folder.");
}

void MainWindow::onTreeItemDoubleClicked(QTreeWidgetItem* item, int)
{
    FileSystemNode* node = nodeFromItem(item);
    if (!node) return;

    if (node->isFile()) {
        openFileViewer(node);
    } else {
        item->setExpanded(!item->isExpanded());
    }
}

// ============================================================
//  File viewer / editor
// ============================================================
void MainWindow::openFileViewer(FileSystemNode* file)
{
    if (!file || file->isFolder()) return;

    QDialog* dlg = new QDialog(this);
    dlg->setWindowTitle("  " + file->displayName() + "  —  " + m_fs.pathOf(file));
    dlg->resize(680, 520);
    dlg->setStyleSheet("QDialog { background-color: #1e1e2e; }");

    QVBoxLayout* vl = new QVBoxLayout(dlg);
    vl->setSpacing(8);
    vl->setContentsMargins(12, 12, 12, 12);

    // ── header ──
    QLabel* header = new QLabel(
        QString("  %1  %2").arg(file->isImageFile() ? "🖼" : "📄").arg(file->metaString()));
    header->setStyleSheet(
        "color:#89b4fa; font-size:11px; padding:4px 6px;"
        "background:#181825; border-radius:4px;");
    vl->addWidget(header);

    if (file->isImageFile()) {
        // ── Image: show placeholder ──
        QLabel* imgLabel = new QLabel(
            "🖼  Image Preview\n\n"
            "Image rendering is not supported in this simulator.\n\n"
            "File: " + file->displayName() + "\n"
                                    "Size: " + QString::number(file->sizeKB) + " KB");
        imgLabel->setAlignment(Qt::AlignCenter);
        imgLabel->setStyleSheet(
            "color:#6c7086; font-size:14px; border:2px dashed #45475a;"
            "border-radius:8px; padding:40px; background:#11111b;");
        vl->addWidget(imgLabel, 1);

        QPushButton* closeBtn = new QPushButton("Close");
        closeBtn->setStyleSheet(btnStyle("#6c7086"));
        closeBtn->setFixedHeight(34);
        QHBoxLayout* row = new QHBoxLayout;
        row->addStretch();
        row->addWidget(closeBtn);
        vl->addLayout(row);
        connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::accept);

    } else if (file->isTextFile()) {
        // ── Text: fully editable ──
        QTextEdit* editor = new QTextEdit;
        editor->setFont(QFont("Cascadia Code, Consolas, Courier New", 12));
        editor->setText(file->content);
        editor->setStyleSheet(
            "background:#11111b; color:#cdd6f4; border:1px solid #45475a;"
            "border-radius:5px; padding:4px;");
        vl->addWidget(editor, 1);

        QLabel* hint = new QLabel("  Ctrl+S  or  Save button to save  •  Edits do not persist after closing without saving.");
        hint->setStyleSheet("color:#6c7086; font-size:10px;");
        vl->addWidget(hint);

        QHBoxLayout* row = new QHBoxLayout;
        row->addStretch();
        QPushButton* saveBtn  = new QPushButton("💾  Save");
        QPushButton* closeBtn = new QPushButton("Close");
        saveBtn->setStyleSheet(btnStyle("#a6e3a1"));
        closeBtn->setStyleSheet(btnStyle("#6c7086"));
        saveBtn->setFixedHeight(34);
        closeBtn->setFixedHeight(34);
        saveBtn->setMinimumWidth(90);
        closeBtn->setMinimumWidth(80);
        row->addWidget(saveBtn);
        row->addWidget(closeBtn);
        vl->addLayout(row);

        // Ctrl+S shortcut inside dialog
        QShortcut* saveShortcut = new QShortcut(
            QKeySequence(Qt::CTRL | Qt::Key_S), dlg);

        auto doSave = [=]() mutable {
            file->content = editor->toPlainText();
            // Update size estimate (1 char ≈ 1 byte ≈ 0.001 KB, min 1)
            int oldSize = file->sizeKB;
            file->sizeKB = qMax(1, file->content.length() / 1024);
            m_fs.totalSizeKBRef() += (file->sizeKB - oldSize);
            updateDiskUsage();
            header->setText(QString("  💾  Saved!  %1").arg(file->metaString()));
            header->setStyleSheet(
                "color:#a6e3a1; font-size:11px; padding:4px 6px;"
                "background:#1a2e1a; border-radius:4px;");
        };

        connect(saveBtn,     &QPushButton::clicked, dlg, doSave);
        connect(saveShortcut,&QShortcut::activated, dlg, doSave);
        connect(closeBtn,    &QPushButton::clicked, dlg, &QDialog::accept);

    } else {
        // ── Unknown type ──
        QLabel* unknown = new QLabel(
            "❓  Cannot Preview\n\n"
            "File type '." + file->extension + "' has no built-in viewer.\n\n"
                                "Supported text types: .txt  .cpp  .h  .py  .js  .html  .css …\n"
                                "Image types show a placeholder.");
        unknown->setAlignment(Qt::AlignCenter);
        unknown->setStyleSheet(
            "color:#f9e2af; font-size:13px; border:2px dashed #45475a;"
            "border-radius:8px; padding:30px; background:#11111b;");
        vl->addWidget(unknown, 1);

        QPushButton* closeBtn = new QPushButton("Close");
        closeBtn->setStyleSheet(btnStyle("#6c7086"));
        closeBtn->setFixedHeight(34);
        QHBoxLayout* row = new QHBoxLayout;
        row->addStretch();
        row->addWidget(closeBtn);
        vl->addLayout(row);
        connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::accept);
    }

    dlg->exec();
    dlg->deleteLater();
    refreshTree();   // in case size changed
}

// ============================================================
//  Add Folder
// ============================================================
void MainWindow::onAddFolder()
{
    FileSystemNode* target = selectedNode();

    // If nothing selected → use root
    if (!target) target = m_fs.root();
    // If a file selected → use its parent
    if (target->isFile()) target = target->parent;
    if (!target) target = m_fs.root();

    QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        bool ok;
        name = QInputDialog::getText(this, "Add Folder",
                                     "Folder name:", QLineEdit::Normal, "", &ok);
        if (!ok || name.trimmed().isEmpty()) return;
        name = name.trimmed();
    }

    FileSystemNode* created = m_fs.addFolder(target, name);
    if (!created) {
        QMessageBox::warning(this, "Add Folder",
                             QString("A folder named \"%1\" already exists in \"%2\".")
                                 .arg(name).arg(target->name));
        return;
    }

    m_nameEdit->clear();
    refreshTree();
    refreshLog();
    updateDiskUsage();
}

// ============================================================
//  Add File
// ============================================================
void MainWindow::onAddFile()
{
    FileSystemNode* target = selectedNode();
    if (!target) target = m_fs.root();
    if (target->isFile()) target = target->parent;
    if (!target) target = m_fs.root();

    QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        bool ok;
        name = QInputDialog::getText(this, "Add File",
                                     "File name (without extension):", QLineEdit::Normal, "", &ok);
        if (!ok || name.trimmed().isEmpty()) return;
        name = name.trimmed();
    }

    QString ext = m_extEdit->text().trimmed().toLower();
    if (ext.isEmpty()) ext = "txt";

    int sizeKB = m_sizeSpin->value();

    m_fs.addFile(target, name, ext, sizeKB);

    m_nameEdit->clear();
    m_extEdit->clear();
    refreshTree();
    refreshLog();
    updateDiskUsage();
}

// ============================================================
//  Delete
// ============================================================
void MainWindow::onDelete()
{
    FileSystemNode* node = selectedNode();
    if (!node || node == m_fs.root()) {
        QMessageBox::information(this, "Delete",
                                 "Please select a file or folder to delete.");
        return;
    }

    QString what = node->isFolder()
                       ? QString("folder \"%1\" and all its contents").arg(node->name)
                       : QString("file \"%1\"").arg(node->displayName());

    auto ans = QMessageBox::question(
        this, "Move to Recycle Bin",
        "Delete " + what + "?\n\nYou can restore it using Undo Delete.",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ans != QMessageBox::Yes) return;

    m_fs.deleteNode(node);
    m_undoBtn->setEnabled(m_fs.canUndo());
    refreshTree();
    refreshLog();
    updateDiskUsage();
}

// ============================================================
//  Undo Delete
// ============================================================
void MainWindow::onUndoDelete()
{
    if (!m_fs.canUndo()) return;
    m_fs.undoDelete();
    m_undoBtn->setEnabled(m_fs.canUndo());
    refreshTree();
    refreshLog();
    updateDiskUsage();
}

// ============================================================
//  Rename
// ============================================================
void MainWindow::onRename()
{
    FileSystemNode* node = selectedNode();
    if (!node || node == m_fs.root()) {
        QMessageBox::information(this, "Rename",
                                 "Please select a file or folder to rename.");
        return;
    }

    bool ok;
    QString newName = QInputDialog::getText(
        this, "Rename",
        QString("New name for \"%1\":\n(Do not include extension for files)")
            .arg(node->displayName()),
        QLineEdit::Normal, node->name, &ok);

    if (ok && !newName.trimmed().isEmpty()) {
        m_fs.rename(node, newName.trimmed());
        refreshTree();
        refreshLog();
    }
}

// ============================================================
//  Search  (DFS)
// ============================================================
void MainWindow::onSearch()
{
    QString query = m_searchEdit->text().trimmed();
    if (query.isEmpty()) {
        QMessageBox::information(this, "Search",
                                 "Type something in the Search field first.");
        return;
    }

    DynArray<FileSystemNode*> results = m_fs.search(query);
    refreshLog();

    if (results.isEmpty()) {
        QMessageBox::information(this, "Search Results",
                                 QString("No files or folders found matching \"%1\".").arg(query));
        return;
    }

    QStringList lines;
    lines << QString("Found %1 result(s) for  \"%2\":\n").arg(results.size()).arg(query);
    for (int i = 0; i < results.size(); ++i) {
        FileSystemNode* n = results[i];
        lines << QString("  %1  %2")
                     .arg(n->isFolder() ? "📁" : "📄")
                     .arg(m_fs.pathOf(n));
    }

    QMessageBox box(this);
    box.setWindowTitle("Search Results");
    box.setIcon(QMessageBox::Information);
    box.setText(lines.join("\n"));
    box.exec();
}

// ============================================================
//  Move
// ============================================================
void MainWindow::onMove()
{
    FileSystemNode* node = selectedNode();
    if (!node || node == m_fs.root()) {
        QMessageBox::information(this, "Move",
                                 "Select the item you want to move, then click Move To.");
        return;
    }

    bool ok;
    QString destName = QInputDialog::getText(
        this, "Move To",
        QString("Move \"%1\" into which folder?\n(Enter exact folder name)")
            .arg(node->displayName()),
        QLineEdit::Normal, "", &ok);
    if (!ok || destName.trimmed().isEmpty()) return;

    // Find destination using DFS search
    DynArray<FileSystemNode*> hits = m_fs.search(destName.trimmed());
    FileSystemNode* dest = nullptr;
    for (int i = 0; i < hits.size(); ++i) {
        if (hits[i]->isFolder() &&
            hits[i]->name.compare(destName.trimmed(), Qt::CaseInsensitive) == 0)
        { dest = hits[i]; break; }
    }

    if (!dest) {
        QMessageBox::warning(this, "Move",
                             QString("Folder \"%1\" not found.\nCheck the spelling and try again.")
                                 .arg(destName));
        return;
    }
    if (!m_fs.move(node, dest)) {
        QMessageBox::warning(this, "Move",
                             "Cannot move here.\n(Destination may be the same folder, or a sub-folder of the item.)");
        return;
    }

    refreshTree();
    refreshLog();
}

// ============================================================
//  Copy
// ============================================================
void MainWindow::onCopy()
{
    FileSystemNode* node = selectedNode();
    if (!node || node == m_fs.root()) {
        QMessageBox::information(this, "Copy",
                                 "Select a file or folder to copy first.");
        return;
    }

    bool ok;
    QString destName = QInputDialog::getText(
        this, "Copy To",
        QString("Copy \"%1\" into which folder?\n(Enter exact folder name)")
            .arg(node->displayName()),
        QLineEdit::Normal, "", &ok);
    if (!ok || destName.trimmed().isEmpty()) return;

    DynArray<FileSystemNode*> hits = m_fs.search(destName.trimmed());
    FileSystemNode* dest = nullptr;
    for (int i = 0; i < hits.size(); ++i) {
        if (hits[i]->isFolder() &&
            hits[i]->name.compare(destName.trimmed(), Qt::CaseInsensitive) == 0)
        { dest = hits[i]; break; }
    }

    if (!dest) {
        QMessageBox::warning(this, "Copy",
                             QString("Folder \"%1\" not found.").arg(destName));
        return;
    }
    if (!m_fs.copy(node, dest)) {
        QMessageBox::warning(this, "Copy", "Cannot copy to that location.");
        return;
    }

    refreshTree();
    refreshLog();
    updateDiskUsage();
}

// ============================================================
//  Clear Log
// ============================================================
void MainWindow::onClearLog()
{
    m_fs.clearLog();
    m_logEdit->clear();
}

// ============================================================
//  refreshLog
// ============================================================
void MainWindow::refreshLog()
{
    m_logEdit->clear();
    const Queue<ActivityEntry>& log = m_fs.activityLog();
    int n = log.size();
    // Display newest first — iterate in reverse via indexed access
    for (int i = n - 1; i >= 0; --i) {
        const ActivityEntry& e = log.at(i);
        m_logEdit->append(e.formatted());
    }
    m_logEdit->moveCursor(QTextCursor::Start);
}

// ============================================================
//  updateDiskUsage
// ============================================================
void MainWindow::updateDiskUsage()
{
    int used = m_fs.totalSizeKB();
    int cap  = m_fs.diskCapacityKB();

    double usedMB = used / 1024.0;
    double capGB  = cap  / (1024.0 * 1024.0);

    m_diskLabel->setText(
        QString("%1 MB  of  %2 GB  used")
            .arg(usedMB, 0, 'f', 2)
            .arg(capGB,  0, 'f', 0));

    int pct = (cap > 0) ? qMin(100, (used * 100) / cap) : 0;
    m_diskBar->setValue(pct);
    m_diskBar->setFormat(QString("%1%").arg(pct));
}

// ============================================================
//  Button style
// ============================================================
QString MainWindow::btnStyle(const QString& color) const
{
    return QString(R"(
        QPushButton {
            background-color: #24243e;
            color: %1;
            border: 1px solid %1;
            border-radius: 5px;
            padding: 5px 14px;
            font-weight: bold;
            font-size: 12px;
        }
        QPushButton:hover {
            background-color: %1;
            color: #1e1e2e;
        }
        QPushButton:pressed {
            background-color: #181825;
            color: %1;
        }
        QPushButton:disabled {
            background-color: #1e1e2e;
            color: #45475a;
            border: 1px solid #313244;
        }
    )").arg(color);
}
