#!/bin/sh

fail() {
    echo -e "❌ FAIL: $*"
    exit 1
}

assert_eq() {
    local got="$1"; shift
    local want="$1"; shift

    if [ "${got}" != "${want}" ]; then
        fail "$*\n    Got: ${got}\n   Want: ${want}"
    fi
}

LLBIN=${LLBIN:-.}

CONVERT="${LLBIN}/convert"
if [ ! -e "${CONVERT}" ]; then
    fail "missing executable ${CONVERT}"
fi

assert_eq "$(echo "xyz" | ${CONVERT})" "Xyz" "convert output"
assert_eq "$(echo "h4x0r" | ${CONVERT})" "h4X0r" "convert output"

echo "✅ OK (convert)"

TR="${LLBIN}/tr"
if [ ! -e "${TR}" ]; then
    fail "missing executable ${TR}"
fi

assert_eq "$(echo "bonenoremi" | ${TR} aeiou aaaaa)" "bananarama" "tr output"
assert_eq "$(echo "oiseau" | ${TR} aeiou ooooo)" "oosooo" "tr output"

echo "✅ OK (truncate)"
