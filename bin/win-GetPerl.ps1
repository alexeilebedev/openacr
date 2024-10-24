# EXPERIMENTAL. FOR NOW USE ai.bat

$ProgressPreference = 'SilentlyContinue'

function Test-PerlInstalled {
    $perlPath = Get-Command perl -ErrorAction SilentlyContinue
    return $null -ne $perlPath
}

if (Test-PerlInstalled) {
    exit
}

$downloadPageUrl = "https://strawberryperl.com/releases.html"
try {
    $htmlContent = Invoke-WebRequest -Uri $downloadPageUrl -UseBasicParsing -Headers @{
        "User-Agent" = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
    }
} catch {
    Write-Error "Failed to retrieve the download page. Exiting script."
    exit
}

$downloadLink = $htmlContent.Links | Where-Object { $_.href -match "portable.*\.zip$" } | Select-Object -First 1 | Select-Object -ExpandProperty href

if (-not $downloadLink) {
    Write-Error "Failed to find the download link for the latest portable Perl. Exiting script."
    exit
}

# concatenate with the base URL if the link is relative
if ($downloadLink -notmatch "^https?://") {
    $downloadUrl = "https://strawberryperl.com" + $downloadLink
} else {
    $downloadUrl = $downloadLink
}

$outputPath = "$PSScriptRoot\acr_perl.zip"
$extractPath = "$PSScriptRoot\perl-portable"

if (-not (Test-Path -Path $extractPath)) {
    try {
        Invoke-WebRequest  -Uri $downloadUrl -OutFile $outputPath -Headers @{
            "User-Agent" = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
        }
    } catch {
        Write-Error "Perl download failed. Exiting."
        exit
    }

    # extract
    try {
        Add-Type -AssemblyName System.IO.Compression.FileSystem
        [System.IO.Compression.ZipFile]::ExtractToDirectory($outputPath, $extractPath)
    } catch {
        Write-Error "Perl extraction failed. Exiting."
        exit
    }
}

if (Test-Path $outputPath) {
    Remove-Item $outputPath
}


try {
    $linkTarget = "$PSScriptRoot\perl-portable\perl\bin\perl.exe"
    $linkPath = "$PSScriptRoot\perl.exe"

    if (-Not (Test-Path -Path $linkTarget)) {
        Write-Error "Target Perl executable not found at $linkTarget. Exiting script."
        exit
    }

    if (Test-Path -Path $linkPath) {
        Remove-Item -Path $linkPath -Force
    }

    New-Item -ItemType HardLink -Path $linkPath -Target $linkTarget

    Write-Host "Symbolic link created from $linkTarget to $linkPath."
} catch {
    Write-Error "Failed to create the symbolic link. Exiting script."
    exit
}