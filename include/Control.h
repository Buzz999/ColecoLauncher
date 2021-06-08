#pragma once

#include <stdint.h>
#define N_FILENAMES 80
#define FILENAME_SIZE 12

class Control {
 public:
  enum MS_STATE {
    MS_INIT,
    MS_PROMPT,  
    MS_DIR,
    MS_SELECT,   
    MS_RUN,
    MS_HALT
  };

  enum MS_DECODE
  {
    IDLE,
    ACQUIRE,
    SKIP
  };
  
  Control()
      : _state(MS_INIT), _nEntries(0) {}
  ~Control() {}

  void init();
  void update();

 protected:
  bool acceptPrompt(bool waitPrompt);
  void acceptFilenames();
  void sendDirCommand() const;
  void showEntries() const;

  void launch() const;

  void flushInput() const;
  void changeBaudrate() const;
  
  MS_STATE _state;
  uint8_t _nEntries;
  char _filenames[N_FILENAMES][FILENAME_SIZE+1];
};