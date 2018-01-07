tail -n 15 /tmp/out-fairfuzz/*/fuzzed-ids.log | cut -d' ' -f 4,11  | sort  # |uniq
