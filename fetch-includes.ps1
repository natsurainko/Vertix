$config = Get-Content -Path "includes.json" -Raw | ConvertFrom-Json

$repository = $config.repository
$commit = $config.commit
$includes = $config.includes

$maxNameLen = ($includes | ForEach-Object { $_.includeName.Length } | Measure-Object -Maximum).Maximum
$maxSourceLen = ($includes | ForEach-Object { $_.sourceDirectory.Length } | Measure-Object -Maximum).Maximum
$maxFilesLen = ($includes | ForEach-Object { "$($_.files.Count) files".Length } | Measure-Object -Maximum).Maximum

Write-Host "`nRepository: " -NoNewline -ForegroundColor Cyan
Write-Host "$repository" -ForegroundColor White
Write-Host "Commit: " -NoNewline -ForegroundColor Cyan
Write-Host "$commit`n" -ForegroundColor Magenta

Write-Host "Includes to fetch:" -ForegroundColor Cyan
Write-Host ("=" * 120) -ForegroundColor Gray

foreach ($include in $includes) {
    $fileCount = $include.files.Count
    $name = $include.includeName.PadRight($maxNameLen)
    $source = $include.sourceDirectory.PadRight($maxSourceLen)
    $files = "$fileCount files".PadRight($maxFilesLen)

    Write-Host "$name " -NoNewline -ForegroundColor White
    Write-Host "| " -NoNewline -ForegroundColor Gray
    Write-Host "$source " -NoNewline -ForegroundColor Yellow
    Write-Host "| " -NoNewline -ForegroundColor Gray
    Write-Host "$files " -NoNewline -ForegroundColor Cyan
    Write-Host "| " -NoNewline -ForegroundColor Gray
    Write-Host "Waiting" -ForegroundColor DarkGray
}

Write-Host ""

if (Test-Path "includes") {
    Write-Host "Cleaning existing includes directory..." -ForegroundColor Yellow
    Remove-Item -Path "includes\*" -Recurse -Force
} else {
    Write-Host "Creating includes directory..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "includes" | Out-Null
}

Write-Host "`nStarting download..." -ForegroundColor Cyan
Write-Host ("=" * 120) -ForegroundColor Gray

$successCount = 0
$failedCount = 0
$totalCount = $includes.Count

foreach ($include in $includes) {
    $targetDir = $include.targetDirectory
    $sourceDir = $include.sourceDirectory
    $totalFiles = $include.files.Count

    $updateStatus = {
        param($status, $color, $progress = "")

        $name = $include.includeName.PadRight($maxNameLen)
        $source = $include.sourceDirectory.PadRight($maxSourceLen)
        $files = "$totalFiles files".PadRight($maxFilesLen)
        $statusText = if ($progress) { "$status $progress" } else { $status }

        Write-Host "`r$name " -NoNewline -ForegroundColor White
        Write-Host "| " -NoNewline -ForegroundColor Gray
        Write-Host "$source " -NoNewline -ForegroundColor Yellow
        Write-Host "| " -NoNewline -ForegroundColor Gray
        Write-Host "$files " -NoNewline -ForegroundColor Cyan
        Write-Host "| " -NoNewline -ForegroundColor Gray
        Write-Host "$statusText" -NoNewline -ForegroundColor $color
        Write-Host (" " * 30) -NoNewline
    }

    $name = $include.includeName.PadRight($maxNameLen)
    $source = $include.sourceDirectory.PadRight($maxSourceLen)
    $files = "$totalFiles files".PadRight($maxFilesLen)

    Write-Host "$name " -NoNewline -ForegroundColor White
    Write-Host "| " -NoNewline -ForegroundColor Gray
    Write-Host "$source " -NoNewline -ForegroundColor Yellow
    Write-Host "| " -NoNewline -ForegroundColor Gray
    Write-Host "$files " -NoNewline -ForegroundColor Cyan
    Write-Host "| " -NoNewline -ForegroundColor Gray
    Write-Host "Waiting" -NoNewline -ForegroundColor DarkGray

    try {
        if (-not (Test-Path $targetDir)) {
            New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
        }

        $fileSuccess = $true
        $downloadedCount = 0

        foreach ($file in $include.files) {
            & $updateStatus "In Progress" "Blue" "($downloadedCount / $totalFiles)"

            $fileUrl = "$repository/raw/$commit/$sourceDir/$file"
            $fileName = Split-Path $file -Leaf
            $fileDir = Split-Path $file -Parent

            if ($fileDir) {
                $targetSubDir = Join-Path $targetDir $fileDir
                if (-not (Test-Path $targetSubDir)) {
                    New-Item -ItemType Directory -Path $targetSubDir -Force | Out-Null
                }
                $targetFile = Join-Path $targetSubDir $fileName
            } else {
                $targetFile = Join-Path $targetDir $fileName
            }

            try {
                Invoke-WebRequest -Uri $fileUrl -OutFile $targetFile -UseBasicParsing -ErrorAction Stop
                $downloadedCount++
            } catch {
                Write-Host "`nError downloading $fileUrl : $_" -ForegroundColor Red
                $fileSuccess = $false
                break
            }
        }

        if ($fileSuccess) {
            & $updateStatus "Finished" "Green" "($downloadedCount / $totalFiles)"
            Write-Host ""
            $successCount++
        } else {
            throw "Download failed"
        }

    } catch {
        & $updateStatus "Failed" "Red" "($downloadedCount / $totalFiles)"
        Write-Host ""
        $failedCount++
    }
}

Write-Host ""
Write-Host ("=" * 120) -ForegroundColor Gray
if ($failedCount -eq 0) {
    Write-Host "$successCount/$totalCount includes fetched successfully" -ForegroundColor Green
} else {
    Write-Host "$successCount/$totalCount includes fetched successfully, $failedCount failed" -ForegroundColor Yellow
}
Write-Host ""