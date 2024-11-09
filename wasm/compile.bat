emcmdprompt.bat
em++ ../towasm.cpp -s WASM=1 -s EXPORTED_FUNCTIONS="[_init_bl, _set_step, _set_cycle, _finished, _draw, _svg]" -o nvenn.js || exit 1