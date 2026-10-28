#!/bin/bash
# Render every case of tests/render/cases with a Polyphone executable.
# Usage: render.sh <polyphone executable> <soundfont directory> <output directory> [two_cores|threaded]
# The soundfont of a case is named on its first line: "# soundfont: <file name>".
# The synth core computes the voices in the calling thread, next to a second synth core with other
# settings if "two_cores" is given, or the synth of the editor with its threads if "threaded" is given.

if [ $# -ne 3 ] && { [ $# -ne 4 ] || { [ "$4" != "two_cores" ] && [ "$4" != "threaded" ]; }; }; then
    echo "Usage: $0 <polyphone executable> <soundfont directory> <output directory> [two_cores|threaded]"
    exit 1
fi
polyphone=$1
soundfontDir=$2
outputDir=$3
option=""
if [ $# -eq 4 ]; then
    option="|$4"
fi
testsDir="$(cd "$(dirname "$0")" && pwd)"
casesDir="$testsDir/cases"
mkdir -p "$outputDir" || exit 1

# The soundfont of the tests is generated if it is not in the directory
if [ ! -f "$soundfontDir/polyphone-test.sf2" ]; then
    python3 "$testsDir/make_test_soundfont.py" "$soundfontDir/polyphone-test.sf2" || exit 1
fi

# No window is needed, and two CPUs give a single sound engine thread to the synth of the editor:
# the voices are always added in the same order and a rendering is reproducible bit for bit
export QT_QPA_PLATFORM=offscreen
runner=""
if command -v taskset > /dev/null; then
    runner="taskset -c 0,1"
fi

failures=0
for case in "$casesDir"/*.txt; do
    name=$(basename "$case" .txt)
    soundfont=$(sed -n 's/^# soundfont: *//p' "$case" | head -n 1)
    if [ ! -f "$soundfontDir/$soundfont" ]; then
        echo "SKIPPED $name: $soundfontDir/$soundfont not found"
        failures=$((failures + 1))
        continue
    fi

    start=$(date +%s%N)
    if $runner "$polyphone" -5 -i "$soundfontDir/$soundfont" -c "$case$option" -d "$outputDir" -o "$name" > "$outputDir/$name.log" 2>&1; then
        printf "OK      %-32s %d ms\n" "$name" $((($(date +%s%N) - start) / 1000000))
    else
        echo "FAILED  $name (see $outputDir/$name.log)"
        failures=$((failures + 1))
    fi
done

exit $((failures > 0))
