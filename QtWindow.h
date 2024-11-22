#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtWindow.h"

#include <Windows.h>
#include <GL/glew.h>
#include <QResizeEvent>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

#include "QtEvent.h"

class QtWindow : public QWidget {
  Q_OBJECT

public:
  QtWindow(QWidget *parent = nullptr);
  ~QtWindow();
  virtual QPaintEngine* paintEngine() const override { return NULL; }
  virtual void resizeEvent(QResizeEvent* event) override;

private:
  bool CreateGLContext();
  void Renderer();
  bool event(QEvent* event);
  void GLUpdate();

private:
  Ui::QtWindowClass ui;

  HDC dc;
  HGLRC rc;
  HWND handle;
};
