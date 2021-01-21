#include <exception>
#include <QtCore/QDebug>
#include <QtGui/QBitmap>
#include <QtGui/QClipboard>
#include <QtGui/QCloseEvent>
#include <QtGui/QTextList>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loader.h"
#include "newdirdialog.h"
#include "passworddialog.h"
#include "PasswordGenerateDialog.h"
#include "settings.h"
#include "settingsdialog.h"
#include "snoteitem.h"


/**
 * @brief:  安全笔记本主窗体类构造函数。
 *
 * @param parent: 父对象。
 * */
MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
          ui(new Ui::MainWindow),
          _file_modified(false),
          _model(new QStandardItemModel()),
          _settings(new Settings()) {

    ui->setupUi((QMainWindow *) this);
    _settings->loadSettings();
    initUi();
    setupEditActions();
    setupFileActions();
    setupTextActions();
    initSignalAndSlots();
    updateTreeView();
}

/**
 * @brief: 字体对齐方式发生改变时，设置对齐菜单控件状态。
 *
 * @param a: 对齐方式
 *
 * */
void MainWindow::alignmentChanged(Qt::Alignment a) {
    if (a & Qt::AlignLeft)
        ui->actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        ui->actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        ui->actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        ui->actionAlignJustify->setChecked(true);
}

/**
 * @brief:  字体背景颜色改变时，修改字体背景菜单图标的颜色
 *
 * @param c:    颜色
 * */
void MainWindow::bgColorChanged(const QColor &c) {
    QPixmap px(QString::fromUtf8(":/png/format-font-color-front.png"));
    QPixmap pxr(px.size());
    pxr.fill(c);
    pxr.setMask(px.createMaskFromColor(Qt::transparent));

    ui->actionTextBackgroundColor->setIcon(pxr);
}

/**
 * @brief: 剪贴板内容改变
 *
 * */
void MainWindow::clipboardDataChanged() {

}

/**
 * @brief: 窗体关闭事件重载，在关闭时判断是否需要保存笔记。
 *
 * @param event: 关闭事件。
 * */
void MainWindow::closeEvent(QCloseEvent *event) {
    if (_note != nullptr) {
        if (!closeNote()) {
            event->ignore();
        }
    }
    _settings->saveSettings();
}

/**
 * @brief 关闭笔记本，如果内容有更改将提示保存。
 *
 * @return: 是否已关闭，如果选择取消则返回False，其它两项返回True。
 * */
bool MainWindow::closeNote() {
    if (_file_modified) {
        auto button = QMessageBox::warning(this, QStringLiteral("是否保存笔记"),
                                           QStringLiteral("笔记内容已经更改，是否在关闭前保存？"),
                                           QMessageBox::Yes | QMessageBox::No |
                                           QMessageBox::Cancel);
        switch (button) {
            case QMessageBox::Yes:
                onActionSaveTriggered();
                break;
            case QMessageBox::No:
                break;
//            case QMessageBox::Cancel:
//                return false;
            default:
                return false;
        }
    }
    setWindowTitle(QStringLiteral("安全笔记本"));
    return true;
}

/**
 * @brief: 字体颜色发生改变时，设置字体颜色菜单图标的颜色。
 *
 * @param c: 字体颜色
 * */
void MainWindow::colorChanged(const QColor &c) {
    QPixmap px(QString::fromUtf8(":/png/format-font-color.png"));
    QImage img = px.toImage();
    for (int x = 37; x < 475; x++) {
        for (int y = 390; y < 512; y++) {
            img.setPixelColor(x, y, c);
        }
    }

    ui->actionTextColor->setIcon(QPixmap::fromImage(img));

}

/**
 * @brief:  文件编辑器当前格式更改。
 *
 * */
void MainWindow::currentCharFormatChanged(const QTextCharFormat &format) {
    fontChanged(format.font());
    colorChanged(format.foreground().color());
    bgColorChanged(format.background().color());
}

/**
 * @brief: 文本编辑器当前光标位置改变时，修改相应控件的状态。
 *
 * */
