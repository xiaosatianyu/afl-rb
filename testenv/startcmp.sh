
target="/home/xiaosatianyu/workspace/git/fuzz/rdfuzz/testenv/binary-test/CVE-2017-5969/xmllint --valid --recover @@"

x-terminal-emulator -e "`pwd`/runrdfuzz.sh $target"   &
x-terminal-emulator -e "`pwd`/runaflrb.sh $target"   &
x-terminal-emulator -e "`pwd`/runaflgo.sh $target"   &
x-terminal-emulator -e "`pwd`/runafl2.5.2.sh $target"   &
