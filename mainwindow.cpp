#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setAcceptDrops(true);
    setWindowIcon(QIcon(":/icons/icon.png"));
    setWindowTitle("Text Editor");

    SetupTabWidget();
    SetupFileExplorerDock();
    SetupOpennedDocsDock();
    CreateFile();
    SetupMenuBar();
    SetupToolBar();

    setCentralWidget(tabs);
    tabs->currentWidget()->setFocus();
    highlighter = new Highlighter(":/highlights/languages.xml", this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent* drag_event) {
    if (drag_event->mimeData()->hasUrls())
        drag_event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent* drop_event) {
    QList<QUrl> url_list = drop_event->mimeData()->urls();
    foreach (QUrl url, url_list) {
        OpenFile(url.url(QUrl::RemoveScheme));
    }
}

void MainWindow::SetupTabWidget() {
    tabs = new QTabWidget(this);
    tabs->setMovable(true);
    tabs->setTabsClosable(true);
    tabs->setUsesScrollButtons(true);
    connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseFile(int)));

    QToolButton* new_tab_button = new QToolButton;
    new_tab_button->setText("+");
    connect(new_tab_button, SIGNAL(clicked()), this, SLOT(CreateFile()));
    tabs->setCornerWidget(new_tab_button, Qt::TopRightCorner);
}

void MainWindow::SetupMenuBar() {
    QMenu* fileMenu = new QMenu("File");
    QMenu* editMenu = new QMenu("Edit");
    QMenu* viewMenu = new QMenu("View");

    fileMenu->addAction("New File",   this, SLOT(CreateFile()),    Qt::CTRL + Qt::Key_N);
    fileMenu->addAction("Open File",  this, SLOT(OpenFile()),      Qt::CTRL + Qt::Key_O);
    fileMenu->addSeparator();
    fileMenu->addAction("Save File",  this, SLOT(SaveFile()),      Qt::CTRL + Qt::Key_S);
    fileMenu->addAction("Save As...", this, SLOT(SaveFileAs()),    Qt::SHIFT + Qt::CTRL + Qt::Key_S);
    fileMenu->addAction("Save All",   this, SLOT(SaveAllFiles()));
    fileMenu->addSeparator();
    fileMenu->addAction("Close File", this, SLOT(CloseFile()),     Qt::CTRL + Qt::Key_W);
    fileMenu->addAction("Close All",  this, SLOT(CloseAllFiles()), Qt::SHIFT + Qt::CTRL + Qt::Key_W);
    fileMenu->addSeparator();
    fileMenu->addAction("Exit",       this, SLOT(CloseWindow()),   Qt::CTRL + Qt::Key_Q);

    editMenu->addAction("Cut",        this, SLOT(slotCut()),       Qt::CTRL + Qt::Key_X);
    editMenu->addAction("Copy",       this, SLOT(slotCopy()),      Qt::CTRL + Qt::Key_C);
    editMenu->addAction("Paste",      this, SLOT(slotPaste()),     Qt::CTRL + Qt::Key_V);
    editMenu->addAction("Delete",     this, SLOT(slotClear()),     Qt::CTRL + Qt::Key_Backspace);
    editMenu->addAction("Select All", this, SLOT(slotSelectAll()), Qt::CTRL + Qt::Key_A);

    file_explorer = viewMenu->addAction("File Explorer", this, SLOT(FileExplorerHandler()));
    openned_docs  = viewMenu->addAction("Openned Docs",  this, SLOT(OpennedDocsHandler()));
    file_explorer->setCheckable(true);
    file_explorer->setChecked(false);
    openned_docs->setCheckable(true);
    openned_docs->setChecked(false);

    ui->menuBar->addMenu(fileMenu);
    ui->menuBar->addMenu(editMenu);
    ui->menuBar->addMenu(viewMenu);
}

