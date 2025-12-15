#include <iostream>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QListWidget>
#include <QFileDialog>
#include <QLineEdit>
#include <QStackedWidget>
#include <QStackedLayout>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QButtonGroup>
#include <QGroupBox>
#include <QGraphicsDropShadowEffect>

#include "ChatWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ChatWindow window;
    QString tmsg = "hi";
    window.addUserMessage(tmsg);
    window.addUserMessage("wawa");
    window.addUserMessage("olala");
    window.addUserMessage("well i wouldnt say setting it straight im just saying there are other politiacians that twist the truth to make it sound better than it is but ofcourse not here, im here to set it straight not like some of the other important questions of our great country that are answered in a vague and misdirecting way to sway the opinion in a certain direction you see the records set by me are infact straighter than those from the opposing party and there for better but ofcourse no record is ever fully straight, i mean i wasnt there to retell the events exactly or have expert opinions on the matter like how a ship captain or marin biologist would have, i am definetly not a marine biologist but i did serve for about 4 months at a navy base were i drank tea with the captain and he was a good guy he seemed like he could give an educated opinion on matter much better  than me and help the record to be straighter but while it might not still be fully straight it will be straighter than those set by other partys and people who want to burry this event which is not me not as much atleast, see im not saying i want to bold this event in the public eye but burrying it certainly isnt my intention, now thinking about it your question seems a bit out of line and manippulative, because no record can ever be fully straight not for such a tragic event since we were not there to recite the event in their fullest and so i think attemping this political slander from you is a national offense and i as the voice of all america will not stand for such things");
    window.show();

    return app.exec();
}