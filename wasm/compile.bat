emcmdprompt.bat
em++ ../towasm.cpp -s WASM=1 -sEXPORTED_FUNCTIONS="[_init_bl, _set_step, _set_cycle, _finished, _draw, _svg, _html, _error, _errorMsg, _load_signature, _rotate_venn]" -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE="[$stringToNewUTF8,$stringToNewUTF8]" -s ALLOW_MEMORY_GROWTH=1 -s WASM=0 -s SINGLE_FILE=1 -o nvenn_single.js
pause
