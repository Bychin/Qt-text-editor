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
#include <QDebug>

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

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    QTabWidget* tabs;
    Highlighter* highlighter;

    QDirModel* file_system_model;
    QTreeView* file_system_view;
    QListWidget* opened_docs_widget;

    QDockWidget* file_explorer_dock;
    QDockWidget* opened_docs_dock;

    void dragEnterEvent(QDragEnterEvent* drag_event) override;
    void dropEvent(QDropEvent* drop_event) override;


    void SetupTabWidget();
    void SetupMenuBar();
    void SetupToolBar();
    void SetupFileExplorerDock();
    void SetupOpenedDocsDock();
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


    void ChangeTabIndexInList(int, int);
    void DeleteTabFromList(int);
    void UpdateCurrentIndex(int);
    void UpdateCurrentIndex(QListWidgetItem*);
    void UpdateCurrentIndexOnDelete(int);

    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotSelectAll();
    void slotClear();
};





#endif // MAINWINDOW_H
