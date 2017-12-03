#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include <QTabWidget>
//#include <QSyntaxHighlighter>
#include <QString>
#include <QMenu>
#include <QMessageBox>
#include <QErrorMessage>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QPrinter>
#include <QDockWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTreeView>
#include <QDirModel>
#include <QModelIndex>
#include <QToolBar>
#include <QTabBar>
#include <QAction>
#include <QToolButton>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QList>
#include <QUrl>

#include "linenumberarea.h"
#include "codeeditor.h"
#include "highlighter.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTabWidget* tabs;
    Highlighter* highlighter;

    QDirModel* file_system_model;
    QTreeView* file_system_view;
    QListWidget* openned_docs_widget;

    QDockWidget* file_explorer_dock;
    QDockWidget* openned_docs_dock;

    void dragEnterEvent(QDragEnterEvent* drag_event) override;
    void dropEvent(QDropEvent* drop_event) override;

    QAction* file_explorer;
    QAction* openned_docs;

    void SetupTabWidget();
    void SetupMenuBar();
    void SetupToolBar();
    void SetupFileExplorerDock();
    void SetupOpennedDocsDock();
    void closeEvent(QCloseEvent*) override;

private slots:
    void UpdateParameter();

    void CreateFile();
    void OpenFile();
    void OpenFile(const QString&);
    void OpenFile(QModelIndex);
    void SaveFile();
    void SaveFileAs();
    void SaveAllFiles();
    void CloseFile();
    void CloseFile(int);
    void CloseAllFiles();
    void CloseWindow();

    void FileExplorerHandler();
    void OpennedDocsHandler();

    void ChangeTabIndexInList(int, int);
    void DeleteTabFromList(int);
    void UpdateCurrentIndex(int);
    void UpdateCurrentIndex(QListWidgetItem*);
    void UpdateCurrentIndexOnDelete(int);

    // TODO: Drag'n Drop support! (from another program + from dock)
};

#endif // MAINWINDOW_H
