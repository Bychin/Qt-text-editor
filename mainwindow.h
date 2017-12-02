#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QMenu>
#include <QMessageBox>
#include <QErrorMessage>
#include <QFileDialog>
#include <QDockWidget>
#include <QListView> // ???
#include <QTreeView> // ???
#include <QToolBar>
#include <QTabBar>
#include <QAction>
#include <QToolButton>


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
    QTextEdit* initial_edit;

    QGridLayout* mainLayout;
    QWidget* mainWidget;

    QAction* file_explorer;
    QAction* openned_docs;
    void SetupMenuBar();

private slots:
    void CreateFile();
};

#endif // MAINWINDOW_H
