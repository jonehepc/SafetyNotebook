#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "passworddialog.h"
#include "loader.h"
#include "PasswordGenerateDialog.h"
#include "settingsdialog.h"
#include "newdirdialog.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtGui/QCloseEvent>
#include <exception>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi((QMainWindow *) this);

    _model = new QStandardItemModel();
    _settings = new Settings();
    _settings->loadSettings();
    initUi();
    initSignalAndSlots();

    updateTreeView();
}

MainWindow::~MainWindow() {
    delete ui;
    delete _settings;
    if (_model != nullptr) {
        delete _model;
        _model = nullptr;
    }
}

void MainWindow::onActionAboutQtTriggered() {
    QMessageBox::aboutQt(this);
}

void MainWindow::onActionAboutTriggered() {
    QMessageBox::about(this, QStringLiteral("关于安全笔记本"),
                       QStringLiteral("安全笔记本是用密码加密您的笔记内容的工具，用于保护您的机密信息。"));
}

void MainWindow::onActionAddTriggered() {
    _current_item = nullptr;
    _do_note_change_edition = true;
    _in_refresh = true;
    ui->lineEditTitle->setText(QStringLiteral(""));
    ui->textEdit->setText(QStringLiteral(""));
    ui->lineEditTitle->setEnabled(true);
    ui->textEdit->setEnabled(true);
    auto noteItem = new SNoteItem(QUuid::createUuid(), QStringLiteral("笔记标题"), QStringLiteral(""),
                                  SNoteItemType::Note);
    _current_item = noteItem;
    _current_note->appendRow(noteItem);
    _file_modified = true;
    _current_modified = false;
    _do_note_change_edition = false;
    _in_refresh = false;
    refreshTreeView();
    ui->treeView->clearSelection();
    ui->treeView->setCurrentIndex(_model->indexFromItem(_current_item));
    ui->lineEditTitle->setFocus();
}

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
        ui->textEdit->setEnabled(false);
        _model->clear();
        _current_note = nullptr;
        _current_item = nullptr;
        refreshTreeView();
        _file_path = "";
        _current_modified = false;
        _current_changed = false;
        _note = nullptr;
        displayMessageOnStatusBar(QStringLiteral("关闭笔记完成。"));
    }

}

void MainWindow::onActionCopyTriggered() {

}

void MainWindow::onActionCutTriggered() {

}

void MainWindow::onActionDeleteTriggered() {
    if (_current_item != nullptr && _current_item->sNoteItemType() == SNoteItemType::Note) {
        if (!_current_item->content().isEmpty()) {
            if (QMessageBox::warning(this, QStringLiteral(""), QStringLiteral(""),
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
                    _file_modified = true;
                    refreshTreeView();
                }
            }
        }
    }
}

void MainWindow::onActionEditTriggered() {

}

void MainWindow::onActionExitTriggered() {
    close();
}

void MainWindow::onActionNewTriggered() {
    onActionCloseTriggered();
    setEditActions(false);
    setNoteActions(true);
    setDirActions(true);
    //_model->setupModelData(new NoteItem());
    refreshTreeView();
    ui->lineEditTitle->setText("");
    ui->textEdit->setPlainText("");
    ui->textEdit->setEnabled(false);
    _file_path = "";
    _note = SNoteItem::makeRootItem();
    updateTreeView();
    _current_note = nullptr;
    displayMessageOnStatusBar(QStringLiteral("新建笔记本完成。"));
    _file_modified = true;
    setSaveActions(true);
}

void MainWindow::onActionOpenTriggered() {

    auto file_path = getOpenPath();
    if (file_path.isNull() || file_path.isEmpty())
        return;
    auto password = getPassword();
    if (password.isNull() || password.isEmpty())
        return;
    if (_note != nullptr) onActionCloseTriggered();
    _file_path = file_path;
    _password = password;
    if (load_note()) {
        setSaveActions(true);
        setNoteActions(true);
        setDirActions(true);
        displayMessageOnStatusBar(QStringLiteral("打开笔记完成"));
        setWindowTitle(QStringLiteral("安全笔记本 - [") + _file_path + QString("]"));
    }

}

