#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/QItemSelectionModel>
#include <QtGui/QStandardItemModel>
#include "settings.h"
#include "SNoteItem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void initUi();

    void initSignalAndSlots();

    void setNoteActions(bool e);

    void setEditActions(bool e);

    void setDirActions(bool e);

    void setSaveActions(bool e);

    void displayMessageOnStatusBar(const QString &msg);

    bool load_note();

    void updateTreeView();

    void refreshTreeView();

    bool closeNote();

    void saveNote();

    void init_remote();

    QString getPassword();

    QString getSavePath();

    QString getOpenPath();

    void closeEvent(QCloseEvent *event) override;

public slots:

    void onActionAboutQtTriggered();

    void onActionAboutTriggered();

    void onActionAddTriggered();

    void onActionCloseTriggered();

    void onActionCopyTriggered();

    void onActionCutTriggered();

    void onActionDeleteTriggered();

    void onActionEditTriggered();

    void onActionExitTriggered();

    void onActionGenPasswordTriggered();

    void onActionNewTriggered();

    void onActionOpenTriggered();

    void onActionOpenOnlineTriggered();

    void onActionPasteTriggered();

    void onActionSaveTriggered();

    void onActionSave_asTriggered();

    void onActionSearchTriggered();

    void onActionSettingsTriggered();

    void onActionAddPartitionTriggered();

    void onActionRemovePartitionTriggered();

    void onTextEdited(const QString &text);

    void onTextEdited();

    void onTreeViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void onLineEditTitleEditFinished();

private:
    Ui::MainWindow *ui;

    Settings *_settings = nullptr;
    QString _file_path;
    bool _file_opened = false;
    bool _file_new = false;
    bool _file_modified = false;
    bool _current_modified = false;
    SNoteItem *_note = nullptr;
    QString _password;
    QStandardItemModel *_model = nullptr;
    SNoteItem *_current_note = nullptr;
    SNoteItem *_current_item = nullptr;
    bool _current_changed = false;
    bool _do_note_change_edition = false;
    bool _in_refresh = false;
    bool _is_remote_file = false;

};

#endif // MAINWINDOW_H
