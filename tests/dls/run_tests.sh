#!/bin/bash
# Tests of the DLS import and export, by comparing renderings.
# Usage: run_tests.sh <polyphone executable> <output directory> [threshold in dBFS]
# Each case of tests/dls/cases names the file tested ("# soundfont:") and a file that must sound the same
# ("# reference:"). A DLS file is also tested after a round trip (DLS => sf2 => DLS).

if [ $# -lt 2 ] || [ $# -gt 3 ]; then
    echo "Usage: $0 <polyphone executable> <output directory> [threshold in dBFS]"
    exit 1
fi
polyphone=$1
outputDir=$2
threshold=${3:--90}
testsDir="$(cd "$(dirname "$0")" && pwd)"
filesDir="$outputDir/files"
export QT_QPA_PLATFORM=offscreen
runner=""
if command -v taskset > /dev/null; then
    runner="taskset -c 0,1"
fi

# Files of the tests: generated, exported to DLS, DLS files converted to sf2 and exported again
rm -rf "$filesDir" "$outputDir/reference" "$outputDir/converted" "$outputDir/roundtrip"
mkdir -p "$filesDir/roundtrip" "$outputDir/reference" "$outputDir/converted" "$outputDir/roundtrip" || exit 1
python3 "$testsDir/make_dls_test_files.py" "$filesDir" || exit 1
"$polyphone" -6 -i "$filesDir/dls-features.sf2" -d "$filesDir" > /dev/null || echo "FAILED  export of dls-features.sf2"
for dls in "$filesDir"/*.dls; do
    name=$(basename "$dls" .dls)
    "$polyphone" -1 -i "$dls" -d "$filesDir/roundtrip" > /dev/null &&
    "$polyphone" -6 -i "$filesDir/roundtrip/$name.sf2" -d "$filesDir/roundtrip" > /dev/null || echo "FAILED  round trip of $name.dls"
done

# Renderings
failures=0
for case in "$testsDir"/cases/*.txt; do
    name=$(basename "$case" .txt)
    soundfont=$(sed -n 's/^# soundfont: *//p' "$case" | head -n 1)
    reference=$(sed -n 's/^# reference: *//p' "$case" | head -n 1)
    for variant in reference converted roundtrip; do
        case $variant in
        reference) file="$filesDir/$reference" ;;
        converted) file="$filesDir/$soundfont" ;;
        roundtrip) file="$filesDir/roundtrip/$soundfont" ;;
        esac
        [ -f "$file" ] || continue
        if ! $runner "$polyphone" -5 -i "$file" -c "$case" -d "$outputDir/$variant" -o "$name" > "$outputDir/$variant/$name.log" 2>&1; then
            echo "FAILED  $variant rendering of $name (see $outputDir/$variant/$name.log)"
            failures=$((failures + 1))
        fi
    done
done

for variant in converted roundtrip; do
    echo "== $variant"
    python3 "$testsDir/../render/compare.py" "$outputDir/reference" "$outputDir/$variant" --threshold "$threshold" \
        --cases "$testsDir/cases" || failures=$((failures + 1))
done

exit $((failures > 0))
