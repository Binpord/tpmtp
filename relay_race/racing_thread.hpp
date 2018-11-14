#include <QThread>
#include <iostream>

class RacingThread : public QThread {
  Q_OBJECT;

public:
  RacingThread(int i) : i_(i) {}

  void run() { exec(); }

public slots:
  void takeTurn() {
    std::cout << "Thread " << i_ << " is taking turn..." << std::endl;
    emit finishedTurn();
    exit(0);
  }

signals:
  void finishedTurn();

private:
  int i_;
};
