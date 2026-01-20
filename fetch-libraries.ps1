$config = Get-Content -Path "libraries.json" -Raw | ConvertFrom-Json

$repository = $config.repository
$commit = $config.commit
$targetDirectory = $config.targetDirectory
$files = $config.files

Write-Host "`nRepository: " -NoNewline -ForegroundColor Cyan
Write-Host "$repository" -ForegroundColor White
Write-Host "Commit: " -NoNewline -ForegroundColor Cyan
Write-Host "$commit" -ForegroundColor Magenta
Write-Host "Target: " -NoNewline -ForegroundColor Cyan
Write-Host "$targetDirectory`n" -ForegroundColor Yellow

Write-Host "Libraries to fetch: $($files.Count) files" -ForegroundColor Cyan
Write-Host ("=" * 120) -ForegroundColor Gray

foreach ($file in $files) {
    $fileName = Split-Path $file -Leaf
    Write-Host "  - $fileName" -ForegroundColor White
}

Write-Host ""

if (Test-Path $targetDirectory) {
    Write-Host "Cleaning existing libraries directory..." -ForegroundColor Yellow
    Remove-Item -Path "$targetDirectory\*" -Force
} else {
    Write-Host "Creating libraries directory..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path $targetDirectory | Out-Null
}

Write-Host "`nStarting download..." -ForegroundColor Cyan
Write-Host ("=" * 120) -ForegroundColor Gray

$successCount = 0
$failedCount = 0
$totalCount = $files.Count
$currentIndex = 0

foreach ($file in $files) {
    $currentIndex++
    $fileName = Split-Path $file -Leaf
    $fileUrl = "$repository/raw/$commit/$file"
    $targetFile = Join-Path $targetDirectory $fileName

    $progress = "[$currentIndex/$totalCount]"
    Write-Host "$progress " -NoNewline -ForegroundColor Gray
    Write-Host "$fileName" -NoNewline -ForegroundColor White
    Write-Host " ... " -NoNewline -ForegroundColor Gray

    try {
        Invoke-WebRequest -Uri $fileUrl -OutFile $targetFile -UseBasicParsing -ErrorAction Stop
        Write-Host "✓ Downloaded" -ForegroundColor Green
        $successCount++
    } catch {
        Write-Host "✗ Failed" -ForegroundColor Red
        Write-Host "  Error: $_" -ForegroundColor Red
        $failedCount++
    }
}

Write-Host ""
Write-Host ("=" * 120) -ForegroundColor Gray
if ($failedCount -eq 0) {
    Write-Host "All $successCount/$totalCount libraries fetched successfully" -ForegroundColor Green
} else {
    Write-Host "$successCount/$totalCount libraries fetched successfully, $failedCount failed" -ForegroundColor Yellow
}
Write-Host ""