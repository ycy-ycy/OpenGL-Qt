#pragma once

#include <QEvent>

class QtEvent : public QEvent {

public:
	enum Type {
		GLRenderer = QEvent::User + 1
	};
	QtEvent(Type type) : QEvent(static_cast<QEvent::Type>(type)) {}
};