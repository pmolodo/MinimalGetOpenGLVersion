@echo off

cd %~dp0
if not EXIST _build (
    mkdir _build
)
cd _build
cmake -A x64 ..
cmake --build . --config Release