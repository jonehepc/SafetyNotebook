#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/QItemSelectionModel>
#include <QtGui/QStandardItemModel>
#include "settings.h"
#include "snoteitem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QAction;

class QComboBox;

class QFontComboBox;

class QTextEdit;

class QTextCharFormat;

class QMenu;

class QPrinter;

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    //~MainWindow();

    void initUi();

    void initSignalAndSlots();

    void setNoteActions(bool e);

    void setEditActions(bool e);

    void setDirActions(bool e);

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

protected:
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

    void onTextEdited(const QString &text = "");

    void onTreeViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void onLineEditTitleEditFinished();

    void setFileModify(bool state);

    void setFileOpened(bool state);

    /* 富文本编译相关 */

    void textBold();

    void textUnderline();

    void textItalic();

    void textStrikeout();

    void textFamily(const QString &f);

    void textSize(const QString &p);

    void textStyle(int styleIndex);

    void textColor();

    void textBackgroundColor();

    void textAlign(QAction *action);

    void setChecked(bool checked);

    void indent();

    void unindent();

    void currentCharFormatChanged(const QTextCharFormat &format);

    void cursorPositionChanged();

    void clipboardDataChanged();

    void printPreview(QPrinter *);

    void zoomIn();

    void zoomOut();

    void updateContent();

private:
    void setTextEditEnableState(bool state);

    void setSaveActions(bool e);

    void setupFileActions();

    void setupEditActions();

    void setupTextActions();

    bool maybeSave();

    void setCurrentFileName(const QString &fileName);

    void modifyIndentation(int amount);

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    void fontChanged(const QFont &f);

    void colorChanged(const QColor &c);

    void bgColorChanged(const QColor &c);

    void alignmentChanged(Qt::Alignment a);

private:
    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;

private:
    Ui::MainWindow *ui;

    Settings *_settings = nullptr;
    QString _file_path;
    bool _file_opened = false;
    //bool _file_new = false;
    bool _file_modified;
    //bool _current_modified = false;
    SNoteItem *_note = nullptr;
    QString _password;
    QStandardItemModel *_model = nullptr;
    //SNoteItem *_current_note = nullptr;
    SNoteItem *_current_item = nullptr;
    bool _current_changed = false;
    bool _do_note_change_edition = false;
    bool _in_refresh = false;
    bool _is_remote_file = false;

};

#endif // MAINWINDOW_H
