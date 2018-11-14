#include "racing_thread.hpp"
#include <QThread>
#include <iostream>
#include <list>

class RaceRunner : public QThread {
  Q_OBJECT;

public:
  RaceRunner(int N) : N_(N) {}

  void run() {
    {
      threads_.emplace_back(0);
      QObject::connect(this, SIGNAL(startRace()), &threads_.back(),
                       SLOT(takeTurn()));
      threads_.back().start();
    }
    for (int i = 1; i < N_; ++i) {
      auto &th = threads_.back();
      threads_.emplace_back(i);
      QObject::connect(&th, SIGNAL(finishedTurn()), &threads_.back(),
                       SLOT(takeTurn()));
      threads_.back().start();
    }
    QObject::connect(&threads_.back(), SIGNAL(finishedTurn()), this,
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
