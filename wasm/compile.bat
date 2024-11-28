emcmdprompt.bat
../towasm.cpp -s WASM=1 -sEXPORTED_FUNCTIONS="[_return_true, _init_bl, _set_step, _set_cycle, _finished, _draw, _svg, _error, _errorMsg, _load_signature]" -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE="[$stringToNewUTF8,$stringToNewUTF8]" -s ALLOW_MEMORY_GROWTH=1 -o nvenn.js
pause