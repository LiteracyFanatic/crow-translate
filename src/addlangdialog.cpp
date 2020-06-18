/*
 *  Copyright © 2018-2020 Hennadii Chernyshchyk <genaloner@gmail.com>
 *
 *  This file is part of Crow Translate.
 *
 *  Crow Translate is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Crow Translate is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a get of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "addlangdialog.h"
#include "ui_addlangdialog.h"

#include "languagebuttonswidget.h"

#include <QPushButton>

AddLangDialog::AddLangDialog(const QVector<QOnlineTranslator::Language> &currentLanguages, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddLangDialog)
{
    ui->setupUi(this);

    // Load languages
    for (int i = 1; i <= QOnlineTranslator::Zulu; ++i) {
        const auto language = static_cast<QOnlineTranslator::Language>(i);
        if (!currentLanguages.contains(language))
            addLanguage(ui->availableLanguagesListWidget, language);
    }
    ui->availableLanguagesListWidget->setCurrentRow(0);

    for (QOnlineTranslator::Language language : currentLanguages)
        addLanguage(ui->currentLanguagesListWidget, language);
    if (ui->currentLanguagesListWidget->count() != 0)
        ui->currentLanguagesListWidget->setCurrentRow(0);
}

AddLangDialog::~AddLangDialog()
{
    delete ui;
}

QVector<QOnlineTranslator::Language> AddLangDialog::languages() const
{
    return m_languages;
}

void AddLangDialog::accept()
{
    QDialog::accept();

    m_languages.reserve(ui->currentLanguagesListWidget->count());
    for (int i = 0; i < ui->currentLanguagesListWidget->count(); ++i) {
        QListWidgetItem *item = ui->currentLanguagesListWidget->item(i);
        m_languages.append(item->data(Qt::UserRole).value<QOnlineTranslator::Language>());
    }
}

void AddLangDialog::filterLanguages(const QString &text)
{
    bool isItemSelected = false;
    for (int i = 0; i < ui->availableLanguagesListWidget->count(); ++i) {
        QListWidgetItem *item = ui->availableLanguagesListWidget->item(i);
        if (item->text().contains(text, Qt::CaseInsensitive)) {
            item->setHidden(false);
            if (!isItemSelected) {
                ui->availableLanguagesListWidget->setCurrentItem(item); // Select first unhidden item
                isItemSelected = true;
            }
        } else {
            item->setHidden(true);
        }
    }

    // Disable Ok button if no item selected
    ui->dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(isItemSelected);
}

void AddLangDialog::moveLanguageRight()
{
    moveLanguageHorizontally(ui->availableLanguagesListWidget, ui->currentLanguagesListWidget);
    ui->moveLeftButton->setEnabled(true);
}

void AddLangDialog::moveLanguageLeft()
{
    // Block signals to emit index change after item deletion
    ui->currentLanguagesListWidget->blockSignals(true);
    moveLanguageHorizontally(ui->currentLanguagesListWidget, ui->availableLanguagesListWidget);
    ui->currentLanguagesListWidget->blockSignals(false);

    emit ui->currentLanguagesListWidget->currentRowChanged(ui->currentLanguagesListWidget->currentRow());

    if (ui->currentLanguagesListWidget->count() == 0)
        ui->moveLeftButton->setEnabled(false);
}

void AddLangDialog::moveLanguageUp()
{
    moveLanguageVertically(ui->currentLanguagesListWidget, - 1);
}

void AddLangDialog::moveLanguageDown()
{
    moveLanguageVertically(ui->currentLanguagesListWidget, + 1);
}

void AddLangDialog::checkVerticalMovement(int row)
{
    if (row == -1) {
        ui->moveUpButton->setEnabled(false);
        ui->moveDownButton->setEnabled(false);
        return;
    }

    // Disable "Up" button for first element and "Down" for last
    ui->moveUpButton->setEnabled(row != 0);
    ui->moveDownButton->setEnabled(row != ui->currentLanguagesListWidget->count() - 1);

}

void AddLangDialog::addLanguage(QListWidget *widget, QOnlineTranslator::Language language)
{
    auto *item = new QListWidgetItem;
    item->setText(QOnlineTranslator::languageString(language));
    item->setIcon(LanguageButtonsWidget::countryIcon(language));
    item->setData(Qt::UserRole, language);
    widget->addItem(item);
}

void AddLangDialog::moveLanguageVertically(QListWidget *widget, int offset)
{
    const int currentRow = widget->currentRow();
    widget->insertItem(currentRow + offset, widget->takeItem(currentRow));
    widget->setCurrentRow(currentRow + offset);
}

void AddLangDialog::moveLanguageHorizontally(QListWidget *from, QListWidget *to)
{
    QListWidgetItem *item = from->takeItem(from->currentRow());
    to->addItem(item);
    to->setCurrentItem(item);
}
