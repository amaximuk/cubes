
#include <QApplication>

#include "main_window.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

    MainWindow window;
    window.setMinimumSize(800, 600);
    window.show();

	return app.exec();
};
