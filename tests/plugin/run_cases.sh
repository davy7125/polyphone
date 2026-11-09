#!/bin/bash
# Play the cases of tests/render through a plugin built (VST3, LV2 or CLAP) and compare them with the references
# rendered by the editor (tests/render/render.sh), see README.md.
# Usage: run_cases.sh <plugin_host> <plugin> <soundfont directory> <reference directory> <output directory> [threshold in dBFS]

if [ $# -lt 5 ]; then
    echo "Usage: $0 <plugin_host> <plugin> <soundfont directory> <reference directory> <output directory> [threshold in dBFS]"
    exit 1
fi
host=$1
plugin=$2
soundfontDir=$3
referenceDir=$4
outputDir=$5
threshold=${6:--100}
testsDir="$(cd "$(dirname "$0")" && pwd)"
mkdir -p "$outputDir" || exit 1

# The soundfont of the tests is generated if it is not in the directory
if [ ! -f "$soundfontDir/polyphone-test.sf2" ]; then
    python3 "$testsDir/../render/make_test_soundfont.py" "$soundfontDir/polyphone-test.sf2" || exit 1
fi

list="$outputDir/cases.txt"
: > "$list"
failures=0
for case in "$testsDir"/../render/cases/*.txt; do
    name=$(basename "$case" .txt)
    "$host" case "$plugin" "$soundfontDir" "$case" "$outputDir/$name.wav" > "$outputDir/$name.log" 2>&1
    result=$?
    if [ $result -eq 3 ]; then
        grep UNSUPPORTED "$outputDir/$name.log"
    elif [ $result -ne 0 ]; then
        echo "FAILED  $name (see $outputDir/$name.log)"
        failures=$((failures + 1))
    else
        grep RENDERED "$outputDir/$name.log" >> "$list"
    fi
done

python3 "$testsDir/compare_cases.py" "$referenceDir" "$outputDir" "$list" "$threshold" || failures=$((failures + 1))
exit $((failures > 0))