void MainWindow::cursorPositionChanged() {
    alignmentChanged(ui->textEdit->alignment());
    QTextList *list = ui->textEdit->textCursor().currentList();
    if (list) {
        switch (list->format().style()) {
            case QTextListFormat::ListDisc:
                comboStyle->setCurrentIndex(1);
                break;
            case QTextListFormat::ListCircle:
                comboStyle->setCurrentIndex(2);
                break;
            case QTextListFormat::ListSquare:
                comboStyle->setCurrentIndex(3);
                break;
            case QTextListFormat::ListDecimal:
                comboStyle->setCurrentIndex(6);
                break;
            case QTextListFormat::ListLowerAlpha:
                comboStyle->setCurrentIndex(7);
                break;
            case QTextListFormat::ListUpperAlpha:
                comboStyle->setCurrentIndex(8);
                break;
            case QTextListFormat::ListLowerRoman:
                comboStyle->setCurrentIndex(9);
                break;
            case QTextListFormat::ListUpperRoman:
                comboStyle->setCurrentIndex(10);
                break;
            default:
                comboStyle->setCurrentIndex(-1);
                break;
        }
        switch (ui->textEdit->textCursor().block().blockFormat().marker()) {
            case QTextBlockFormat::MarkerType::NoMarker:
                ui->actionToggleCheckState->setChecked(false);
                break;
            case QTextBlockFormat::MarkerType::Unchecked:
                comboStyle->setCurrentIndex(4);
                ui->actionToggleCheckState->setChecked(false);
                break;
            case QTextBlockFormat::MarkerType::Checked:
                comboStyle->setCurrentIndex(5);
                ui->actionToggleCheckState->setChecked(true);
                break;
        }
    } else {
        int headingLevel = ui->textEdit->textCursor().blockFormat().headingLevel();
        comboStyle->setCurrentIndex(headingLevel ? headingLevel + 10 : 0);
    }
}

/**
 * @brief:  在状态栏上显示消息
 *
 * @param msg:  消息内容
 * */
void MainWindow::displayMessageOnStatusBar(const QString &msg) {
    ui->statusBar->showMessage(msg, 3000);
}

/**
 * @brief:  字体格式改变时，设置格式菜单中相应的控件状态。
 * @param f:    字体格式
 *
 * */
void MainWindow::fontChanged(const QFont &f) {
    comboFont->setCurrentIndex(comboFont->findText(f.family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    ui->actionTextBold->setChecked(f.bold());
    ui->actionTextItalic->setChecked(f.italic());
    ui->actionTextUnderline->setChecked(f.underline());
}

/**
 * @brief: 获取笔记文件路径。
 * */
QString MainWindow::getOpenPath() {
    auto filePath = QFileDialog::getOpenFileName(this, QStringLiteral("打开笔记"), _settings->lastNotePath(),
                                                 QStringLiteral("笔记文件 (*.secnote);;所有文件 (*)"));
    if (!filePath.isNull()) {
        QFileInfo info(filePath);
        _settings->setLastNotePath(info.absoluteDir().path());
        return filePath;
    }
    return QString();
}

/**
 * @brief: 获取密码
 * */
QString MainWindow::getPassword() {
    PasswordDialog passwordDialog(this);
    if (passwordDialog.exec() == QDialog::Accepted)
        return passwordDialog.getPassword();
    return QString();
}

/**
 * @brief: 获取笔记文件保存路径。
 * */
QString MainWindow::getSavePath() {
    auto filePath = QFileDialog::getSaveFileName(this, QStringLiteral("保存笔记本"), _settings->lastNotePath(),
                                                 QStringLiteral("笔记文件 (*.secnote);;所有文件 (*)"));
    if (!filePath.isNull()) {
        QFileInfo info(filePath);
        _settings->setLastNotePath(info.absoluteDir().path());
        return filePath;
    }
    return QString();
}

/**
 * @brief:  增加缩进。
 * */
void MainWindow::indent() {
    modifyIndentation(1);
}

/**
 * @brief: 初始化云端笔记。
 * */
void MainWindow::init_remote() {

}

/**
 * @brief: 关联信号和处理槽。
 * */
void MainWindow::initSignalAndSlots() {
    connect(this, SIGNAL(fileModifyChanged(bool)), this, SLOT(onFileModifyChanged(bool)));

    connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(onActionAboutQtTriggered()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(onActionAboutTriggered()));
    connect(ui->actionAdd, SIGNAL(triggered()), this, SLOT(onActionAddTriggered()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(onActionCloseTriggered()));
    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(onActionCopyTriggered()));
    connect(ui->actionCut, SIGNAL(triggered()), this, SLOT(onActionCutTriggered()));
    connect(ui->actionDelete, SIGNAL(triggered()), this, SLOT(onActionDeleteTriggered()));
    connect(ui->actionEdit, SIGNAL(triggered()), this, SLOT(onActionEditTriggered()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(onActionExitTriggered()));
    connect(ui->actionGenPassword, SIGNAL(triggered()), this, SLOT(onActionGenPasswordTriggered()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(onActionNewTriggered()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onActionOpenTriggered()));
    connect(ui->actionOpenOnline, SIGNAL(triggered()), this, SLOT(onActionOpenOnlineTriggered()));
    connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(onActionPasteTriggered()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onActionSaveTriggered()));
    connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(onActionSave_asTriggered()));
    connect(ui->actionSearch, SIGNAL(triggered()), this, SLOT(onActionSearchTriggered()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(onActionSettingsTriggered()));
    connect(ui->actionAddPartition, SIGNAL(triggered()), this, SLOT(onActionAddPartitionTriggered()));
    connect(ui->actionRemovePartition, SIGNAL(triggered()), this, SLOT(onActionRemovePartitionTriggered()));

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextEdited()));
    connect(ui->lineEditTitle, SIGNAL(textEdited(const QString &)), this, SLOT(onTextEdited(const QString&)));

    auto selectionModel = ui->treeView->selectionModel();
    connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(onTreeViewSelectionChanged(const QItemSelection &, const QItemSelection &)));

}

