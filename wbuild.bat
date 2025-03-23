set files=src/main.cpp src/Game/Game.cpp src/Entity/Entity.cpp src/Player/Player.cpp

em++ -o build/web/index.html %files% -std=c++17 -Os -Wall ./lib/libraylib.web.a -I. -I include/ -L. -L lib/ -s USE_GLFW=3 -s EXPORTED_FUNCTIONS="['_main', '_SetHighScore', '_GetHighScore']" -s EXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"getValue\",\"setValue\"] --shell-file build/web/shell.html -DPLATFORM_WEB