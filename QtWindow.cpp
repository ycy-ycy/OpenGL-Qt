#include "QtWindow.h"
#include <assert.h>

QtWindow::QtWindow(QWidget *parent): 

	QWidget(parent, Qt::MSWindowsOwnDC) {

  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);
  setAutoFillBackground(true);

  ui.setupUi(this);

	handle = (HWND)winId();
	CreateGLContext();
	wglMakeCurrent(dc, rc);
	assert(glewInit() == GLEW_OK);

	GLUpdate();
}

QtWindow::~QtWindow() {
	wglMakeCurrent(NULL, NULL);
	if (dc) {
		ReleaseDC(handle, dc);
	}
	if (rc) {
		wglDeleteContext(rc);
	}
}

bool QtWindow::CreateGLContext() {
	dc = GetDC(handle);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

	int format = 0;
	format = ChoosePixelFormat(dc, &pfd);
	if (!format) {
		throw;
	}
	SetPixelFormat(dc, format, &pfd);
	rc = wglCreateContext(dc);
	return true;
}

void QtWindow::Renderer() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SwapBuffers(dc);
}

bool QtWindow::event(QEvent* event) {
	if (event->type() == QtEvent::GLRenderer) {
		Renderer();
	}
	return QWidget::event(event);
}

void QtWindow::resizeEvent(QResizeEvent* event) {
	glViewport(0, 0, event->size().width(), event->size().height());
	GLUpdate();
}

void QtWindow::GLUpdate() {
	QApplication::postEvent(this, new QtEvent(QtEvent::GLRenderer));
}