/**
 * @brief: 初始化界面。
 * */
void MainWindow::initUi() {
    setToolButtonStyle(Qt::ToolButtonFollowStyle);

    ui->lineEditTitle->setEnabled(false);
    setTextEditEnableState(false);
    ui->treeView->setModel(_model);

    setNoteActions(false);
    setDirActions(false);
    setEditActions(false);
    setSaveActions(false);

    ui->actionOpenOnline->setEnabled(_settings->remoteSettings().state);
}

/**
 * @brief: 加载笔记内容
 * */
bool MainWindow::load_note() {
    if (_note != nullptr) {
        _model->clear();
        _note = nullptr;
        updateTreeView();
    }
    _note = load_item_from_path(_file_path, _password);
    if (_note == nullptr) {
        displayMessageOnStatusBar(QStringLiteral("加载笔记失败。"));
        return false;
    }
    _model->appendRow(_note);
    refreshTreeView();
    return true;
}

bool MainWindow::maybeSave() {
    return false;
}

/**
 * @brief:  更改选择的内容或当前单词的格式
 *
 * @param format: 指定的格式
 * */
void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format) {
    QTextCursor cursor = ui->textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
    ui->textEdit->setFocus(Qt::TabFocusReason);
}

/**
 * @brief: 更改缩进
 *
 * @param amount: 缩进值
 * */
void MainWindow::modifyIndentation(int amount) {
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.beginEditBlock();
    if (cursor.currentList()) {
        QTextListFormat listFmt = cursor.currentList()->format();
        // See whether the line above is the list we want to move this item into,
        // or whether we need a new list.
        QTextCursor above(cursor);
        above.movePosition(QTextCursor::Up);
        if (above.currentList() && listFmt.indent() + amount == above.currentList()->format().indent()) {
            above.currentList()->add(cursor.block());
        } else {
            listFmt.setIndent(listFmt.indent() + amount);
            cursor.createList(listFmt);
        }
    } else {
        QTextBlockFormat blockFmt = cursor.blockFormat();
        blockFmt.setIndent(blockFmt.indent() + amount);
        cursor.setBlockFormat(blockFmt);
    }
    cursor.endEditBlock();
}

/**
 * @brief:  显示关于QT窗体
 * */
void MainWindow::onActionAboutQtTriggered() {
    QMessageBox::aboutQt(this);
}

/**
 * @brief: 显示关于窗体。
 * */
void MainWindow::onActionAboutTriggered() {
    auto about_info = QStringLiteral("<html><head/><body><p>安全笔记本，是一个AES加密保护的笔记本类工具。"
                                     "<br/>"
                                     "<br/>主要功能有："
                                     "<br/>&nbsp;&nbsp;&nbsp;&nbsp;笔记管理：分区、分主题进行管理。"
                                     "<br/>&nbsp;&nbsp;&nbsp;&nbsp;密码生成：生成指定长度的密钥。"
                                     "<br/>版本: 0.1.2"
                                     "<br/>授权: <span><a href=\"https://www.gnu.org/licenses/lgpl.html\">GNU 宽通用公共许可证</a><br/>"
                                     "</span>联系: <a href=\"mailto:jonehepc@hotmail.com\">jonehepc@hotmail.com</a>"
                                     "&#169; 2020 版权所有</p></body></html>");
    QMessageBox::about(this, QStringLiteral("关于安全笔记本"),
                       about_info);
}

/**
 * @brief: 添加笔记分区。
 * */
void MainWindow::onActionAddPartitionTriggered() {
    NewDirDialog newDirDialog(this);
    if (newDirDialog.exec() == QDialog::Accepted) {
        if (!newDirDialog.getDirName().isEmpty()) {
            auto noteItem = new SNoteItem(QUuid::createUuid(), newDirDialog.getDirName(), QStringLiteral(""),
                                          SNoteItemType::Partition);
            if (_current_item == nullptr) {
                _note->appendRow(noteItem);
            } else {
                _current_item->appendRow(noteItem);
            }
            _current_item = noteItem;
            ui->lineEditTitle->setText(_current_item->title());
            ui->textEdit->setText(QStringLiteral(""));
            ui->lineEditTitle->setEnabled(true);
            setTextEditEnableState(false);
            refreshTreeView();
            ui->treeView->clearSelection();
            ui->treeView->setCurrentIndex(_model->indexFromItem(_current_item));
            ui->lineEditTitle->setFocus();
        }
    }
}

/**
 * @brief: 添加笔记
 * */
