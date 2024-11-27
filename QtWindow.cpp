#include "QtWindow.h"

QtWindow::QtWindow(QWidget *parent): 

	QWidget(parent, Qt::MSWindowsOwnDC) {

  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);
  setAutoFillBackground(true);

  ui.setupUi(this);

	handle = (HWND)winId();
	CreateGLContext();
	wglMakeCurrent(dc, rc);
	if (glewInit() != GLEW_OK) {
		throw "GL initialization failed";
	}

	dcRatio = static_cast<GLfloat>(devicePixelRatio());
	w = width();
	h = height();
	glViewport(0, 0, static_cast<GLint>(w * dcRatio), static_cast<GLint>(h * 2));

	InitializeGL();

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

	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(smp, 0);

	QMatrix4x4 modelMat;
	QMatrix4x4 viewMat;
	QMatrix4x4 projMat;

	modelMat.translate(0.0f, 0.0f, -1.5f);

	QVector3D eyePoint(0.0f, 0.0f, 0.0f);
	viewMat.lookAt(eyePoint, eyePoint + QVector3D(0.0f, 0.0f, -1.0f), QVector3D(0.0f, 1.0f, 0.0f));

	projMat.perspective(45.0f, static_cast<GLfloat>(w) / static_cast<GLfloat>(h), 0.1f, 100.0f);

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat.data());
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, viewMat.data());
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, projMat.data());

	glBindVertexArray(vao);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	SwapBuffers(dc);
}

bool QtWindow::event(QEvent* event) {
	if (event->type() == QtEvent::GLRenderer) {
		Renderer();
	}
	return QWidget::event(event);
}

void QtWindow::resizeEvent(QResizeEvent* event) {
	w = event->size().width();
	h = event->size().height();
	glViewport(0, 0, static_cast<GLint>(w * dcRatio), static_cast<GLint>(h * 2));
	GLUpdate();
}

void QtWindow::GLUpdate() {
	QApplication::postEvent(this, new QtEvent(QtEvent::GLRenderer));
}

void QtWindow::InitializeGL() {
	program = CreateGPUProgram("assets/vertexShader.glsl", "assets/fragmentShader.glsl");

	posLoc = glGetAttribLocation(program, "pos");
	colLoc = glGetAttribLocation(program, "col");
	texCoordLoc = glGetAttribLocation(program, "texCoord");
	modelLoc = glGetUniformLocation(program, "modelMat");
	viewLoc = glGetUniformLocation(program, "viewMat");
	projLoc = glGetUniformLocation(program, "projMat");

	smp = glGetUniformLocation(program, "uTexture");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	CreateGLBuffer(&vbo, GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vertices), vertices);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(posLoc);
	glVertexAttribPointer(
		posLoc,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(GLfloat), // stride
		(void*)(0) // offset
	);
	glEnableVertexAttribArray(colLoc);
	glVertexAttribPointer(colLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(texCoordLoc);
	glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, NULL); // vbo
	glBindVertexArray(NULL); // vao

	CreateGLBuffer(&ebo, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(indices), indices);

	LoadTextureImage(&texture, L"assets/resources/img/texture.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_BGRA);

	if (glGetError()) {
		throw;
	}

	//direction: +z: front; -z: back.
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
}
