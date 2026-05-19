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
python -m pip install --upgrade pip
python -m pip install flask psutil pyinstaller
if errorlevel 1 (
    echo [ERRO] Falha ao instalar dependencias.
    echo Tente rodar como Administrador (clique direito no .bat e "Executar como administrador")
    pause
    exit /b 1
)

echo.
echo [2/4] Preparando arquivos...

:: Cria o spec do PyInstaller com dados incluidos
echo.
echo [3/4] Gerando executavel com PyInstaller...
python -m PyInstaller --noconfirm --onefile --windowed ^
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
    python -m PyInstaller --noconfirm --onefile --windowed ^
        --name "GerenciadorFazcode" ^
        --add-data "templates;templates" ^
        --add-data "static;static" ^
        --hidden-import flask ^
        --hidden-import psutil ^
        --hidden-import jinja2.ext ^
        app.py
)

if errorlevel 1 (
    echo [ERRO] Falha ao gerar o executavel.
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