void MainWindow::onActionAddTriggered() {
    _do_note_change_edition = true;
    _in_refresh = true;
    ui->lineEditTitle->setText(QStringLiteral(""));
    ui->textEdit->setText(QStringLiteral(""));
    ui->lineEditTitle->setEnabled(true);
    setTextEditEnableState(true);
    auto noteItem = new SNoteItem(QUuid::createUuid(), QStringLiteral("笔记标题"), QStringLiteral(""),
                                  SNoteItemType::Note);
    _current_item->appendRow(noteItem);
    _current_item = noteItem;
    setFileModify(true);
    _do_note_change_edition = false;
    _in_refresh = false;
    refreshTreeView();
    ui->treeView->clearSelection();
    ui->treeView->setCurrentIndex(_model->indexFromItem(_current_item));
    ui->lineEditTitle->setFocus();
}

/**
 * @brief: 关闭笔记本
 * */
void MainWindow::onActionCloseTriggered() {
    if (_note != nullptr) {
        if (!closeNote())
            return;
        setEditActions(false);
        setNoteActions(false);
        setDirActions(false);
        setSaveActions(false);
        ui->lineEditTitle->setText("");
        ui->lineEditTitle->setEnabled(false);
        ui->textEdit->setPlainText("");
        setTextEditEnableState(false);
        _model->clear();
        _current_item = nullptr;
        refreshTreeView();
        _file_path = "";
        setFileOpened(false);
        setFileModify(false);
        setWindowTitle(QStringLiteral("安全笔记本"));
        _current_changed = false;
        _note = nullptr;
        displayMessageOnStatusBar(QStringLiteral("关闭笔记完成。"));
    }

}

/**
 * @brief: 复制
 * */
void MainWindow::onActionCopyTriggered() {

}

/**
 * @brief:  剪切
 * */
void MainWindow::onActionCutTriggered() {

}

/**
 * @brief:  删除笔记或分区
 * */