void MainWindow::SetupToolBar() {
    ui->mainToolBar->addAction(QIcon(":/icons/new_file.png"),       "Create New File", this, SLOT(CreateFile()));
    ui->mainToolBar->addAction(QIcon(":/icons/open_file.png"),      "Open File",       this, SLOT(OpenFile()));
    ui->mainToolBar->addAction(QIcon(":/icons/save_file.png"),      "Save File",       this, SLOT(SaveFile()));
    ui->mainToolBar->addAction(QIcon(":/icons/save_all_files.png"), "Save All Files",  this, SLOT(SaveAllFiles()));
}

void MainWindow::SetupFileExplorerDock() {
    file_system_model = new QDirModel;
    file_system_view  = new QTreeView;
    file_system_view->setModel(file_system_model);
    file_system_view->setRootIndex(file_system_model->index("/home"));
    file_system_view->setColumnHidden(1, true);
    file_system_view->setColumnHidden(2, true);
    file_system_view->setColumnHidden(3, true);
    file_system_view->setHeaderHidden(true);
    connect(file_system_view, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OpenFile(QModelIndex)));

    file_explorer_dock = new QDockWidget("File explorer", this);

    file_explorer_dock->setWidget(file_system_view);
    file_explorer_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    file_explorer_dock->hide();
    addDockWidget(Qt::LeftDockWidgetArea, file_explorer_dock);
}

void MainWindow::SetupOpennedDocsDock() {
    openned_docs_widget = new QListWidget;

    // update on openning/creating new file
    // delete on deleting tab provided by DeleteTabFromList(int) function
    // update position in list
    connect(tabs->tabBar(),      SIGNAL(tabMoved(int, int)),            this, SLOT(ChangeTabIndexInList(int, int)));
    connect(openned_docs_widget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(UpdateCurrentIndex(QListWidgetItem*)));
    connect(openned_docs_widget, SIGNAL(currentRowChanged(int)),        tabs, SLOT(setCurrentIndex(int)));
    connect(tabs->tabBar(),      SIGNAL(currentChanged(int)),           this, SLOT(UpdateCurrentIndex(int)));
    connect(tabs->tabBar(),      SIGNAL(tabCloseRequested(int)),        this, SLOT(UpdateCurrentIndexOnDelete(int)));

    openned_docs_dock  = new QDockWidget("Openned files", this);

    openned_docs_dock->setWidget(openned_docs_widget);
    openned_docs_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    openned_docs_dock->hide();
    addDockWidget(Qt::RightDockWidgetArea, openned_docs_dock);
}

void MainWindow::CreateFile() {
    CodeEditor* new_text_edit = new CodeEditor;
    int index = tabs->addTab(new_text_edit, "untitled");
    tabs->setCurrentIndex(index);
    tabs->setTabToolTip(index, "");
    tabs->setTabWhatsThis(index, "No changes");
    connect(new_text_edit, SIGNAL(textChanged()), this, SLOT(UpdateParameter()));

    QListWidgetItem* new_item = new QListWidgetItem;
    new_item->setText(tabs->tabText(index));
    new_item->setToolTip(tabs->tabToolTip(index));
    openned_docs_widget->addItem(new_item);

    UpdateCurrentIndex(index);
}

void MainWindow::OpenFile() {
    QString filepath = QFileDialog::getOpenFileName(this, "Choose file", "/home");
    if (filepath.isEmpty())
        return;
    OpenFile(filepath);
}

