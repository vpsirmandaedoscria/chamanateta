@echo off
title Gerenciador Fazcode - Build EXE
color 0D
echo.
echo ============================================
echo   GERENCIADOR FAZCODE - Gerador de EXE
echo ============================================
echo.

:: Verifica se Python esta instalado
python --version >nul 2>&1
if errorlevel 1 (
    echo [ERRO] Python nao encontrado! Instale Python 3 primeiro.
    echo Download: https://www.python.org/downloads/
    echo.
    echo IMPORTANTE: Na instalacao do Python, marque a opcao
    echo "Add Python to PATH"
    pause
    exit /b 1
)

echo [1/4] Instalando dependencias...
call python -m pip install --upgrade pip
call python -m pip install flask psutil pyinstaller
if errorlevel 1 (
    echo [ERRO] Falha ao instalar dependencias.
    echo Tente rodar como Administrador
    pause
    exit /b 1
)

echo.
echo [2/4] Preparando arquivos...
echo.

echo [3/4] Gerando executavel com PyInstaller...
echo       Isso pode demorar alguns minutos...
echo.
call python -m PyInstaller --noconfirm --onefile --console ^
    --name "GerenciadorFazcode" ^
    --icon "static\img\icon.ico" ^
    --add-data "templates;templates" ^
    --add-data "static;static" ^
    --hidden-import flask ^
    --hidden-import psutil ^
    --hidden-import jinja2.ext ^
    app.py

if errorlevel 1 (
    echo.
    echo [AVISO] Tentando sem icone .ico ...
    call python -m PyInstaller --noconfirm --onefile --console ^
        --name "GerenciadorFazcode" ^
        --add-data "templates;templates" ^
        --add-data "static;static" ^
        --hidden-import flask ^
        --hidden-import psutil ^
        --hidden-import jinja2.ext ^
        app.py
)

if errorlevel 1 (
    echo.
    echo [ERRO] Falha ao gerar o executavel.
    echo Verifique se o PyInstaller suporta sua versao do Python.
    echo Tente: python -m pip install --upgrade pyinstaller
    pause
    exit /b 1
)

echo.
echo [4/4] Limpando arquivos temporarios...
rmdir /s /q build 2>nul
del GerenciadorFazcode.spec 2>nul

echo.
echo ============================================
echo   BUILD CONCLUIDO COM SUCESSO!
echo ============================================
echo.
echo   Executavel gerado em:
echo   dist\GerenciadorFazcode.exe
echo.
echo   Para usar:
echo   1. Execute GerenciadorFazcode.exe
echo   2. Abra o navegador em http://localhost:5000
echo.
echo ============================================

:: Abre a pasta dist
explorer dist

pause
