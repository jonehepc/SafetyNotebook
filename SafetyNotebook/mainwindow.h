#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtWidgets/QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QPrinter;
class QStandardItemModel;
class QItemSelection;
QT_END_NAMESPACE

class Settings;
class SNoteItem;

/**
 * @brief:  安全笔记本主窗体类。
 * */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
    /**
     * @brief:  安全笔记本主窗体类构造函数。
     *
     * @param parent: 父对象。
     * */
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    /**
     * @brief: 窗体关闭事件重载，在关闭时判断是否需要保存笔记。
     *
     * @param event: 关闭事件。
     * */
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

    void textChanged(const QString &text);

    void textChanged1();

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

    /**
     * @brief:  增加缩进。
     * */
    void indent();

    void unindent();

    /**
     * @brief:  文件编辑器当前格式更改。
     *
     * */
    void currentCharFormatChanged(const QTextCharFormat &format);

    /**
     * @brief: 文本编辑器当前光标位置改变时，修改相应控件的状态。
     *
     * */
    void cursorPositionChanged();


    void printPreview(QPrinter *);

    void zoomIn();

    void zoomOut();

    void updateContent();

private:
    /**
     * @brief: 字体对齐方式发生改变时，设置对齐菜单控件状态。
     *
     * @param a: 对齐方式
     *
     * */
    void alignmentChanged(Qt::Alignment a);

    /**
     * @brief:  字体背景颜色改变时，修改字体背景菜单图标的颜色
     *
     * @param c:    颜色
     * */
    void bgColorChanged(const QColor &c);

    /**
     * @brief: 剪贴板内容改变
     *
     * */
    void clipboardDataChanged();

    void setTextEditEnableState(bool state);

    void setSaveActions(bool e);

    void setupFileActions();

    void setupEditActions();

    void setupTextActions();

    bool maybeSave();

    void setCurrentFileName(const QString &fileName);

    void modifyIndentation(int amount);

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    /**
     * @brief:  字体格式改变时，设置格式菜单中相应的控件状态。
     *
     * @param f:    字体格式
     * */
    void fontChanged(const QFont &f);

    /**
     * @brief: 字体颜色发生改变时，设置字体颜色菜单图标的颜色。
     *
     * @param c: 字体颜色
     * */
    void colorChanged(const QColor &c);

    /**
     * @brief: 获取笔记文件路径。
     * */
    QString getOpenPath();

    /**
     * @brief: 获取密码
     * */
    QString getPassword();

    /**
     * @brief: 获取笔记文件保存路径。
     * */
    QString getSavePath();

    void initUi();

    void initSignalAndSlots();

    void setNoteActions(bool e);

    void setEditActions(bool e);

    void setDirActions(bool e);

    /**
     * @brief:  在状态栏上显示消息
     *
     * @param msg:  消息内容
     * */
    void displayMessageOnStatusBar(const QString &msg);

    bool load_note();

    void updateTreeView();

    void refreshTreeView();

    /**
     * @brief 关闭笔记本，如果内容有更改将提示保存。
     *
     * @return: 是否已关闭，如果选择取消则返回False，其它两项返回True。
     * */
    bool closeNote();

    void saveNote();

    /**
     * @brief: 初始化云端笔记。
     * */
    void init_remote();

private:
    QComboBox *comboStyle{};
    QFontComboBox *comboFont{};
    QComboBox *comboSize{};
    Ui::MainWindow *ui;
    Settings *_settings = nullptr;
    QString _file_path;
    bool _file_opened = false;
    bool _file_modified;
    SNoteItem *_note = nullptr;
    QString _password;
    QStandardItemModel *_model = nullptr;
    SNoteItem *_current_item = nullptr;
    bool _current_changed = false;
    bool _do_note_change_edition = false;
    bool _in_refresh = false;
    bool _is_remote_file = false;

};

#endif // MAIN_WINDOW_H
