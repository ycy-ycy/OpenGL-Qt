#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtWindow.h"
#include <QResizeEvent>

#include "QtEvent.h"
#include "GLTools.h"

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
  void InitializeGL();

private:
  Ui::QtWindowClass ui;

  HDC dc;
  HGLRC rc;
  HWND handle;
  int w, h;
	float dcRatio;

	GLuint program;
	GLfloat vertices[32] = {
		// positions       // colors        // texture coords
		-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.7f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 0.2f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 0.0f, 0.9f, 0.9f, 1.0f, 1.0f
	};
	GLuint vbo, vao, ebo;
	GLuint indices[6] = {
		0, 1, 2,
		0, 2, 3
	};
	GLuint texture;
	GLint smp;
};