void MainWindow::onActionDeleteTriggered() {
    if (_current_item != nullptr && _current_item->sNoteItemType() == SNoteItemType::Note) {
        if (!_current_item->content().isEmpty()) {
            if (QMessageBox::warning(this, QStringLiteral("警告"),
                                     QStringLiteral("正在删除的笔记中有内容，是否要继续删除？"),
                                     QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                return;
        }
        auto parent = _current_item->parent();
        if (parent != nullptr) {
            auto index = _model->indexFromItem(_current_item);
            if (index.isValid() && index.row() >= 0) {
                auto r = index.row();
                auto c = (SNoteItem *) parent->child(r);
                if (_current_item == c) {
                    parent->removeRow(r);
                    setFileModify(true);
                    refreshTreeView();
                }
            }
        }
    }
}

/**
 * @brief: 编辑笔记
 * */
void MainWindow::onActionEditTriggered() {

}

/**
 * @brief: 退出本窗体
 * */
void MainWindow::onActionExitTriggered() {
    close();
}

/**
 * @brief: 密码生成工具。
 * */
void MainWindow::onActionGenPasswordTriggered() {
    PasswordGenerateDialog dialog(this);
    dialog.exec();
}

/**
 * @brief: 新建笔记本。
 * */
void MainWindow::onActionNewTriggered() {
    onActionCloseTriggered();
    setEditActions(false);
    setNoteActions(true);
    setDirActions(true);
    //_model->setupModelData(new NoteItem());
    refreshTreeView();
    ui->lineEditTitle->setText("");
    ui->textEdit->setPlainText("");
    setTextEditEnableState(false);
    _file_path = "";
    _note = SNoteItem::makeRootItem();
    updateTreeView();
    //_current_note = nullptr;
    _current_item = nullptr;
    setWindowTitle(QStringLiteral("安全笔记本 - [NewFile]"));
    displayMessageOnStatusBar(QStringLiteral("新建笔记本完成。"));
    setFileModify(true);
    setSaveActions(true);
}

/**
 * @brief: 打开云端笔记本。
 * */
void MainWindow::onActionOpenOnlineTriggered() {

}

/**
 * @brief: 打开笔记本文件。
 * */
void MainWindow::onActionOpenTriggered() {
    if (_note != nullptr) {
        if (!closeNote()) {
            return;
        }
    }
    auto file_path = getOpenPath();
    if (file_path.isNull() || file_path.isEmpty())
        return;
    auto password = getPassword();
    if (password.isNull() || password.isEmpty())
        return;
    _file_path = file_path;
    _password = password;
    if (load_note()) {
        _in_refresh = false;
        _do_note_change_edition = false;
        _current_changed = false;
        //_file_opened = true;
        //_file_new = false;
        setFileModify(false);
        setFileOpened(true);
        //_current_modified = false;
        setSaveActions(false);
        setNoteActions(true);
        setDirActions(true);
        displayMessageOnStatusBar(QStringLiteral("打开笔记完成"));
        setWindowTitle(QStringLiteral("安全笔记本 - [") + _file_path + QString("]"));
    }

}

/**
 * @brief: 粘贴
 * */
void MainWindow::onActionPasteTriggered() {

}

/**
 * @brief: 删除笔记分区。
 * */
void MainWindow::onActionRemovePartitionTriggered() {
    if (_current_item != nullptr && _current_item->sNoteItemType() == SNoteItemType::Partition) {
        auto parent = _current_item->parent();
        if (parent != nullptr) {
            auto index = _model->indexFromItem(_current_item);
            auto item = (SNoteItem *) parent->child(index.row());
            if (item == _current_item) {
                if (_current_item->hasChildren()) {
                    if (QMessageBox::warning(this, QStringLiteral("警告"),
                                             QString("\"") + _current_item->title() +
                                             QStringLiteral("\"分区中还有笔记或子分区，如果删除本分区，本分区下所有内容都将删除。是否继续删除？"),
                                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
                        return;
                    }
                }
                parent->removeRow(index.row());
                setFileModify(true);
                refreshTreeView();
            }
        }
    }
}

/**
 * @brief: 笔记本另存为。
 * */
void MainWindow::onActionSave_asTriggered() {
    auto file_path = getSavePath();
    if (file_path.isEmpty())
        return;
    auto password = getPassword();
    if (password.isEmpty())
        return;
    _file_path = file_path;
    _password = password;
    saveNote();
    setFileModify(false);
    //_file_new = false;
    ui->actionSave->setEnabled(false);
    ui->actionSave_as->setEnabled(false);
    setWindowTitle(QStringLiteral("安全笔记本 - [") + _file_path + QString("]"));
    displayMessageOnStatusBar(QStringLiteral("笔记另存为成功。"));
}

/**
 * @brief: 保存笔记本。
 * */
void MainWindow::onActionSaveTriggered() {
    if (!_file_modified) {
        return;
    }
    if (_file_path.isEmpty()) {
        auto file_path = getSavePath();
        if (file_path.isEmpty()) {
            displayMessageOnStatusBar(QStringLiteral("保存路径必须设置。"));
            return;
        }
        _file_path = file_path;
    }
    if (_password.isEmpty()) {
        auto password = getPassword();
        if (password.isEmpty()) {
            displayMessageOnStatusBar(QStringLiteral("保存密码必须设置。"));
            return;
        }
        _password = password;
    }
    saveNote();
    setFileOpened(true);
    setFileModify(false);
    _current_changed = false;
    //_current_modified = false;
    ui->actionSave->setEnabled(false);
    ui->actionSave_as->setEnabled(false);
}

/**
 * @brief: 搜索笔记内容。
 * */
void MainWindow::onActionSearchTriggered() {

}

/**
 * @brief: 打开设置窗体。
 * */
void MainWindow::onActionSettingsTriggered() {
    SettingsDialog settingsDialog(this);
    settingsDialog.setRemoteSettings(_settings->remoteSettings());
    if (settingsDialog.exec() == QDialog::Accepted) {
        _settings->setRemoteSettings(settingsDialog.getRemoteSettings());
        _settings->saveSettings();
    }
}

/**
 * @brief:  笔记标题编辑完成。
 * */
void MainWindow::onLineEditTitleEditFinished() {
    if (_current_item == nullptr || _do_note_change_edition)
        return;

    auto need_refresh = false;

    if (ui->lineEditTitle->text() != _current_item->title()) {
        need_refresh = true;
        _current_item->setTitle(ui->lineEditTitle->text());
        setFileModify(true);
    }

    if (_current_item->sNoteItemType() == SNoteItemType::Note) {
        if (ui->textEdit->document()->isModified()) {
            //_current_item->setContent(ui->textEdit->toHtml());
            setFileModify(true);
        }
    }

    if (need_refresh)
        refreshTreeView();
}

/**
 * @brief: 文本编辑控件的文本内容发生改变。
 *
 * @param text: 文本内容。
 * */
void MainWindow::onTextEdited(const QString &text) {
    if (_current_item != nullptr && !_do_note_change_edition)
        onLineEditTitleEditFinished();
}

/**
 * @brief: 笔记树选择变化。
 *
 * @param selected: 选择的笔记树
 * @param deselected: 取消选择的笔记树。
 * */
void MainWindow::onTreeViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    updateContent();
    if (!selected.isEmpty()) {
        if (_model != selected.indexes()[0].model()) {
            return;
        }
        auto note = (SNoteItem *) _model->itemFromIndex(selected.indexes()[0]);
        if (note == nullptr)
            return;
        _current_item = note;
        _do_note_change_edition = true;
        ui->lineEditTitle->setEnabled(true);
        ui->lineEditTitle->setText(note->title());
        switch (note->sNoteItemType()) {
            case SNoteItemType::Partition:
                setEditActions(false);
                setDirActions(true);
                ui->actionAdd->setEnabled(true);
                ui->textEdit->setText(QStringLiteral(""));
                setTextEditEnableState(false);
                if (_current_item->parent() == nullptr) {
                    ui->actionRemovePartition->setEnabled(false);
                    ui->lineEditTitle->setEnabled(false);
                }
                break;
            case SNoteItemType::Note:
                setEditActions(true);
                ui->actionAdd->setEnabled(false);
                setDirActions(false);
                ui->textEdit->setHtml(_current_item->content());
                setTextEditEnableState(true);
                break;
            default:
                break;
        }

        _do_note_change_edition = false;

        if (_in_refresh)
            return;

    }
}

/**
 * @brief:  打印预览
 *
 * */
void MainWindow::printPreview(QPrinter *) {

}

/**
 * @brief: 刷新笔记树控件显示。
 * */
void MainWindow::refreshTreeView() {
    _in_refresh = true;
    emit _model->layoutChanged();
    ui->treeView->expandAll();
    if (_current_item != nullptr) {
        ui->treeView->selectionModel()->setCurrentIndex(_model->indexFromItem(_current_item),
                                                        QItemSelectionModel::Select);
    }
    _in_refresh = false;
}

/**
 * 保存笔记内容。
 * */
void MainWindow::saveNote() {
    onLineEditTitleEditFinished();
    updateContent();
    save_items_to_path(_file_path, _password, *_note);
    displayMessageOnStatusBar(QStringLiteral("保存笔记成功"));
}

/**
 * @brief:  设置文本的样式为选择框。
 * */
void MainWindow::setChecked(bool checked) {
    textStyle(checked ? 5 : 4);
}

void MainWindow::setCurrentFileName(const QString &fileName) {

}

/**
 * @brief:  设置笔记分区控件的启用状态
 *
 * @param e: 启用状态
 * */
void MainWindow::setDirActions(bool e) {
    ui->actionAddPartition->setEnabled(e);
    ui->actionRemovePartition->setEnabled(e);
}

/**
 * @brief: 设置编辑控件的启用状态
 *
 * @param e: 状态
 * */
void MainWindow::setEditActions(bool e) {
    ui->actionAdd->setEnabled(e);
    ui->actionDelete->setEnabled(e);
    ui->actionEdit->setEnabled(e);
}

/**
 * @brief: 设置文件修改标志
 *
 * @param state:    修改标志
 *
 * 会同时更改文件修改状态相关的Action标志。
 * */
void MainWindow::setFileModify(bool state) {
    _file_modified = state;
    ui->actionSave->setEnabled(state);
    if (_file_opened)
        setWindowTitle(QStringLiteral("安全笔记本 - [") + _file_path + QString(state ? "] *" : "]"));
}

/**
 * @brief: 设置文件打开或新建标志。
 *
 * @param state: 文件标志
 * */
void MainWindow::setFileOpened(bool state) {
    _file_opened = state;
    ui->actionClose->setEnabled(state);
}

/**
 * @brief: 设置笔记内容的启用状态
 *
 * @param e: 状态
 * */
void MainWindow::setNoteActions(bool e) {
    ui->treeView->setEnabled(e);
}

/**
 * @brief:  设置保存类的Action状态
 *
 * @param e:    状态
 * */
void MainWindow::setSaveActions(bool e) {
    ui->actionSave->setEnabled(e);
    ui->actionSave_as->setEnabled(e);
    //ui->actionClose->setEnabled(e);
}

/**
 * @brief:  设置文本编辑控件与相应的格式菜单是否启用
 *
 * @param state:    启用状态
 * */
void MainWindow::setTextEditEnableState(bool state) {
    ui->textEdit->setEnabled(state);
    ui->toolBarRichEditor->setEnabled(state);
    ui->toolBarFont->setEnabled(state);
}

/**
 * @brief: 设置编辑相关的控件
 *
 * */
void MainWindow::setupEditActions() {
    connect(ui->actionUndo, &QAction::triggered, ui->textEdit, &MTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->textEdit, &MTextEdit::redo);
#ifndef QT_NO_CLIPBOARD
    connect(ui->actionCopy, &QAction::triggered, ui->textEdit, &MTextEdit::copy);
    connect(ui->actionCut, &QAction::triggered, ui->textEdit, &MTextEdit::cut);
    connect(ui->actionPaste, &QAction::triggered, ui->textEdit, &MTextEdit::paste);
#endif
}

/**
 * @brief: 设置文件相关的控件
 *
 * */
void MainWindow::setupFileActions() {

}

/**
 * @brief: 设置字体相关的控件
 *
 * */
void MainWindow::setupTextActions() {
    ui->textEdit->setTextBackgroundColor(Qt::white);
    ui->textEdit->setTextColor(Qt::black);

    connect(ui->textEdit, &QTextEdit::currentCharFormatChanged, this, &MainWindow::currentCharFormatChanged);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::cursorPositionChanged);
    connect(ui->textEdit->document(), &QTextDocument::modificationChanged, ui->actionSave, &QAction::setEnabled);
    connect(ui->textEdit->document(), &QTextDocument::modificationChanged, this, &QWidget::setWindowModified);
    connect(ui->textEdit->document(), &QTextDocument::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(ui->textEdit->document(), &QTextDocument::redoAvailable, ui->actionRedo, &QAction::setEnabled);


#ifndef QT_NO_CLIPBOARD
    ui->actionCut->setEnabled(false);
    connect(ui->textEdit, &QTextEdit::copyAvailable, ui->actionCut, &QAction::setEnabled);
    ui->actionCopy->setEnabled(false);
    connect(ui->textEdit, &QTextEdit::copyAvailable, ui->actionCopy, &QAction::setEnabled);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::clipboardDataChanged);
#endif

    //connect(ui->actionTextBold, SIGNAL(triggered(bool)), this, SLOT(textBold()));
    connect(ui->actionTextBold, &QAction::triggered, this, &MainWindow::textBold);
    connect(ui->actionTextItalic, SIGNAL(triggered(bool)), this, SLOT(textItalic()));
    connect(ui->actionTextUnderline, SIGNAL(triggered(bool)), this, SLOT(textUnderline()));
    connect(ui->actionStrikeout, SIGNAL(triggered(bool)), this, SLOT(textStrikeout()));
    connect(ui->actionIndentLess, SIGNAL(triggered(bool)), this, SLOT(indent()));
    connect(ui->actionIndentMore, SIGNAL(triggered(bool)), this, SLOT(unident()));

    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, SIGNAL(triggered(QAction * )), this, SLOT(textAlign(QAction * )));
    if (QApplication::isLeftToRight()) {
        alignGroup->addAction(ui->actionAlignLeft);
        alignGroup->addAction(ui->actionAlignCenter);
        alignGroup->addAction(ui->actionAlignRight);
    } else {
        alignGroup->addAction(ui->actionAlignRight);
        alignGroup->addAction(ui->actionAlignCenter);
        alignGroup->addAction(ui->actionAlignLeft);
    }
    alignGroup->addAction(ui->actionAlignJustify);

    connect(ui->actionTextColor, &QAction::triggered, this, &MainWindow::textColor);
    connect(ui->actionTextBackgroundColor, &QAction::triggered, this, &MainWindow::textBackgroundColor);
    connect(ui->actionToggleCheckState, &QAction::triggered, this, &MainWindow::setChecked);

    comboStyle = new QComboBox(ui->toolBarFont);
    ui->toolBarFont->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Task List (Unchecked)");
    comboStyle->addItem("Task List (Checked)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    comboStyle->addItem("Heading 1");
    comboStyle->addItem("Heading 2");
    comboStyle->addItem("Heading 3");
    comboStyle->addItem("Heading 4");
    comboStyle->addItem("Heading 5");
    comboStyle->addItem("Heading 6");

    connect(comboStyle, SIGNAL(activated(int)), this, SLOT(textStyle(int)));

    comboFont = new QFontComboBox(ui->toolBarFont);
    ui->toolBarFont->addWidget(comboFont);

    connect(comboFont, &QFontComboBox::textActivated, this, &MainWindow::textFamily);

    comboSize = new QComboBox(ui->toolBarFont);
    comboSize->setObjectName("comboSize");
    ui->toolBarFont->addWidget(comboSize);
    comboSize->setEditable(true);
    const QList<int> standardSizes = QFontDatabase::standardSizes();
    for (int size : standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(ui->actionZoomIn, &QAction::triggered, this, &MainWindow::zoomIn);
    connect(ui->actionZoomOut, &QAction::triggered, this, &MainWindow::zoomOut);
    connect(comboSize, &QComboBox::textActivated, this, &MainWindow::textSize);

    //connect(ui->textEdit, &MTextEdit::focusOut, this, &MainWindow::updateContent);
}

/**
 * @brief:  更改文本对齐方式。
 * @param action: 对齐方式控件。
 *
 * */
void MainWindow::textAlign(QAction *action) {
    if (action == ui->actionAlignLeft)
        ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (action == ui->actionAlignCenter)
        ui->textEdit->setAlignment(Qt::AlignHCenter);
    else if (action == ui->actionAlignRight)
        ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (action == ui->actionAlignJustify)
        ui->textEdit->setAlignment(Qt::AlignJustify);
}

/**
 * @brief:  更改字体背景颜色。
 *
 * */
void MainWindow::textBackgroundColor() {
    QColor col = QColorDialog::getColor(ui->textEdit->textBackgroundColor(), this);
    QTextCursor cursor = ui->textEdit->textCursor();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    QTextCharFormat fmt = cursor.charFormat();
    if (col.isValid()) {
        fmt.setBackground(col);
    } else {
        fmt.clearBackground();
    }
    cursor.setCharFormat(fmt);
    ui->textEdit->setCurrentCharFormat(fmt);
    bgColorChanged(col);
}

/**
 * @brief: 设置字体格式为加粗。
 * */
void MainWindow::textBold() {
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

/**
 * @brief: 更改字体颜色。
 * */
void MainWindow::textColor() {
    QColor col = QColorDialog::getColor(ui->textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

/**
 * @brief:  更改字体
 * @param f: 字体名称
 *
 * */
void MainWindow::textFamily(const QString &f) {
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.beginEditBlock();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::BlockUnderCursor);
    }
    QTextCharFormat fmt;
    fmt = cursor.charFormat();
    fmt.setFontFamily(f);
    cursor.setCharFormat(fmt);
    ui->textEdit->setCurrentCharFormat(fmt);
    cursor.endEditBlock();
}

/**
 * @brief: 设置字体格式为斜体。
 * */
void MainWindow::textItalic() {
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

/**
 * @brief:  更改字体大小。
 * @param p: 字体大小
 * */
void MainWindow::textSize(const QString &p) {
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

/**
 * @brief:  设置字体删除线。
 *
 * */
void MainWindow::textStrikeout() {
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(ui->actionStrikeout->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

/**
 * @brief:  更改字体的样式。
 * @param styleIndex: 样式列表中的序号。
 * */
void MainWindow::textStyle(int styleIndex) {
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;
    QTextBlockFormat::MarkerType marker = QTextBlockFormat::MarkerType::NoMarker;

    switch (styleIndex) {
        case 1:
            style = QTextListFormat::ListDisc;
            break;
        case 2:
            style = QTextListFormat::ListCircle;
            break;
        case 3:
            style = QTextListFormat::ListSquare;
            break;
        case 4:
            if (cursor.currentList())
                style = cursor.currentList()->format().style();
            else
                style = QTextListFormat::ListDisc;
            marker = QTextBlockFormat::MarkerType::Unchecked;
            break;
        case 5:
            if (cursor.currentList())
                style = cursor.currentList()->format().style();
            else
                style = QTextListFormat::ListDisc;
            marker = QTextBlockFormat::MarkerType::Checked;
            break;
        case 6:
            style = QTextListFormat::ListDecimal;
            break;
        case 7:
            style = QTextListFormat::ListLowerAlpha;
            break;
        case 8:
            style = QTextListFormat::ListUpperAlpha;
            break;
        case 9:
            style = QTextListFormat::ListLowerRoman;
            break;
        case 10:
            style = QTextListFormat::ListUpperRoman;
            break;
        default:
            break;
    }

    cursor.beginEditBlock();

    QTextBlockFormat blockFmt = cursor.blockFormat();

    if (style == QTextListFormat::ListStyleUndefined) {
        blockFmt.setObjectIndex(-1);
        int headingLevel = styleIndex >= 11 ? styleIndex - 11 + 1 : 0; // H1 to H6, or Standard
        blockFmt.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(blockFmt);

        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0; // H1 to H6: +3 to -2
        QTextCharFormat fmt;
        fmt.setFontWeight(headingLevel ? QFont::Bold : QFont::Normal);
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fmt);
        ui->textEdit->mergeCurrentCharFormat(fmt);
    } else {
        blockFmt.setMarker(marker);
        cursor.setBlockFormat(blockFmt);
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}

/**
 * @brief: 设置字体格式为下划线。
 *
 * */
void MainWindow::textUnderline() {
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

/**
 * @brief:  减小缩进
 * */
void MainWindow::unindent() {
    modifyIndentation(-1);
}

/**
 * @brief:  尝试更新笔记内容到笔记树中。
 *
 * 如果文本编辑控件中的文本被修改过，则将内容同步到当前笔记对象。
 *
 * */
void MainWindow::updateContent() {
    if (_current_item != nullptr && _current_item->sNoteItemType() == SNoteItemType::Note) {
        if (ui->textEdit->document()->isModified()) {
            _current_item->setContent(ui->textEdit->toHtml());
        }
    }
}

/**
 * @brief:  更新笔记树控件数据。
 * */
void MainWindow::updateTreeView() {
    _model->clear();
    if (_note != nullptr)
        _model->appendRow(_note);
    refreshTreeView();
}

/**
 * @brief:  缩小字体
 *
 * */
void MainWindow::zoomIn() {
    auto current = comboSize->currentIndex();
    if (current - 1 > 0) {
        comboSize->setCurrentIndex(current - 1);
        textSize(comboSize->currentText());
    }
}

/**
 * @brief: 放大字体
 *
 * */
void MainWindow::zoomOut() {
    auto max = comboSize->count();
    auto current = comboSize->currentIndex();
    if (current + 1 < max) {
        comboSize->setCurrentIndex(current + 1);
        textSize(comboSize->currentText());
    }
}