void MainWindow::onActionOpenOnlineTriggered() {

}

void MainWindow::onActionPasteTriggered() {

}

void MainWindow::onActionSaveTriggered() {
    if (!_file_modified && !_current_modified) {
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
    _file_modified = false;
    _current_changed = false;
    _current_modified = false;
    ui->actionSave->setEnabled(false);
    ui->actionSave_as->setEnabled(false);
}

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
    _file_modified = false;
    _file_new = false;
    ui->actionSave->setEnabled(false);
    ui->actionSave_as->setEnabled(false);
    setWindowTitle(QStringLiteral("安全笔记本 - [") + _file_path + QString("]"));
    displayMessageOnStatusBar(QStringLiteral("笔记另存为成功。"));
}

void MainWindow::onActionSearchTriggered() {

}

void MainWindow::onActionSettingsTriggered() {
    SettingsDialog settingsDialog(this);
    settingsDialog.setRemoteSettings(_settings->remoteSettings());
    if (settingsDialog.exec() == QDialog::Accepted) {
        _settings->setRemoteSettings(settingsDialog.getRemoteSettings());
        _settings->saveSettings();
    }
}

void MainWindow::onActionAddPartitionTriggered() {
    NewDirDialog newDirDialog(this);
    if (newDirDialog.exec() == QDialog::Accepted) {
        if (!newDirDialog.getDirName().isEmpty()) {
            auto noteItem = new SNoteItem(QUuid::createUuid(), newDirDialog.getDirName(), QStringLiteral(""),
                                          SNoteItemType::Partition);
            if (_current_note == nullptr)
                _note->appendRow(noteItem);
            else
                _current_note->appendRow(noteItem);
            _current_item = noteItem;
            _current_note = noteItem;
            ui->lineEditTitle->setText(_current_item->title());
            ui->textEdit->setText(QStringLiteral(""));
            ui->lineEditTitle->setEnabled(true);
            ui->textEdit->setEnabled(false);
            refreshTreeView();
            ui->treeView->clearSelection();
            ui->treeView->setCurrentIndex(_model->indexFromItem(_current_item));
            ui->lineEditTitle->setFocus();
        }
    }
}

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
                _file_modified = true;
                refreshTreeView();
            }
        }
    }
}

void MainWindow::onTextEdited(const QString &text) {
    if (_current_note != nullptr)
        onLineEditTitleEditFinished();
}

void MainWindow::onTreeViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
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
                _current_note = note;
                //_current_item = nullptr;
                setEditActions(false);
                setDirActions(true);
                ui->actionAdd->setEnabled(true);
                ui->textEdit->setText(QStringLiteral(""));
                ui->textEdit->setEnabled(false);
                if (_current_item->parent() == nullptr) {
                    ui->actionRemovePartition->setEnabled(false);
                    ui->lineEditTitle->setEnabled(false);
                }
                //ui->textEdit->hide();
                break;
            case SNoteItemType::Note:
                //_current_item = note;
                setEditActions(true);
                ui->actionAdd->setEnabled(false);
                setDirActions(false);
                ui->textEdit->setPlainText(_current_item->content());
                ui->textEdit->setEnabled(true);
                //ui->textEdit->show();
                break;
            default:
                break;
        }

        _do_note_change_edition = false;

        if (_in_refresh)
            return;

    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    _settings->saveSettings();


    //if(_is_remote_file)
    //QWidget::closeEvent(event);
}

void MainWindow::initUi() {
    ui->lineEditTitle->setEnabled(false);
    ui->textEdit->setEnabled(false);
    ui->treeView->setModel(_model);

    setNoteActions(false);
    setDirActions(false);
    setEditActions(false);
    setSaveActions(false);

    ui->actionOpenOnline->setEnabled(_settings->remoteSettings().state);
}

void MainWindow::initSignalAndSlots() {
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
    connect(ui->lineEditTitle, SIGNAL(textChanged(const QString &)), this, SLOT(onTextEdited(const QString&)));

    auto selectionModel = ui->treeView->selectionModel();
    connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
            SLOT(onTreeViewSelectionChanged(const QItemSelection &, const QItemSelection &)));

}

