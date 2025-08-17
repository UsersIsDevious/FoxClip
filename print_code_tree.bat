@echo off
rem 現在のフォルダ名を取得
for %%P in (.) do set CURNAME=%%~nxP

echo %CURNAME%
echo +--- src
for /f "usebackq delims=" %%L in (`tree "src" /A /F ^| more +3`) do @echo ^|   %%L
echo \--- include
for /f "usebackq delims=" %%L in (`tree "include" /A /F ^| more +3`) do @echo      %%L
