try () {
    expected="$1"
    input="$2"

    ./shun2cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" != "$expected" ]; then
        echo "$input expected, but got $actual"
        exit 1
    fi
}

try 0 0
try 42 42
try 21 '5+20-4'
try 255 '5-6'

echo OK