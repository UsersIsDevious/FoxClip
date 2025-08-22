// src/infra_shared/ui/window/infrastructure/qt/QtHelloWindow.cpp
#include "infra_shared/ui/window/infrastructure/qt/QtHelloWindow.h"

#include <QLabel>
#include <QVBoxLayout>

namespace foxclip::infra_shared::ui::window {

QtHelloWindow::QtHelloWindow(QWidget *parent, const QString &text) : QDialog(parent)
{
	setWindowTitle("Foxclip - HaloWorld");
	setObjectName("fc.ui.QtHelloWindow");
	setModal(false);

	layout = new QVBoxLayout(this);
	label = new QLabel(text, this);
	label->setAlignment(Qt::AlignCenter);

	layout->addWidget(label);
	resize(360, 160);
}

QtHelloWindow::~QtHelloWindow() = default;

void QtHelloWindow::show()
{
	// QDialog::show() は非モーダル表示
	QDialog::show();
	QDialog::raise();
	QDialog::activateWindow();
}

} // namespace foxclip::infra_shared::ui::window
