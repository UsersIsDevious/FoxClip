@echo off
echo .
echo +--- src
for /f "usebackq delims=" %%L in (`tree "src" /A /F ^| more +3`) do @echo ^|   %%L
echo \--- include
for /f "usebackq delims=" %%L in (`tree "include" /A /F ^| more +3`) do @echo      %%L
