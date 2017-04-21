auto Home::Input::reset() -> void {
  up     = false;
  down   = false;
  left   = false;
  right  = false;
  b      = false;
  a      = false;
  select = false;
  start  = false;
  previousUp     = false;
  previousDown   = false;
  previousLeft   = false;
  previousRight  = false;
  previousB      = false;
  previousA      = false;
  previousSelect = false;
  previousStart  = false;
}

auto Home::Input::run() -> void {
  poll();

  if(home->graphics.cursorReady()) {
    if(left && !previousLeft) {
      home->gameCursor = (home->gameCursor + home->gameList.size() - 1) % home->gameList.size();
      home->theme.updateActiveGameCard();
    }

    if(right && !previousRight) {
      home->gameCursor = (home->gameCursor + 1) % home->gameList.size();
      home->theme.updateActiveGameCard();
    }

    if(a || start) {
      program->mediumQueue.append(home->gamePath(home->gameCursor));
      program->loadMedium();
      program->updateVideoShader();
    }
  }
}

auto Home::Input::poll() -> void {
  uint port1 = Famicom::ID::Port::Controller1;
  uint gamepad = Famicom::ID::Device::Gamepad;
  enum : uint {Up, Down, Left, Right, B, A, Select, Start};

  previousUp     = up;
  previousDown   = down;
  previousLeft   = left;
  previousRight  = right;
  previousB      = b;
  previousA      = a;
  previousSelect = select;
  previousStart  = start;

  up     = program->inputPoll(port1, gamepad, Up);
  down   = program->inputPoll(port1, gamepad, Down);
  left   = program->inputPoll(port1, gamepad, Left);
  right  = program->inputPoll(port1, gamepad, Right);
  b      = program->inputPoll(port1, gamepad, B);
  a      = program->inputPoll(port1, gamepad, A);
  select = program->inputPoll(port1, gamepad, Select);
  start  = program->inputPoll(port1, gamepad, Start);
}
