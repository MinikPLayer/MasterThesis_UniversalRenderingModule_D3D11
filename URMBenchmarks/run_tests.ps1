function RunList() {
    param (
        [string[]] $tests
    )

    for ($i = 0; $i -lt $tests.Count; $i++) {
        Write-Host "Running test: $($tests[$i])..."
        Start-Process -Wait -FilePath .\URMBenchmarks.exe -ArgumentList "run", $tests[$i]
    }
}

$TESTS_LIST_FILE = "tests_list.txt"

Start-Process -Wait -FilePath .\URMBenchmarks.exe -ArgumentList "save_tests_list_to_file", $TESTS_LIST_FILE

$contents = Get-Content $TESTS_LIST_FILE
# Tests names start with "test_"
$tests = $contents | Where-Object { $_ -match '^test_' }

# Demos start with "demo_"
$demoTests = $contents | Where-Object { $_ -match '^demo_' }

Write-Host "Running tests..."
RunList -tests $tests

Write-Host "Running demo tests..."
RunList -tests $demoTests
