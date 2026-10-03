@echo off
chcp 65001 >nul
set "output=tous_les_fichiers.txt"

:: Supprime le fichier de sortie s'il existe déjà pour éviter de doubler le contenu
if exist "%output%" del "%output%"

echo Début de la fusion des fichiers...

:: Parcourt tous les fichiers de manière récursive (dans le dossier et les sous-dossiers)
for /R %%i in (*) do (
    :: Exclut le script lui-même et le fichier de sortie pour éviter de les inclure dedans
    if "%%~nxži" neq "%output%" if "%%~nxži" neq "%~nx0" (
        echo Traitement de : %%i
        echo ======================================== >> "%output%"
        echo Fichier : %%i >> "%output%"
        echo ======================================== >> "%output%"
        type "%%i" >> "%output%"
        echo. >> "%output%"
        echo. >> "%output%"
    )
)

echo.
echo Opération terminée ! Tous les fichiers ont été regroupés dans "%output%".
pause