void MainWindow::OpenFile(const QString& filepath) {
    // checks for duplicate file-openning and prevents it
    for (int i = 0; i < tabs->count(); ++i)
        if (tabs->tabToolTip(i) == filepath) {
            QMessageBox::warning(this, "Duplicate", "Cannot open one file twice!",
                                          QMessageBox::Ok);
            return;
        }

    QString filename = filepath.section("/",-1,-1);
    QFile file(filepath);

    if (file.open(QIODevice::ReadOnly)) {
        CodeEditor* temp_text = (CodeEditor*)tabs->currentWidget(); // TODO: change this unsafe check!
        if (temp_text->document()->isEmpty() &&
            tabs->tabToolTip(tabs->currentIndex()) == "" &&
            tabs->tabText(tabs->currentIndex()) == "untitled") {
            DeleteTabFromList(tabs->currentIndex());
            delete tabs->widget(tabs->currentIndex());
        }

        CodeEditor* new_text_edit = new CodeEditor;
        QString content = file.readAll();
        file.close();
        new_text_edit->appendPlainText(content);
        int index = tabs->addTab(new_text_edit, filename);
        tabs->setCurrentIndex(index);
        tabs->setTabToolTip(index, filepath);
        tabs->setTabWhatsThis(index, "No changes");
        connect(new_text_edit, SIGNAL(textChanged()), this, SLOT(UpdateParameter()));

        QListWidgetItem* new_item = new QListWidgetItem;
        new_item->setText(tabs->tabText(index));
        new_item->setToolTip(tabs->tabToolTip(index));
        openned_docs_widget->addItem(new_item);

        QString file_extension = QFileInfo(filename).suffix(); // setting up highlight
        if (highlighter->getRequest(file_extension)) {
            highlighter->setDocument(new_text_edit->document());
            highlighter->highlightBlock(new_text_edit->toPlainText());
        }

        tabs->setTabWhatsThis(index, "No changes");
        UpdateCurrentIndex(index); // setting up selected item in openned_docs_dock
    } else {
        (new QErrorMessage(this))->showMessage("Cannot open file!");
        return;
    }
}

void MainWindow::SaveFile() {
    if (tabs->tabToolTip(tabs->currentIndex()) == "") {
        SaveFileAs();
        return;
    }
    QString filepath = tabs->tabToolTip(tabs->currentIndex());
    QFile file(filepath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(((CodeEditor*)tabs->currentWidget())->document()->toPlainText().toUtf8()); // unsafe getting!
        file.close();
        tabs->setTabWhatsThis(tabs->currentIndex(), "No changes");
    } else {
        (new QErrorMessage(this))->showMessage("Cannot save file!");
        return;
    }
}

void MainWindow::SaveFileAs() {
    QString filename = tabs->tabText(tabs->currentIndex());
    QString filepath = QFileDialog::getSaveFileName(this, "Save " + filename, "/home/" + filename);
    if (filepath.isEmpty())
        return;
    if (QFileInfo(filepath).suffix().isEmpty())
        filepath.append(".txt");
    if (QFileInfo(filepath).suffix() == "pdf") {
        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPaperSize(QPrinter::A4);
        printer.setOutputFileName(filepath);
        //QTextDocument.setPageSize(printer.pageRect().size());
        ((CodeEditor*)tabs->currentWidget())->document()->print(&printer);
    } else {
        QFile file(filepath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(((CodeEditor*)tabs->currentWidget())->document()->toPlainText().toUtf8()); // unsafe getting!
            file.close();
        } else {
            (new QErrorMessage(this))->showMessage("Cannot save file!");
            return;
        }
    }
    filename = filepath.section("/",-1,-1);
    tabs->tabBar()->setTabText(tabs->currentIndex(), filename);
    tabs->tabBar()->setTabToolTip(tabs->currentIndex(), filepath);

    QString file_extension = QFileInfo(filename).suffix(); // setting up highlight
    if (highlighter->getRequest(file_extension)) {
        highlighter->setDocument(((CodeEditor*)tabs->currentWidget())->document()); // unsafe getting!
        highlighter->highlightBlock(((CodeEditor*)tabs->currentWidget())->toPlainText()); // unsafe getting!
    }

    tabs->setTabWhatsThis(tabs->currentIndex(), "No changes");
}

void MainWindow::SaveAllFiles() {
    int current_index = tabs->currentIndex();
    for (int i = 0; i < tabs->count(); ++i) { // QTabWidget guarantees the consistency of indices?
        tabs->setCurrentIndex(i);
        if (tabs->tabWhatsThis(tabs->currentIndex()) != "No changes")
            SaveFile();
    }
    tabs->setCurrentIndex(current_index);
}

void MainWindow::CloseFile(int index_) {
    if (tabs->tabWhatsThis(tabs->currentIndex()) != "No changes") {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Saving changes", "Save changes before closing?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            SaveFile();
        }
    }
    delete tabs->widget(index_);

    DeleteTabFromList(index_);

    if (!tabs->count())
        CreateFile();
    tabs->currentWidget()->setFocus();
}

