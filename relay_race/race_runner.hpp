#include "racing_thread.hpp"
#include <QThread>
#include <iostream>
#include <list>

class RaceRunnerThread : public QThread {
  Q_OBJECT;

public:
  virtual void run() override { exec(); }
};

class RaceRunner : public QObject {
  Q_OBJECT;

public:
  RaceRunner(int N) : N_(N) {}

  void tuneRunner() {
    threads_.emplace_back(0);
    RacingThread *prev = &threads_.back();
    bool ret =
        QObject::connect(this, SIGNAL(startRace()), prev, SLOT(takeTurn()));
    if (!ret) {
      std::cerr << "First slot is not connected" << std::endl;
      exit(1);
    }
    for (int i = 1; i < N_; ++i) {
      threads_.emplace_back(i);
      ret = QObject::connect(prev, SIGNAL(finishedTurn()), &threads_.back(),
                             SLOT(takeTurn()));
      if (!ret) {
        std::cerr << "Slot " << i << "is not connected" << std::endl;
        exit(1);
      }
      prev = &threads_.back();
    }
    ret = QObject::connect(prev, SIGNAL(finishedTurn()), this,
                           SLOT(finishRace()));
    if (!ret) {
      std::cerr << "Last slot is not connected" << std::endl;
      exit(1);
    }

    std::cout << "Race is starting..." << std::endl;
    emit startRace();
  }

public slots:
  void finishRace() {
    std::cout << "Race is finished..." << std::endl;
    for (auto &th : threads_) {
      th.wait();
    }
    exit(0);
  }

signals:
  void startRace();

private:
  int N_;
  std::list<RacingThread> threads_;
};

