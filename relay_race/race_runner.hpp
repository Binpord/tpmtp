#include "racing_thread.hpp"
#include <QThread>
#include <iostream>
#include <list>

class RaceRunner : public QThread {
  Q_OBJECT;

public:
  RaceRunner(int N) : N_(N) {}

  virtual void run() override {
    threads_.emplace_back(0);
    RacingThread *prev = &threads_.back();
    QObject::connect(this, SIGNAL(startRace()), prev,
                     SLOT(takeTurn()));
    for (int i = 1; i < N_; ++i) {
      threads_.emplace_back(i);
      QObject::connect(prev, SIGNAL(finishedTurn()), &threads_.back(),
                       SLOT(takeTurn()));
      prev = &threads_.back();
    }
    QObject::connect(prev, SIGNAL(finishedTurn()), this,
                     SLOT(finishRace()));

    std::cout << "Race is starting..." << std::endl;
    emit startRace();
    exec();
  }

public slots:
  void finishRace() {
    for (auto &th : threads_) {
      th.wait();
    }
    std::cout << "Race is finished..." << std::endl;
    exit(0);
  }

signals:
  void startRace();

private:
  int N_;
  std::list<RacingThread> threads_;
};