void MainWindow::CloseFile() {
    CloseFile(tabs->currentIndex());
}

void MainWindow::CloseAllFiles() {
    bool checker = false;
    for (int i = 0; i < tabs->count(); ++i) { // QTabWidget guarantees the consistency of indices?
        if (tabs->tabWhatsThis(i) != "No changes") {
            checker = true;
            break;
        }
    }
    if (checker) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Save all changes", "Save all changes before closing?",
            QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
            SaveAllFiles();
    }
    while (tabs->count() > 0)
        delete tabs->widget(0);

    openned_docs_widget->clear();

    CreateFile();
    tabs->currentWidget()->setFocus();
}

void MainWindow::CloseWindow() {
    CloseAllFiles();
    QApplication::quit();
}

void MainWindow::closeEvent(QCloseEvent*) {
    CloseWindow();
}

void::MainWindow::UpdateParameter() {
    // highlight bad support (always changed)
    QString file = tabs->tabBar()->tabText(tabs->currentIndex());
    QString file_extension = QFileInfo(file).suffix();
    if (!file_extension.isEmpty()) {
        if (highlighter->getRequest(file_extension)) {
            tabs->setTabWhatsThis(tabs->currentIndex(), "Changed");
            return;
        }
    }
    tabs->setTabWhatsThis(tabs->currentIndex(), "Changed");
    disconnect(sender(), SIGNAL(textChanged()), this, SLOT(UpdateParameter()));
}

void MainWindow::FileExplorerHandler() {
    if (file_explorer_dock->isHidden())
        file_explorer_dock->show();
    else
        file_explorer_dock->hide();
}

void MainWindow::OpennedDocsHandler() {
    if (openned_docs_dock->isHidden())
        openned_docs_dock->show();
    else
        openned_docs_dock->hide();
}

void MainWindow::OpenFile(QModelIndex file_index) {
    if (!file_system_model->isDir(file_index))
        OpenFile(file_system_model->filePath(file_index));
}

void MainWindow::DeleteTabFromList(int index) {
    QListWidgetItem* temp_item = openned_docs_widget->takeItem(index);
    delete temp_item;
}

void MainWindow::ChangeTabIndexInList(int old_index, int new_index) {
    QListWidgetItem* first_item  = openned_docs_widget->takeItem(old_index);
    openned_docs_widget->insertItem(new_index, first_item);
}

void MainWindow::UpdateCurrentIndex(QListWidgetItem* current_item) {
    int index = current_item->listWidget()->row(current_item);
    tabs->setCurrentIndex(index);
}

void MainWindow::UpdateCurrentIndex(int new_selection_index) {
    openned_docs_widget->setCurrentRow(new_selection_index);

    // + highlight update
    QString file = tabs->tabBar()->tabText(new_selection_index);
    QString file_extension = QFileInfo(file).suffix();
    if (!file_extension.isEmpty()) {
        if (highlighter->getRequest(file_extension)) {
            highlighter->setDocument(((CodeEditor*)tabs->currentWidget())->document()); // unsafe getting!
            highlighter->highlightBlock(((CodeEditor*)tabs->currentWidget())->toPlainText()); // unsafe getting!
        }
    }
}

void MainWindow::UpdateCurrentIndexOnDelete(int) { // should be better?
    // (Relies on fact that after deletion current tab is always (count() - 1)th tab)
    openned_docs_widget->setCurrentRow(openned_docs_widget->count() - 1);
}

void MainWindow::slotCopy() {
    ((CodeEditor*)tabs->currentWidget())->copy();
}

void MainWindow::slotCut() {
    ((CodeEditor*)tabs->currentWidget())->cut();
}

void MainWindow::slotSelectAll() {
    ((CodeEditor*)tabs->currentWidget())->selectAll();
}

void MainWindow::slotPaste() {
    ((CodeEditor*)tabs->currentWidget())->paste();
}

void MainWindow::slotClear() {
    ((CodeEditor*)tabs->currentWidget())->clear();
}
