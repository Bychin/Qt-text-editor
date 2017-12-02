#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    tabs = new QTabWidget(this);
    initial_edit = new QTextEdit;
    tabs->addTab(initial_edit, "Безымянный");
    tabs->setTabsClosable(true);
    QToolButton* new_tab_button = new QToolButton;
    new_tab_button->setText("+");
    connect(new_tab_button, SIGNAL(clicked()), this, SLOT(CreateFile()));
    tabs->setCornerWidget(new_tab_button, Qt::TopRightCorner);

    mainLayout = new QGridLayout;
    mainLayout->addWidget(tabs, 0, 0, 1, 1);
    setCentralWidget(tabs);

    SetupMenuBar();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::SetupMenuBar() {
    QMenu* fileMenu = new QMenu("File");
    QMenu* editMenu = new QMenu("Edit");
    QMenu* viewMenu = new QMenu("View");

    fileMenu->addAction("New File", this, SLOT(CreateFile()), Qt::CTRL + Qt::Key_N);
    fileMenu->addAction("Open File", this, SLOT(OpenFile()), Qt::CTRL + Qt::Key_O);
    fileMenu->addSeparator();
    fileMenu->addAction("Save File", this, SLOT(SaveFile()), Qt::CTRL + Qt::Key_S);
    fileMenu->addAction("Save As...", this, SLOT(SaveFileAs()), Qt::SHIFT + Qt::CTRL + Qt::Key_S);
    fileMenu->addAction("Save All", this, SLOT(SaveAllFiles()));
    fileMenu->addSeparator();
    fileMenu->addAction("Close File", this, SLOT(ClearTree()), Qt::CTRL + Qt::Key_W);
    fileMenu->addAction("Close All", this, SLOT(ClearTree()), Qt::SHIFT + Qt::CTRL + Qt::Key_W);
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, SLOT(CloseWindow()), Qt::CTRL + Qt::Key_Q);

    editMenu->addAction("Cut", this, SLOT(Cut()), Qt::CTRL + Qt::Key_X);
    editMenu->addAction("Copy", this, SLOT(Copy()), Qt::CTRL + Qt::Key_C);
    editMenu->addAction("Paste", this, SLOT(Paste()), Qt::CTRL + Qt::Key_V);
    editMenu->addAction("Delete", this, SLOT(Delete()), Qt::CTRL + Qt::Key_Backspace);
    editMenu->addAction("Select All", this, SLOT(SelectAll()), Qt::CTRL + Qt::Key_A);

    file_explorer = viewMenu->addAction("File Explorer", this, SLOT(FileExplorerHandler()));
    openned_docs = viewMenu->addAction("Openned Docs", this, SLOT(OpennedDocsHandler()));
    file_explorer->setCheckable(true);
    file_explorer->setChecked(false);
    openned_docs->setCheckable(true);
    openned_docs->setChecked(false);

    ui->menuBar->addMenu(fileMenu);
    ui->menuBar->addMenu(editMenu);
    ui->menuBar->addMenu(viewMenu);
}

void MainWindow::CreateFile() {
    QTextEdit* new_text_edit = new QTextEdit;
    int index = tabs->addTab(new_text_edit, "Безымянный");
}
