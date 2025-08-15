$timestamp = Get-Date -Format "yyyyMMdd_HHmmss"
$zipName = "project_context_$timestamp.zip"
$paths = @(
    "README.md",
    ".gitignore",
    "docs",
    "templates",
    "specifications",
    "PRD",
    "*.md",
    "*.txt",
    "*.csv"
)

$existing = $paths | Where-Object { Test-Path $_ }

if ($existing.Count -gt 0) {
    Compress-Archive -Path $existing -DestinationPath $zipName
    Write-Output "✅ Zipped into: $zipName"
} else {
    Write-Output "⚠️ No matching files or folders to zip!"
}
Read-Host -Prompt "Press Enter to close"
