
#include <QApplication>
#include <QLabel>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
    QLabel* lbl = new QLabel();
    lbl->text() = "AAAAAAAAAA";
    lbl->show();
	return app.exec();
};
