set files=src/main.cpp src/Game/Game.cpp src/Entity/Entity.cpp src/Player/Player.cpp

em++ -o index.html %files% -std=c++17 -Os -Wall ./llib/libraylib.a -I. -I include/ -L. -L llib/ -s USE_GLFW=3 --shell-file shell.html -DPLATFORM_WEB