void MainWindow::setNoteActions(bool e) {
    ui->treeView->setEnabled(e);
}

void MainWindow::setEditActions(bool e) {
    ui->actionAdd->setEnabled(e);
    ui->actionDelete->setEnabled(e);
    ui->actionEdit->setEnabled(e);
}

void MainWindow::setDirActions(bool e) {
    ui->actionAddPartition->setEnabled(e);
    ui->actionRemovePartition->setEnabled(e);
}

void MainWindow::displayMessageOnStatusBar(const QString &msg) {
    ui->statusBar->showMessage(msg, 3000);
}

bool MainWindow::load_note() {
    if (_note != nullptr) {
        _model->clear();
        _note = nullptr;
        updateTreeView();
    }
    _note = load_item_from_path(_file_path, _password);
    if (_note == nullptr) {
        displayMessageOnStatusBar(QStringLiteral("加载笔记失败。"));
        //_note = SNoteItem::makeRootItem();
        return false;
    }
    _model->appendRow(_note);
    refreshTreeView();
    return true;
}

void MainWindow::updateTreeView() {
    _model->clear();
    if (_note != nullptr)
        _model->appendRow(_note);
    refreshTreeView();
}

void MainWindow::refreshTreeView() {
    _in_refresh = true;
    emit _model->layoutChanged();
    ui->treeView->expandAll();
    if (_current_item != nullptr) {
        ui->treeView->selectionModel()->setCurrentIndex(_model->indexFromItem(_current_item),
                                                        QItemSelectionModel::Select);
    } else if (_current_note != nullptr) {
        auto old = _do_note_change_edition;
        _do_note_change_edition = true;
        ui->treeView->selectionModel()->setCurrentIndex(_model->indexFromItem(_current_note),
                                                        QItemSelectionModel::Select);
        _do_note_change_edition = old;
    }
    _in_refresh = false;
}

void MainWindow::saveNote() {
    onLineEditTitleEditFinished();
    save_items_to_path(_file_path, _password, *_note);
    displayMessageOnStatusBar(QStringLiteral("保存笔记成功"));
}

void MainWindow::init_remote() {

}

QString MainWindow::getPassword() {
    PasswordDialog passwordDialog(this);
    if (passwordDialog.exec() == QDialog::Accepted)
        return passwordDialog.getPassword();
    return QString();
}

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

void MainWindow::onLineEditTitleEditFinished() {
    if (_current_item == nullptr || _do_note_change_edition)
        return;

    auto need_refresh = false;

    if (ui->lineEditTitle->text() != _current_item->title()) {
        need_refresh = true;
        _current_item->setTitle(ui->lineEditTitle->text());
        _file_modified = true;
    }

    if (_current_item->sNoteItemType() == SNoteItemType::Note) {
        if (ui->textEdit->toPlainText() != _current_item->content()) {
            _current_item->setContent(ui->textEdit->toPlainText());
            _current_modified = true;
        }
    }

    if (need_refresh)
        refreshTreeView();
}

void MainWindow::onTextEdited() {
    onTextEdited("");
}

void MainWindow::onActionGenPasswordTriggered() {
    PasswordGenerateDialog dialog(this);
    dialog.exec();
}

bool MainWindow::closeNote() {
    if (_file_modified) {
        auto t = QMessageBox::warning(this, QStringLiteral("是否保存笔记"),
                                      QStringLiteral("笔记内容已经更改，是否在关闭前保存？"),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch (t) {
            case QMessageBox::Yes:
                onActionSaveTriggered();
                break;
            case QMessageBox::No:
                return true;
            case QMessageBox::Cancel:
                return false;
            default:
                return false;
        }
    }
    return true;
}

void MainWindow::setSaveActions(bool e) {
    ui->actionSave->setEnabled(e);
    ui->actionSave_as->setEnabled(e);
    ui->actionClose->setEnabled(e);
}

