// src/infra_shared/ui/window/infrastructure/qt/QtHelloWindow.h
#pragma once

#include <QDialog>
#include <QString>
#include <memory>
#include "infra_shared/ui/window/domain/IWindow.h"

class QLabel;
class QVBoxLayout;

namespace foxclip::infra_shared::ui::window {

// Qt ダイアログ + IWindow 実装（非モーダル表示）
class QtHelloWindow : public QDialog, public IWindow {
	Q_OBJECT
public:
	explicit QtHelloWindow(QWidget *parent, const QString &text);
	~QtHelloWindow() override;

	void show() override; // IWindow

private:
	QLabel *label_{nullptr};
	QVBoxLayout *layout_{nullptr};
};

} // namespace foxclip::infra_shared::ui::window