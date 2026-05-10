#Requires -RunAsAdministrator

$ErrorActionPreference = 'Stop'

$Repo      = 'ShulhaOleh/mouse-jiggler'
$Binary    = 'mouse-jiggler-x86_64.exe'
$InstallDir = "$env:ProgramFiles\mouse-jiggler"
$InstallBin = "$InstallDir\mouse-jiggler.exe"

Write-Host "Fetching latest release..."
$release = Invoke-RestMethod "https://api.github.com/repos/$Repo/releases/latest"
$version = $release.tag_name
$url = $release.assets | Where-Object { $_.name -eq $Binary } | Select-Object -ExpandProperty browser_download_url

if (-not $url) {
    Write-Error "Could not find $Binary in release $version"
}

Write-Host "Downloading mouse-jiggler $version..."
New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null
Invoke-WebRequest -Uri $url -OutFile $InstallBin -UseBasicParsing

Write-Host "Adding to system PATH..."
$currentPath = [Environment]::GetEnvironmentVariable('Path', 'Machine')
if ($currentPath -notlike "*$InstallDir*") {
    [Environment]::SetEnvironmentVariable('Path', "$currentPath;$InstallDir", 'Machine')
}

Write-Host "Done! Open a new terminal and run 'mouse-jiggler' to start."
